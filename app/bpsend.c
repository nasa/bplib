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

#include "bp/bplib.h"
#include "bpsock.h"

/*************************************************************************
 * Defines
 *************************************************************************/

#define PARM_STR_SIZE  64

/******************************************************************************
 * Main
 ******************************************************************************/
int main(int argc, char *argv[])
{
    bool is_server = false;
    int src_node = 0, src_serv = 0;
    int dst_node = 0, dst_serv = 0;

    bool is_udp = true;
    char ip_addr[PARM_STR_SIZE] = "0.0.0.0";
    int port = 0;

    int i;
    char parm[PARM_STR_SIZE];
    
    fprintf(stderr, "\n***************************************************************************");
    fprintf(stderr, "\n                              BP Send                                      ");
    fprintf(stderr, "\n***************************************************************************");
    fprintf(stderr, "\n bpsend [options] ipn:<node>.<service> <udp|tcp>://<ip address>:<port>     ");
    fprintf(stderr, "\n   --server: listens for connections on provided ip address and port       ");
    fprintf(stderr, "\n   --client: connects to provided ip address and port                      ");
    fprintf(stderr, "\n   --node <n>: overrides local node number of BP agent to n                ");
    fprintf(stderr, "\n   --service <s>: overrides local service number of BP agent to s          ");
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
    fprintf(stderr, "\n   Both a UDP and a TCP convergence layer is support, selected by the      ");
    fprintf(stderr, "\n   protocol specified on the command line.  In the case of TCP, an         ");
    fprintf(stderr, "\n   encapsulation packet header is added to every bundle.                   ");
    fprintf(stderr, "\n                                                                           ");
    fprintf(stderr, "\n   Example usage:                                                          ");
    fprintf(stderr, "\n                                                                           ");
    fprintf(stderr, "\n         ./bpsend --server ipn:1.4 udp://10.1.215.5:34404                  ");
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
            is_udp = true;

            char* port_str = strrchr(parm, ':');
            *port_str = '\0';
            port_str++;
            port = (int)strtol(port_str, NULL, 0);            

            char* ip_str = strrchr(parm, '/');
            ip_str++;
            snprintf(ip_addr, PARM_STR_SIZE, "%s", ip_str);
        }
        else if(strstr(argv[i], "tcp") != NULL)
        {
            is_udp = false;
            
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
    fprintf(stderr, "Creating %s BP agent at ipn:%d.%d and sending bundles to ipn:%d.%d over %s://%s:%d\n", is_server ? "server" : "client", src_node, src_serv, dst_node, dst_serv, is_udp ? "udp" : "tcp", ip_addr, port);

    return 0;
}