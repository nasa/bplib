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

#define BPCAT_CLA_TIMEOUT              100
#define BPCAT_CLA_BUFLEN               4096

/* These configurations are going to be greatly improved
** once there is a) time to do so and b) an ability to pass
** config into BPCat. For now, keeping them static and hardcoded
** is ok for our testing purposes.
*/
typedef struct CLAOutConfig
{
    int SockFd;
    uint16_t TxPort;
} CLAOutConfig_t;

typedef struct CLAInConfig
{
    int SockFd;
    uint16_t RxPort;
} CLAInConfig_t;

static CLAOutConfig_t TxCLAConfig = {
    .SockFd = -1,
    .TxPort = 4551,
};

static CLAInConfig_t RxCLAConfig = {
    .SockFd = -1,
    .RxPort = 4501,
};

/*******************************************************************************
* CLA Out Task Implementation 
*/
BPCat_Status_t BPCat_CLAOutSetup()
{
    int SockFd;
    SockFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (SockFd < 0)
    {
        perror("socket()");
        return BPCAT_SOCKET_ERR;
    }

    printf("Setup CLA Egress UDP socket on 127.0.0.1:%u\n", TxCLAConfig.TxPort);
    TxCLAConfig.SockFd = SockFd;
    return BPCAT_SUCCESS;
}

BPCat_Status_t BPCat_CLAOutTeardown()
{
    close(TxCLAConfig.SockFd);
    TxCLAConfig.SockFd = -1;
    return BPCAT_SUCCESS;
}

void* BPCat_CLAOutTaskFunc(BPCat_AppData_t* AppData)
{
    BPLib_Status_t EgressStatus;
    int rc;
    uint8_t buffer[BPCAT_CLA_BUFLEN] = {0};
    size_t OutSize;
    struct sockaddr_in ServerAddr;

    /* Pre-populate the ServerAddr here to prevent doing it in the run loop */
    memset(&ServerAddr, 0, sizeof(ServerAddr));
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(TxCLAConfig.TxPort);
    ServerAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    while(AppData->Running)
    {
        EgressStatus = BPLib_CLA_Egress(&AppData->BPLibInst, 0, buffer, &OutSize, BPCAT_CLA_BUFLEN,
            BPCAT_CLA_TIMEOUT);
        if (EgressStatus == BPLIB_SUCCESS)
        {
            rc = sendto(TxCLAConfig.SockFd, buffer, OutSize, 0, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr));
            if (rc < 0)
            {
                perror("sendto()");
                return NULL;
            }
        }
    }
    return NULL;
}

/*******************************************************************************
* CLA In Task Implementation 
*/
BPCat_Status_t BPCat_CLAInSetup()
{
    int SockFd;
    struct sockaddr_in BindAddr;

    /* Create a UDP socket for the RX link */
    SockFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (SockFd < 0)
    {
        perror("socket()");
        return BPCAT_SOCKET_ERR;
    }

    /* Bind the socket to localhost */
    memset(&BindAddr, 0, sizeof(struct sockaddr_in));
    BindAddr.sin_family = AF_INET;
    BindAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    BindAddr.sin_port = htons(RxCLAConfig.RxPort);
    if (bind(SockFd, (const struct sockaddr*)&BindAddr,
        sizeof(struct sockaddr))) 
    {
        perror("bind()");
        close(SockFd);
        return BPCAT_SOCKET_ERR;
    }

    printf("Setup CLA Ingress UDP socket on 127.0.0.1:%u\n", RxCLAConfig.RxPort);
    RxCLAConfig.SockFd = SockFd;
    return BPCAT_SUCCESS;
}

BPCat_Status_t BPCat_CLAInTeardown()
{
    close(RxCLAConfig.SockFd);
    RxCLAConfig.SockFd = -1;
    return BPCAT_SUCCESS;
}

void* BPCat_CLAInTaskFunc(BPCat_AppData_t* AppData)
{
    uint8_t buffer[BPCAT_CLA_BUFLEN] = {0};
    struct pollfd pfd;
    int BytesRx, PollRc;

    while(AppData->Running)
    {
        pfd.fd = RxCLAConfig.SockFd;
        pfd.events = POLLIN;
        PollRc = poll(&pfd, 1, BPCAT_CLA_TIMEOUT);
        if (PollRc > 0)
        {
            BytesRx = recv(RxCLAConfig.SockFd, buffer, BPCAT_CLA_BUFLEN, 0);
            if (BytesRx > 0)
            {
                if (BPLib_CLA_Ingress(&AppData->BPLibInst, 0, buffer, BytesRx, 0) != BPLIB_SUCCESS)
                {
                    fprintf(stderr, "BPLib_CLA_Ingress Fail\n");
                }
            }
            else if (BytesRx < 0)
            {
                perror("recv()");
                return NULL;
            }
        }
        else if (PollRc < 0)
        {
            perror("poll()");
            return NULL;
        }
    }

    return NULL;
}
