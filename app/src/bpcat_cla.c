/*
 * NASA Docket No. GSC-18,587-1 and identified as “The Bundle Protocol Core Flight
 * System Application (BP) v6.5”
 *
 * Copyright © 2020 United States Government as represented by the Administrator of
 * the National Aeronautics and Space Administration. All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include "bpcat_cla.h"

#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define BPCAT_CLA_IN_PORT              4501
#define BPCAT_CLA_OUT_PORT             4551
#define BPCAT_CLA_TIMEOUT              100

/*******************************************************************************
* CLA Out Task Implementation 
*/
BPCat_Status_t CLAOutSetup()
{
    uint8_t buffer[4096] = {0};
    size_t outSize;
    int sock_fd;
    struct sockaddr_in server_addr;

    sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock_fd < 0)
    {
        perror("socket()");
        return BPCAT_SOCKET_ERR;
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4551);
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    return BPCAT_SUCCESS;
}

BPCat_Status_t CLAOutTeardown()
{
    return BPCAT_SUCCESS;
}

void* CLAOutTaskFunc()
{
    int rc;

    while(true)
    {
        BPLib_CLA_Egress(&BPLibInst, 0, buffer, &outSize, 4096, 100);
        rc = sendto(sock_fd, buffer, outSize, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
        if (rc < 0)
        {
            perror("sendto()");
            return NULL;
        }
    }
    return NULL;
}

/*******************************************************************************
* CLA In Task Implementation 
*/
BPCat_Status_t CLAInSetup()
{
    int sock_fd;
    struct sockaddr_in bind_addr;
    uint8_t buffer[BPCAT_CLA_BUFLEN] = {0};
    struct pollfd pfd;
    int bytes_rx, poll_rc;

    /* Create a UDP socket for the RX link */
    sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock_fd < 0)
    {
        perror("socket()");
        return BPCAT_SOCKET_ERR;
    }

    /* Bind the socket to localhost */
    memset(&bind_addr, 0, sizeof(struct sockaddr_in));
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    bind_addr.sin_port = htons(12345);
    if (bind(sock_fd, (const struct sockaddr*)&bind_addr,
        sizeof(struct sockaddr))) 
    {
        perror("bind()");
        close(sock_fd);
        return BPCAT_SOCKET_ERR;
    }

    return BPCAT_SUCCESS;
}

BPCat_Status_t CLAInTeardown()
{
    return BPCAT_SUCCESS;
}

void* CLAInTaskFunc(BPCat_AppData_t* AppData);
{
    while(AppData->Running)
    {
        pfd.fd = sock_fd;
        pfd.events = POLLIN;
        poll_rc = poll(&pfd, 1, 100);
        if (poll_rc > 0)
        {
            bytes_rx = recv(sock_fd, buffer, 4096, 0);
            if (bytes_rx < 0)
            {
                perror("recv()");
            }
            else
            {
                /* In this Ticket, Ingress is disabled to simplify the review process */
                printf("UDP Recv %lu bytes\n", bytes_rx);
                // if (BPLib_CLA_Ingress(&BPLibInst, 0, buffer, bytes_rx, 0) != BPLIB_SUCCESS)
                // {
                //     printf("Ingress fail\n");
                // }
            }
        }
        else if (poll_rc < 0)
        {
            perror("poll()");
            return NULL;
        }
    }

    return NULL;
}
