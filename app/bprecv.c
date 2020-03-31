/************************************************************************
 * File: bprecv.c
 *
 *  Copyright 2019 United States Government as represented by the
 *  Administrator of the National Aeronautics and Space Administration.
 *  All Other Rights Reserved.
 *
 *  This software was created at NASA's Goddard Space Flight Center.
 *  This software is governed by the NASA Open Source Agreement and may be
 *  used, distributed and modified only pursuant to the terms of that
 *  agreement.
 *
 * Maintainer(s):
 *  Joe-Paul Swinski, Code 582 NASA GSFC
 *
 *************************************************************************/

/*************************************************************************
 * Includes
 *************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#include "bp/bplib.h"
#include "bp/bplib_store_ram.h"

#include "bpsock.h"

/*************************************************************************
 * Defines
 *************************************************************************/

#define PARM_STR_SIZE   64
#define DFLT_SRC_NODE   5
#define DFLT_SRC_SERV   1
#define DFLT_IP_ADDR    "127.0.0.1"
#define DFLT_PORT       34500
#define BPLIB_TIMEOUT   1

/*************************************************************************
 * Typedefs
 *************************************************************************/

typedef struct {
    bp_desc_t* bpc;
    bool is_server;
    const char* ip_addr;
    int port;    
} thread_parm_t;

/*************************************************************************
 * File Data
 *************************************************************************/

static bool app_immediate_abort = false;
static bool app_signal_abort = false;
static bool app_running = true;

static bp_store_t storage_service =
{
    .create     = bplib_store_ram_create,
    .destroy    = bplib_store_ram_destroy,
    .enqueue    = bplib_store_ram_enqueue,
    .dequeue    = bplib_store_ram_dequeue,
    .retrieve   = bplib_store_ram_retrieve,
    .release    = bplib_store_ram_release,
    .relinquish = bplib_store_ram_relinquish,
    .getcount   = bplib_store_ram_getcount,
};

/******************************************************************************
 * Local Functions
 ******************************************************************************/

/*
 * app_quick_exit - Signal handler for Control-C
 */
static void app_quick_exit(int parm)
{
    (void)parm;
    if(app_immediate_abort) exit(0);
    printf("\n...Shutting down!\n");
    app_running = false;
    app_immediate_abort = true; // multiple control-c will exit immediately
}

/*
 * signal_thread - Manages all the SIGINT and SIGTERM signals
 */
static void* signal_thread (void* parm)
{
    sigset_t* signal_set = (sigset_t*)parm;
    
    while(true)
    {
        int sig = 0;
        int status = sigwait(signal_set, &sig);
        if (status != 0)
        {
            printf("Fatal error (%d) ...failed to wait for signal: %s\n", status, strerror(errno));
            signal(SIGINT, app_quick_exit);
            break;
        }
        else if(app_signal_abort)
        {
            break; // exit thread for clean up
        }
        else
        {
            app_quick_exit(0);
        }
    }    
    
    return NULL;
}

/*
 * writer_thread - Accepts payloads and writes them to stdout
 */
static void* writer_thread (void* parm)
{   
    thread_parm_t* info = (thread_parm_t*)parm;

    /* Reader Loop */
    while(app_running)
    {
        char* payload = NULL;
        int payload_size = 0;
        uint32_t flags = 0;

        /* Accept Payload */
        int lib_status = bplib_accept(info->bpc, (void**)&payload, &payload_size, BPLIB_TIMEOUT, &flags);
        if(lib_status == BP_SUCCESS)
        {
            /* Write Line */
            fprintf(stdout, "%s", payload);        

            /* Acknowledge PAyload */
            bplib_ackpayload(info->bpc, payload);
        }
        else if(lib_status != BP_TIMEOUT)
        {
            fprintf(stderr, "Failed (%d) to accept payload [%08X]\n", lib_status, flags);
        }
    }

    return NULL;
}

/*
 * reader_thread - Reads bundles from socket and processes them
 */
static void* reader_thread (void* parm)
{
    static uint8_t bundle[BP_DEFAULT_MAX_LENGTH];

    thread_parm_t* info = (thread_parm_t*)parm;

    int sock = SOCK_INVALID;

    /* Write Loop */
    while(app_running)
    {
        uint32_t flags = 0;

        /* Make Connection */
        if(sock == SOCK_INVALID)
        {
            sock = sockdatagram(info->ip_addr, info->port, info->is_server, NULL);
        }

        /* Check for Connection */
        if(sock == SOCK_INVALID)
        {
            fprintf(stderr, "Connection unavailable... trying again\n");
            sleep(1);
            continue;
        }

        /* Read Socket */
        int bytes_recv = sockrecv(sock, bundle, BP_DEFAULT_MAX_LENGTH, SOCK_TIMEOUT);
        if(bytes_recv > 0)
        {
            int lib_status = bplib_process(info->bpc, bundle, bytes_recv, BP_CHECK, &flags);
            if(lib_status != BP_SUCCESS)
            {
                fprintf(stderr, "Failed (%d) to process bundle [%08X]\n", lib_status, flags);
            }
        }
        else if(bytes_recv != 0)
        {
            fprintf(stderr, "Failed (%d) to receive bundle over socket: %s\n", bytes_recv, strerror(errno));
        }
    }

    /* Close Connection */
    if(sock != SOCK_INVALID)
    {
        sockclose(sock);
    }    

    return NULL;
}

/******************************************************************************
 * Main
 ******************************************************************************/
