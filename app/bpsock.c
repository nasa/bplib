/*
**  Copyright 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
**  Purpose: Abstraction layer for io links and devices
**
**  Reference: sio_sock.h
**
**  Notes:
**
*/

/*************************************************************************
 * Includes
 *************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <assert.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>

#include "sio_sock.h"
#include "sio_link.h"

/******************************************************************************
 * Defines
 ******************************************************************************/

/* Definitions */
#define PORT_STR_LEN    16
#define HOST_STR_LEN    64
#define SERV_STR_LEN    64
#define SOCK_PEND       (-1)
#define SOCK_CHECK      0
#define SOCK_TIMEOUT    1000 // milliseconds

/* Macros */
#if SIO_VERBOSE
#define display_error(...)  printf(__VA_ARGS__)
#else
#define display_error(...) (void)0
#endif

/******************************************************************************
 * Local Functions
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 * sockkeepalive
 *----------------------------------------------------------------------------*/
static int sockkeepalive(int socket_fd, int idle, int cnt, int intvl)
{
    int         optval;
    socklen_t   optlen = sizeof(optval);

    optval = 1; // set SO_KEEPALIVE on a socket to true (1)
    if (setsockopt(socket_fd, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0)
    {
        display_error("Failed to set SO_KEEPALIVE option on socket, %s\n", strerror(errno));
        return SIO_INVALID_LINK;
    }

    optval = idle; // set TCP_KEEPIDLE on a socket to number of seconds the connection is idle before keepalive probes are sent
    if (setsockopt(socket_fd, IPPROTO_TCP, TCP_KEEPIDLE, &optval, optlen) < 0)
    {
        display_error("Failed to set TCP_KEEPIDLE option on socket, %s\n", strerror(errno));
        return SIO_INVALID_LINK;
    }

    optval = cnt; // set TCP_KEEPCNT on a socket to number of keepalive probes before dropping connection
    if (setsockopt(socket_fd, IPPROTO_TCP, TCP_KEEPCNT, &optval, optlen) < 0)
    {
        display_error("Failed to set TCP_KEEPCNT option on socket, %s\n", strerror(errno));
        return SIO_INVALID_LINK;
    }

    optval = intvl; // set TCP_KEEPINTVL on a socket to number of seconds between keepalive probes
    if(setsockopt(socket_fd, IPPROTO_TCP, TCP_KEEPINTVL, &optval, optlen) < 0)
    {
        display_error("Failed to set TCP_KEEPINTVL option on socket, %s\n", strerror(errno));
        return SIO_INVALID_LINK;
    }
    
    return 0;
}

/*----------------------------------------------------------------------------*
 * sockreuse
 *----------------------------------------------------------------------------*/
static int sockreuse(int socket_fd)
{
    int         optval;
    socklen_t   optlen = sizeof(optval);

    optval = 1; // set SO_REUSEADDR on a socket to true (1)
    if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, optlen) < 0)
    {
        display_error("Failed to set SO_REUSEADDR option on socket, %s\n", strerror(errno));
        return SIO_INVALID_LINK;
    }
    
    return 0;
}

/*----------------------------------------------------------------------------*
 * socknonblock
 *----------------------------------------------------------------------------*/
static int socknonblock(int socket_fd)
{

    int flags = fcntl(socket_fd, F_GETFL, 0);
    if(flags < 0 || fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        display_error("Failed to make socket non-blocking, %s\n", strerror(errno));
        return SIO_INVALID_LINK;
    }

    return 0;
}

/******************************************************************************
 * Exported Functions
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 * sockcreate
 *----------------------------------------------------------------------------*/
