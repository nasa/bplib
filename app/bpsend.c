/************************************************************************
 * File: bpsend.c
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
#define DFLT_SRC_NODE   4
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
 * reader_thread - Reads data from stdin and stores as bundles
 */
static void* reader_thread (void* parm)
{
    #define LINE_STR_SIZE 1024
    static char line_buffer[LINE_STR_SIZE];
    
    thread_parm_t* info = (thread_parm_t*)parm;

    /* Reader Loop */
    while(app_running)
    {
        fprintf(stderr, "$ ");
        char* payload = fgets(line_buffer, LINE_STR_SIZE, stdin);
        if(payload)
        {
            uint32_t flags;
            int payload_len = strnlen(line_buffer, LINE_STR_SIZE);
            int lib_status = bplib_store(info->bpc, payload, payload_len, BP_CHECK, &flags);
            if(lib_status != BP_SUCCESS)
            {
                fprintf(stderr, "Failed (%d) to store payload [%08X]\n", lib_status, flags);
            }
        }
        else
        {
            fprintf(stderr, "EOF detected... exiting reader thread\n");
            break;
        }
    }

    return NULL;
}

/*
 * writer_thread - Loads bundles from storage and writes them to socket
 */
static void* writer_thread (void* parm)
{
    thread_parm_t* info = (thread_parm_t*)parm;

    int sock = SOCK_INVALID;

    /* Write Loop */
    while(app_running)
    {
        uint8_t* bundle = NULL;
        int bundle_size = 0;
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

        /* Load Bundle */
        int lib_status = bplib_load(info->bpc, (void**)&bundle, &bundle_size, BPLIB_TIMEOUT, &flags);
        if(lib_status == BP_SUCCESS)
        {
            /* Send Bundle */
            int bytes_sent = socksend(sock, bundle, bundle_size, SOCK_TIMEOUT);
            if(bytes_sent != bundle_size)
            {
                fprintf(stderr, "Failed (%d) to send bundle over socket: %s\n", bytes_sent, strerror(errno));
            }            

            /* Acknowledge bundle */
            bplib_ackbundle(info->bpc, bundle);
        }
        else if(lib_status != BP_TIMEOUT)
        {
            fprintf(stderr, "Failed (%d) to load bundle [%08X]\n", lib_status, flags);
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
    bool is_server = false;
    int src_node = DFLT_SRC_NODE, src_serv = DFLT_SRC_SERV;
    int dst_node = 0, dst_serv = 0;

    char ip_addr[PARM_STR_SIZE] = DFLT_IP_ADDR;
    int port = DFLT_PORT;

    int timeout = BP_DEFAULT_TIMEOUT;
    int lifetime = BP_DEFAULT_LIFETIME;

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

    /* Initialize from Environment */
    char* sys_src_node_str = getenv("BP_SEND_NODE");
    if(sys_src_node_str) src_node = (int)strtol(sys_src_node_str, NULL, 0);

    char* sys_src_serv_str = getenv("BP_SEND_SERVICE");
    if(sys_src_serv_str) src_serv = (int)strtol(sys_src_serv_str, NULL, 0);

    /* Display Welcome Banner */
    fprintf(stderr, "\n***************************************************************************");
    fprintf(stderr, "\n                              BP Send                                      ");
    fprintf(stderr, "\n***************************************************************************");
    fprintf(stderr, "\n bpsend [options] ipn:<node>.<service> udp://<ip address>:<port>           ");
    fprintf(stderr, "\n   --server: listens for connections on provided ip address and port       ");
    fprintf(stderr, "\n   --client: connects to provided ip address and port [DEFAULT]            ");
    fprintf(stderr, "\n   --node <n>: overrides local node number of BP agent to n                ");
    fprintf(stderr, "\n   --service <s>: overrides local service number of BP agent to s          ");
    fprintf(stderr, "\n   --timeout <t>: sets timeout of BP agent to t                            ");
    fprintf(stderr, "\n   --lifetime <l>: sets lifetime of BP agent to l                          ");
    fprintf(stderr, "\n                                                                           ");
    fprintf(stderr, "\n   Creates a local BP agent with a source endpoint ID of:                  ");
    fprintf(stderr, "\n                                                                           ");
    fprintf(stderr, "\n         ipn:BP_SEND_NODE.BP_SEND_SERVICE                                  ");
    fprintf(stderr, "\n                                                                           ");
    fprintf(stderr, "\n   where BP_SEND_NODE, and BP_SEND_SERVICE are sytem environment variables ");
    fprintf(stderr, "\n   optionally overriden by the --node and --service parameters.            ");
    fprintf(stderr, "\n                                                                           ");
    fprintf(stderr, "\n   A connection is made to the ip address and port number provided on the  ");
    fprintf(stderr, "\n   command line, and anything read from stdin is bundled and sent to the   ");
    fprintf(stderr, "\n   destination endpoint ID specified by the ipn address provided on the    ");
    fprintf(stderr, "\n   command line.                                                           ");
    fprintf(stderr, "\n                                                                           ");
    fprintf(stderr, "\n   Example usage:                                                          ");
    fprintf(stderr, "\n                                                                           ");
    fprintf(stderr, "\n         ./bpsend --client ipn:1.5 udp://10.1.215.5:34404                  ");
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
        else if(strcmp(argv[i],"--node") == 0)
        {
            src_node = (int)strtol(argv[++i], NULL, 0);
        }
        else if(strcmp(argv[i],"--service") == 0)
        {
            src_serv = (int)strtol(argv[++i], NULL, 0);
        }
        else if(strstr(argv[i], "ipn") != NULL)
        {
            char* serv_str = strrchr(parm, '.');
            *serv_str = '\0';
            serv_str++;
            dst_serv = (int)strtol(serv_str, NULL, 0);

            char* node_str = strrchr(parm, ':');
            node_str++;
            dst_node = (int)strtol(node_str, NULL, 0);
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
    fprintf(stderr, "Creating %s BP agent at ipn:%d.%d and sending bundles to ipn:%d.%d over udp://%s:%d\n", is_server ? "server" : "client", src_node, src_serv, dst_node, dst_serv, ip_addr, port);

    /* Initialize bplib */
    bplib_init();
    bplib_store_ram_init();

    /* Create bplib Channel */
    bp_route_t route = { src_node, src_serv, dst_node, dst_serv, 0, 0 };

    bp_attr_t attributes;
    bplib_attrinit(&attributes);
    attributes.lifetime = lifetime;
    attributes.timeout = timeout;
    attributes.cid_reuse = true;

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