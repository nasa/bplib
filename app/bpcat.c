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
#include <string.h>
#include <getopt.h>
#include <poll.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "osapi.h"

#include "bplib.h"

#define NUM_GEN_WORKER 1
#define MEM_ALLOC_BYTES 8000000

static BPLib_Instance_t            BplibInst;
static pthread_t cla_in_thr;
static pthread_t cla_out_thr;
static pthread_t gen_worker_thr[NUM_GEN_WORKER];

/* Returns current monotonic time */
int64_t BPA_TIMEP_GetMonotonicTime(void)
{
    struct timespec ts;

    printf("get mono\n");
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
    {
        perror("clock_gettime MONOTONIC");
        return 0;
    }
    return (int64_t)(ts.tv_sec) * 1000 + (int64_t)(ts.tv_nsec) / 1000000;
}

/* Returns current host time */
int64_t BPA_TIMEP_GetHostTime(void)
{
    struct timeval tv;

    printf("get host\n");
    if (gettimeofday(&tv, NULL) == -1) {
        perror("gettimeofday");
        return 0;
    }

    return (int64_t)(tv.tv_sec) * 1000 + (int64_t)(tv.tv_usec) / 1000;
}

/* Returns host time epoch */
void BPA_TIMEP_GetHostEpoch(BPLib_TIME_Epoch_t *Epoch)
{
    if (Epoch != NULL)
    {
        Epoch->Year   = 1970;
        Epoch->Day    = 1;
        Epoch->Hour   = 1;
        Epoch->Minute = 0;
        Epoch->Second = 0;
        Epoch->Msec   = 0;
    }
    return;
}

/* Returns current host clock state */
BPLib_TIME_ClockState_t BPA_TIMEP_GetHostClockState(void)
{
    return BPLIB_TIME_CLOCK_VALID;
}

void BPA_PERFLOGP_Entry(uint32_t PerfLogID)
{

}

void BPA_PERFLOGP_Exit(uint32_t PerfLogID)
{

}

