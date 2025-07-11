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
    struct sockaddr_in ServerAddr;    
} CLAOutConfig_t;

typedef struct CLAInConfig
{
    int SockFd;
} CLAInConfig_t;

static CLAOutConfig_t TxCLAConfig;
static CLAInConfig_t RxCLAConfig;

/*******************************************************************************
* CLA Out Task Implementation 
*/
BPCat_Status_t BPCat_CLAOutSetup(uint32_t TaskId)
{
    int SockFd;
    
    /* Pre-populate the ServerAddr here to prevent doing it in the run loop */
    memset(&(TxCLAConfig.ServerAddr), 0, sizeof(TxCLAConfig.ServerAddr));
    TxCLAConfig.ServerAddr.sin_family = AF_INET;
    TxCLAConfig.ServerAddr.sin_addr.s_addr = inet_addr(AppData.ConfigPtrs.ContactsConfigPtr->ContactSet[TaskId].ClaOutAddr);
    TxCLAConfig.ServerAddr.sin_port = htons(AppData.ConfigPtrs.ContactsConfigPtr->ContactSet[TaskId].ClaOutPort);

    SockFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (SockFd < 0)
    {
        perror("socket()");
        return BPCAT_SOCKET_ERR;
    }

    printf("Setup CLA Egress UDP socket on %s:%u\n", AppData.ConfigPtrs.ContactsConfigPtr->ContactSet[TaskId].ClaOutAddr,
                                                      AppData.ConfigPtrs.ContactsConfigPtr->ContactSet[TaskId].ClaOutPort);
    TxCLAConfig.SockFd = SockFd;
    return BPCAT_SUCCESS;
}

BPCat_Status_t BPCat_CLAOutTeardown(uint32_t TaskId)
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

    while(AppData->Running)
    {
        EgressStatus = BPLib_CLA_Egress(&AppData->BPLibInst, 0, buffer, &OutSize, BPCAT_CLA_BUFLEN,
            BPCAT_CLA_TIMEOUT);
        if (EgressStatus == BPLIB_SUCCESS)
        {
            rc = sendto(TxCLAConfig.SockFd, buffer, OutSize, 0, 
                (struct sockaddr*)&(TxCLAConfig.ServerAddr), sizeof(TxCLAConfig.ServerAddr));
            if (rc < 0)
            {
                perror("sendto()");
                return NULL;
            }
        }
        else if (EgressStatus != BPLIB_CLA_TIMEOUT)
        {
            fprintf(stderr, "Error egressing, RC=%d\n", EgressStatus);
        }
    }
    return NULL;
}

/*******************************************************************************
* CLA In Task Implementation 
*/
BPCat_Status_t BPCat_CLAInSetup(uint32_t TaskId)
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
    BindAddr.sin_addr.s_addr = inet_addr(AppData.ConfigPtrs.ContactsConfigPtr->ContactSet[TaskId].ClaInAddr);
    BindAddr.sin_port = htons(AppData.ConfigPtrs.ContactsConfigPtr->ContactSet[TaskId].ClaInPort);
    if (bind(SockFd, (const struct sockaddr*)&BindAddr,
        sizeof(struct sockaddr))) 
    {
        perror("bind()");
        close(SockFd);
        return BPCAT_SOCKET_ERR;
    }

    printf("Setup CLA Ingress UDP socket on %s:%u\n", AppData.ConfigPtrs.ContactsConfigPtr->ContactSet[TaskId].ClaInAddr,
                                                      AppData.ConfigPtrs.ContactsConfigPtr->ContactSet[TaskId].ClaInPort);
    RxCLAConfig.SockFd = SockFd;
    return BPCAT_SUCCESS;
}

BPCat_Status_t BPCat_CLAInTeardown(uint32_t TaskId)
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
    BPLib_Status_t BpStatus;

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
                BpStatus = BPLib_CLA_Ingress(&AppData->BPLibInst, 0, buffer, BytesRx, 0);
                if (BpStatus != BPLIB_SUCCESS)
                {
                    fprintf(stderr, "BPLib_CLA_Ingress Fail RC=%d\n", BpStatus);
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