int sockcreate(int type, const char* ip_addr, int port, int is_server, int* block)
{
    struct addrinfo     hints, *result, *rp;
    int                 sock = SIO_INVALID_LINK;
    int                 status = -1; // start with error condition
    char                portstr[PORT_STR_LEN];
    char                host[HOST_STR_LEN];
    char                serv[SERV_STR_LEN];
    
    /* Check Address */
    if(!ip_addr) ip_addr = "0.0.0.0"; // wildcard
    
    /* Initialize Port */
    snprintf(portstr, PORT_STR_LEN, "%d", port);
    
    /* Get Destination Host */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;    /* IPv4 */
    hints.ai_socktype = type;
    status = getaddrinfo(ip_addr, portstr, &hints, &result);
    if(status != 0) 
    {
        display_error("Failed to get address info for %s:%d, %s\n", ip_addr, port, gai_strerror(status));
        return SIO_INVALID_LINK;
    }

    /* Try each address until we successfully connect. */
    for(rp = result; rp != NULL; rp = rp->ai_next) 
    {
        /* Get address information */
        status = getnameinfo(rp->ai_addr,rp->ai_addrlen, host, HOST_STR_LEN, serv, SERV_STR_LEN, NI_NUMERICHOST | NI_NUMERICSERV);
        
        /* Create socket */
        sock = socket(rp->ai_family, rp->ai_socktype, 0);
        if(sock < 0)
        {
            display_error("Failed to open socket for %s:%s, %s\n", host, serv, strerror(errno));
            continue;
        }

        if(is_server)
        {
            /* Set Reuse Option on Listen Socket */
            if(sockreuse(sock) < 0)
            {
                close(sock);
                continue;
            }

            /* Bind Socket */
            status = bind(sock, rp->ai_addr, rp->ai_addrlen);
            if(status < 0)
            {
                display_error("Failed to bind socket to %s:%s, %s\n", host, serv, strerror(errno));
                close(sock);
            }
            else
            {
                break;
            }
        }
        else
        {
            do {
                /* Connect Socket */
                status = connect(sock, rp->ai_addr, rp->ai_addrlen);
                if(status < 0) 
                {
                    display_error("Failed to connect socket to %s:%s... %s\n", host, serv, strerror(errno));
                    sleep(1);
                }
            } while(status < 0 && block && *block);
            
            /* Check Connection */
            if(status < 0)  close(sock);
            else            break;
        }
    }

    /* Free linked list returned by getaddrinfo */
    freeaddrinfo(result);

    /* Check success of connection */
    if (rp == NULL) return SIO_INVALID_LINK;

    /* Set Keep Alive on Socket */
    if(type == SOCK_STREAM)
    {
        if(sockkeepalive(sock, 60, 12, 5) < 0)
        {
            shutdown(sock, SHUT_RDWR);
            close(sock);
            return SIO_INVALID_LINK;
        }

        /* Make Socket Non-Blocking*/
        if(socknonblock(sock) < 0)
        {
            shutdown(sock, SHUT_RDWR);
            close(sock);
            return SIO_INVALID_LINK;
        }
    }

    /* Return Client TcpSocket */
    return sock;    
}

/*----------------------------------------------------------------------------*
 * sockstream
 *----------------------------------------------------------------------------*/
int sockstream(const char* ip_addr, int port, int is_server, int* block)
{
    /* Create initial socket */
    int sock = sockcreate(SOCK_STREAM, ip_addr, port, is_server, block);
    if(sock == SIO_INVALID_LINK) return SIO_INVALID_LINK;
    
    if(!is_server) // client
    {
        return sock;
    }
    else // server
    {
        int listen_socket = sock;
        int server_socket = SIO_INVALID_LINK;

        /* Make Socket a Listen Socket */
        if(listen(listen_socket, 1) != 0)
        {
            display_error("Failed to mark socket bound to %s:%d as a listen socket, %s\n", ip_addr ? ip_addr : "0.0.0.0", port, strerror(errno));
            close(listen_socket);
            return SIO_INVALID_LINK;
        }

        do {
            /* Build Poll Structure */
            struct pollfd polllist[1];
            polllist[0].fd = listen_socket;
            polllist[0].events = POLLIN;
            polllist[0].revents = 0;

            /* Poll Listener */
            int activity = 0;
            do activity = poll(polllist, 1, 1000);
            while(activity == -1 && (errno == EINTR || errno == EAGAIN));

            /* Accept Connection */
            if((activity > 0) && (polllist[0].revents & POLLIN))
            {
                server_socket = accept(listen_socket, NULL, NULL);
            }
        } while(server_socket == SIO_INVALID_LINK && block && *block);

        /* Shutdown Listen Socket */
        shutdown(listen_socket, SHUT_RDWR);
        close(listen_socket);

        /* Check Acceptance */
        if(server_socket < 0)
        {
            display_error("Failed to accept connection on %s:%d, %s\n", ip_addr ? ip_addr : "0.0.0.0", port, strerror(errno));
            return SIO_INVALID_LINK;
        }

        /* Set Keep Alive on Socket */
        if(sockkeepalive(server_socket, 60, 12, 5) < 0)
        {
            shutdown(server_socket, SHUT_RDWR);
            close(server_socket);
            return SIO_INVALID_LINK;
        }

        /* Make Socket Non-Blocking */
        if(socknonblock(server_socket) < 0)
        {
            shutdown(server_socket, SHUT_RDWR);
            close(server_socket);
            return SIO_INVALID_LINK;
        }

        /* Return Server Socket */
        return server_socket;
    }
}