BPLib_Status_t BPA_TABLEP_SingleTableUpdate(int16_t TblHandle)
{
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPA_EVP_Init()
{
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPA_EVP_SendEvent(uint16_t EventID, BPLib_EM_EventType_t EventType, char const* Spec)
{
    printf("%s\n", Spec);
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPA_TLMP_SendNodeMibConfigPkt(BPLib_NodeMibConfigHkTlm_Payload_t* NodeMIBConfigTlmPayload)
{
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPA_TLMP_SendPerSourceMibConfigPkt(BPLib_SourceMibConfigHkTlm_Payload_t* SrcMIBConfigTlmPayload)
{
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPA_TLMP_SendNodeMibCounterPkt(BPLib_NodeMibCountersHkTlm_Payload_t* NodeMIBCounterTlmPayload)
{
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPA_TLMP_SendPerSourceMibCounterPkt(BPLib_SourceMibCountersHkTlm_Payload_t* SrcMIBCounterTlmPayload)
{
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPA_TLMP_SendChannelContactPkt(BPLib_ChannelContactStatHkTlm_Payload_t* ChannelContactTlmPayload)
{
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPA_TLMP_SendStoragePkt(BPLib_StorageHkTlm_Payload_t* StorTlmPayload)
{
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPA_ADUP_AddApplication(uint8_t ChanId)
{
    printf("Add app called\n");
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPA_ADUP_StartApplication(uint8_t ChanId)
{
    printf("Start App\n");
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPA_ADUP_StopApplication(uint8_t ChanId)
{
    printf("Stop App\n");
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPA_ADUP_RemoveApplication(uint8_t ChanId)
{
    printf("Remove App\n");
    return BPLIB_SUCCESS;
}

static BPLib_FWP_ProxyCallbacks_t Callbacks = {
    /* Time Proxy */
    .BPA_TIMEP_GetMonotonicTime = BPA_TIMEP_GetMonotonicTime,
    .BPA_TIMEP_GetHostEpoch = BPA_TIMEP_GetHostEpoch,
    .BPA_TIMEP_GetHostClockState = BPA_TIMEP_GetHostClockState,
    .BPA_TIMEP_GetHostTime = BPA_TIMEP_GetHostTime,
    /* Perf Log Proxy */
    .BPA_PERFLOGP_Entry = BPA_PERFLOGP_Entry,
    .BPA_PERFLOGP_Exit = BPA_PERFLOGP_Exit,
    /* Table Proxy */
    .BPA_TABLEP_SingleTableUpdate = BPA_TABLEP_SingleTableUpdate,
    /* Event Proxy */
    .BPA_EVP_Init = BPA_EVP_Init,
    .BPA_EVP_SendEvent = BPA_EVP_SendEvent,
    /* ADU Proxy */
    .BPA_ADUP_AddApplication = BPA_ADUP_AddApplication,
    .BPA_ADUP_StartApplication = BPA_ADUP_StartApplication,
    .BPA_ADUP_StopApplication = BPA_ADUP_StopApplication,
    .BPA_ADUP_RemoveApplication = BPA_ADUP_RemoveApplication,
    /* Telemetry Proxy */
    .BPA_TLMP_SendNodeMibConfigPkt = BPA_TLMP_SendNodeMibConfigPkt,
    .BPA_TLMP_SendPerSourceMibConfigPkt = BPA_TLMP_SendPerSourceMibConfigPkt,
    .BPA_TLMP_SendNodeMibCounterPkt = BPA_TLMP_SendNodeMibCounterPkt,
    .BPA_TLMP_SendPerSourceMibCounterPkt = BPA_TLMP_SendPerSourceMibCounterPkt,
    .BPA_TLMP_SendChannelContactPkt = BPA_TLMP_SendChannelContactPkt,
    .BPA_TLMP_SendStoragePkt = BPA_TLMP_SendStoragePkt
};


void* cla_in_loop()
{
    int sock_fd;
    struct sockaddr_in bind_addr;
    uint8_t buffer[4096] = {0};
    struct pollfd pfd;
    int bytes_rx, poll_rc;

    /* Create a UDP socket for the RX link */
    sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock_fd < 0)
    {
        perror("socket()");
        return NULL;
    }

    /* Bind the socket to localhost */
    memset(&bind_addr, 0, sizeof(struct sockaddr_in));
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    bind_addr.sin_port = htons(4501);
    if (bind(sock_fd, (const struct sockaddr*)&bind_addr,
        sizeof(struct sockaddr))) 
    {
        perror("bind()");
        close(sock_fd);
        return NULL;
    }

    while(true)
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
                if (BPLib_CLA_Ingress(&BplibInst, 0, buffer, bytes_rx, 0) != BPLIB_SUCCESS)
                {
                    printf("Ingress fail\n");
                }
            }
        }
        else if (poll_rc < 0)
        {
            perror("poll()");
        }
    }

    return NULL;
}

void* gen_worker_loop()
{
    while (true)
    {
        BPLib_QM_RunJob(&BplibInst, 100);
    }
    return NULL;
}

void* cla_out_loop()
{
    uint8_t buffer[4096] = {0};
    size_t outSize;
    int sock_fd;
    struct sockaddr_in server_addr;

    sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock_fd < 0)
    {
        perror("socket()");
        return NULL;
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4551);
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    while(true)
    {
        
        BPLib_CLA_Egress(&BplibInst, 0, buffer, &outSize, 4096, 100);
        sendto(sock_fd, buffer, outSize, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    }
    return NULL;
}

void BPCat_Main()
{
    BPLib_Status_t InitStatus;

    /* FWP */
    InitStatus = BPLib_FWP_Init(&Callbacks);
    if (InitStatus != BPLIB_SUCCESS)
    {
        printf("Failed to init FWP\n");
        return;
    }

    InitStatus = BPLib_EM_Init();
    if (InitStatus != BPLIB_SUCCESS)
    {
        printf("Failed to init EM\n");
        return;
    }

    /* Time Management */
    // InitStatus = BPLib_TIME_Init();
    // if (InitStatus != BPLIB_SUCCESS)
    // {
    //     printf("Failed to init time\n");
    //     return;
    // }

    InitStatus = BPLib_QM_QueueTableInit(&BplibInst, 1024);
    if (InitStatus != BPLIB_SUCCESS)
    {
        printf("Failed to initialize QM\n");
        return;
    }

    InitStatus = BPLib_MEM_PoolInit(&BplibInst.pool, (void *)calloc(MEM_ALLOC_BYTES, 1),
        (size_t)MEM_ALLOC_BYTES);
    if (InitStatus != BPLIB_SUCCESS)
    {
        printf("Failed to initialize MEM\n");
        return;
    }

    pthread_create(&cla_in_thr, NULL, cla_in_loop, NULL);
    pthread_create(&cla_out_thr, NULL, cla_out_loop, NULL);
    for (int i = 0; i < NUM_GEN_WORKER; i++)
    {
        pthread_create(&gen_worker_thr[i], NULL, gen_worker_loop, NULL);
    }

    while (true)
    {
        BPLib_QM_SortJobs(&BplibInst, 1000);
    }
}

/* We're forced to use OSAL until AS, TIME, and QM implement abstraction */
void OS_Application_Startup()
{
    OS_API_Init();
    BPCat_Main();
}