int main(int argc, char *argv[])
{
    bool is_server = true;
    int src_node = DFLT_SRC_NODE, src_serv = DFLT_SRC_SERV;

    char ip_addr[PARM_STR_SIZE] = DFLT_IP_ADDR;
    int port = DFLT_PORT;

    int dacs_rate = BP_DEFAULT_DACS_RATE;

    int i;
    char parm[PARM_STR_SIZE];

    /* Block SIGINT and SIGTERM for all future threads created */    
    sigset_t signal_set;
    sigemptyset(&signal_set);
    sigaddset(&signal_set, SIGINT);
    sigaddset(&signal_set, SIGTERM);
    pthread_sigmask(SIG_BLOCK, &signal_set, NULL);
    
    /* Create dedicated signal thread to handle SIGINT and SIGTERM */
    pthread_t signal_pid;
    pthread_attr_t signal_pthread_attr;
    pthread_attr_init(&signal_pthread_attr);
    pthread_attr_setdetachstate(&signal_pthread_attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&signal_pid, &signal_pthread_attr, &signal_thread, (void *) &signal_set);    

    /* Display Welcome Banner */
    fprintf(stderr, "\n***************************************************************************");
    fprintf(stderr, "\n                              BP Receive                                   ");
    fprintf(stderr, "\n***************************************************************************");
    fprintf(stderr, "\n bprecv [options] ipn:<node>.<service> udp://<ip address>:<port>           ");
    fprintf(stderr, "\n   --server: listens for connections on ip address and port [DEFAULT]      ");
    fprintf(stderr, "\n   --client: connects to provided ip address and port                      ");
    fprintf(stderr, "\n   --dacsrate <r>: sets DACS rate of BP agent to r                         ");
    fprintf(stderr, "\n                                                                           ");
    fprintf(stderr, "\n   Creates a local BP agent with a local endpoint ID of:                   ");
    fprintf(stderr, "\n                                                                           ");
    fprintf(stderr, "\n         ipn:<node>.<service>                                              ");
    fprintf(stderr, "\n                                                                           ");
    fprintf(stderr, "\n   A connection is made to the ip address and port number provided on the  ");
    fprintf(stderr, "\n   command line, and anything read from the socket treated as a bundle and ");
    fprintf(stderr, "\n   processed by the BP Agent.  All payloads retrieved from the bundles are ");
    fprintf(stderr, "\n   writeen to stdout.                                                      ");
    fprintf(stderr, "\n                                                                           ");
    fprintf(stderr, "\n   Example usage:                                                          ");
    fprintf(stderr, "\n                                                                           ");
    fprintf(stderr, "\n         ./bprecv --server ipn:1.5 udp://10.1.215.5:34404                  ");
    fprintf(stderr, "\n***************************************************************************");
    fprintf(stderr, "\n\n");
       
    /* Process Command Line */
    for(i = 1; i < argc; i++)
    {
        snprintf(parm, PARM_STR_SIZE, "%s", argv[i]);

        if(strcmp(argv[i],"--server") == 0)
        {
            is_server = true;
        }
        else if(strcmp(argv[i],"--client") == 0)
        {
            is_server = false;
        }
        else if(strcmp(argv[i],"--dacsrate") == 0)
        {
            dacs_rate = (int)strtol(argv[++i], NULL, 0);
        }
        else if(strstr(argv[i], "ipn") != NULL)
        {
            char* serv_str = strrchr(parm, '.');
            *serv_str = '\0';
            serv_str++;
            src_serv = (int)strtol(serv_str, NULL, 0);

            char* node_str = strrchr(parm, ':');
            node_str++;
            src_node = (int)strtol(node_str, NULL, 0);
        }
        else if(strstr(argv[i], "udp") != NULL)
        {
            char* port_str = strrchr(parm, ':');
            *port_str = '\0';
            port_str++;
            port = (int)strtol(port_str, NULL, 0);            

            char* ip_str = strrchr(parm, '/');
            ip_str++;
            snprintf(ip_addr, PARM_STR_SIZE, "%s", ip_str);
        }
        else
        {
            fprintf(stderr, "Unrecognized parameter: %s\n", argv[i]);
        }   
    }

    /* Echo Command Line Options */
    fprintf(stderr, "Creating %s BP agent at ipn:%d.%d to receiving bundles over udp://%s:%d\n", is_server ? "server" : "client", src_node, src_serv, ip_addr, port);

    /* Initialize bplib */
    bplib_init();
    bplib_store_ram_init();

    /* Create bplib Channel */
    bp_route_t route = { src_node, src_serv, 0, 0, 0, 0 };

    bp_attr_t attributes;
    bplib_attrinit(&attributes);
    attributes.dacs_rate = dacs_rate;

    bp_desc_t* bpc = bplib_open(route, storage_service, attributes);
    if(bpc == NULL)
    {
        fprintf(stderr, "Failed to create bplib channel... exiting\n");
        return -1;
    }

    /* Create Thread Parameters (note it's on the stack) */
    thread_parm_t thread_parm = { 
        .bpc        = bpc,
        .is_server  = is_server,
        .ip_addr    = ip_addr, 
        .port       = port };

    /* Create Reader Thread */
    pthread_t read_pid;
    pthread_create(&read_pid, NULL, &reader_thread, &thread_parm);    

    /* Create Writer Thread */
    pthread_t write_pid;
    pthread_create(&write_pid, NULL, &writer_thread, &thread_parm);    

    /* Idle Loop */
    while(app_running)
    {
        sleep(1);
    }

    /* Join Threads */
    int read_rc = pthread_join(read_pid, NULL);
    if(read_rc != 0)
    {
        fprintf(stderr, "Failed (%d) to join reader thread: %s\n", read_rc, strerror(read_rc));
    }
 
    int write_rc = pthread_join(write_pid, NULL);
    if(write_rc != 0)
    {
        fprintf(stderr, "Failed (%d) to join writer thread: %s\n", write_rc, strerror(write_rc));
    }

    /* Close bplib Channel */
    bplib_close(bpc);

    return 0;
}