/*----------------------------------------------------------------------------*
 * sockdatagram
 *----------------------------------------------------------------------------*/
int sockdatagram(const char* ip_addr, int port, int is_server, int* block)
{
    return sockcreate(SOCK_DGRAM, ip_addr, port, is_server, block);
}

/*----------------------------------------------------------------------------*
 * socksend
 *
 * Notes: returns number of bytes written after 1 second of trying
 *----------------------------------------------------------------------------*/
int socksend(int fd, const void* buf, int size, int timeout)
{
    int activity = 1;
    int revents = POLLOUT;
    int c = SIO_TIMEOUT_RC;

    /* Check Sock */
    if(fd == SIO_INVALID_LINK)
    {
        if(timeout != SOCK_CHECK)
        {
            sleep(1);
        }

        return SIO_TIMEOUT_RC;
    }

    /* Build Poll Structure */
    struct pollfd polllist[1];
    polllist[0].fd = fd;
    polllist[0].events = POLLOUT | POLLHUP;
    polllist[0].revents = 0;

    /* Poll */
    do activity = poll(polllist, 1, timeout);
    while(activity == -1 && (errno == EINTR || errno == EAGAIN));

    /* Set Activity */
    if(activity > 0)    revents = polllist[0].revents;
    else                revents = 0;

    /* Perform Send */
    if(revents & POLLHUP)
    {
        c = SIO_INVALID_LINK;
    }
    else if(revents & POLLOUT)
    {
        c = send(fd, buf, size, MSG_DONTWAIT | MSG_NOSIGNAL);
        if(c == 0)
        {
            c = SIO_INVALID_LINK;
        }        
        else if(c < 0)
        {
            display_error("Failed (%d) to send data to ready socket [%0X]: %s\n", c, revents, strerror(errno));
            c = SIO_INVALID_LINK;
        }
    }

    /* Return Results */
    return c;
}

/*----------------------------------------------------------------------------*
 * sockrecv
 *----------------------------------------------------------------------------*/
int sockrecv(int fd, void* buf, int size, int timeout)
{
    int c = SIO_TIMEOUT_RC;
    int revents = POLLIN;

    /* Perform Poll if not Checking */
    struct pollfd polllist[1];
    polllist[0].fd = fd;
    polllist[0].events = POLLIN | POLLHUP;
    polllist[0].revents = 0;

    int activity = 1;
    do activity = poll(polllist, 1, timeout);
    while(activity == -1 && (errno == EINTR || errno == EAGAIN));

    revents = polllist[0].revents;

    /* Perform Receive */
    if(revents & POLLIN)
    {
        c = recv(fd, buf, size, MSG_DONTWAIT);
        if(c == 0)
        {
            c = SIO_INVALID_LINK;
        }
        else if(c < 0)
        {
            display_error("Failed (%d) to receive data from ready socket [%0X]: %s\n", c, revents, strerror(errno));
            c = SIO_INVALID_LINK;
        }
    }
    else if(revents & POLLHUP)
    {
        c = SIO_INVALID_LINK;
    }

    /* Return Results*/
    return c;
}

/*----------------------------------------------------------------------------*
 * sockclose
 *----------------------------------------------------------------------------*/
void sockclose(int fd)
{
    shutdown(fd, SHUT_RDWR);
    close(fd);
}
