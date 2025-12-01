/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking 
 * (DTN) Bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this 
 * file except in compliance with the License. You may obtain a copy of the License at 
 *
 * http://www.apache.org/licenses/LICENSE-2.0 
 *
 * Unless required by applicable law or agreed to in writing, software distributed under 
 * the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF 
 * ANY KIND, either express or implied. See the License for the specific language 
 * governing permissions and limitations under the License. The copyright notice to be 
 * included in the software is as follows: 
 *
 * Copyright 2025 United States Government as represented by the Administrator of the 
 * National Aeronautics and Space Administration. All Rights Reserved.
 *
 */

#include "bplib_nc_rwlock.h"

BPLib_Status_t BPLib_NC_RWLock_Init(BPLib_NC_RWLock_t *RWLock)
{
    if (RWLock == NULL)
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    if (pthread_mutex_init(&RWLock->Lock, NULL) != 0)
    {
        return BPLIB_OS_ERROR;
    }

    if (pthread_cond_init(&RWLock->ReadCond, NULL) != 0)
    {
        pthread_mutex_destroy(&RWLock->Lock);
        return BPLIB_OS_ERROR;
    }

    if (pthread_cond_init(&RWLock->WriteCond, NULL) != 0)
    {
        pthread_mutex_destroy(&RWLock->Lock);
        pthread_cond_destroy(&RWLock->ReadCond);
        return BPLIB_OS_ERROR;
    }

    RWLock->ReaderCnt = 0;
    RWLock->WriterCnt = 0;

    return BPLIB_SUCCESS;
}

void BPLib_NC_RWLock_Destroy(BPLib_NC_RWLock_t *RWLock)
{
    if (RWLock == NULL)
    {
        return;
    }

    pthread_mutex_destroy(&RWLock->Lock);
    pthread_cond_destroy(&RWLock->ReadCond);
    pthread_cond_destroy(&RWLock->WriteCond);
}

void BPLib_NC_RWLock_RLock(BPLib_NC_RWLock_t *RWLock)
{
    if (RWLock == NULL)
    {
        return;
    }

    pthread_mutex_lock(&RWLock->Lock);

    while (RWLock->WriterCnt > 0)
    {
        pthread_cond_wait(&RWLock->ReadCond, &RWLock->Lock);
    }

    RWLock->ReaderCnt++;

    pthread_mutex_unlock(&RWLock->Lock);
}

void BPLib_NC_RWLock_RUnlock(BPLib_NC_RWLock_t *RWLock)
{
    if (RWLock == NULL)
    {
        return;
    }

    pthread_mutex_lock(&RWLock->Lock);

    RWLock->ReaderCnt--;

    if (RWLock->ReaderCnt == 0)
    {
        pthread_cond_signal(&RWLock->WriteCond);
    }

    pthread_mutex_unlock(&RWLock->Lock);
}

void BPLib_NC_RWLock_WLock(BPLib_NC_RWLock_t *RWLock)
{
    if (RWLock == NULL)
    {
        return;
    }

    pthread_mutex_lock(&RWLock->Lock);

    while (RWLock->ReaderCnt > 0 || RWLock->WriterCnt > 0)
    {
        pthread_cond_wait(&RWLock->WriteCond, &RWLock->Lock);
    }

    RWLock->WriterCnt++;

    pthread_mutex_unlock(&RWLock->Lock);
}

void BPLib_NC_RWLock_WUnlock(BPLib_NC_RWLock_t *RWLock)
{
    if (RWLock == NULL)
    {
        return;
    }

    pthread_mutex_lock(&RWLock->Lock);

    RWLock->WriterCnt--;

    pthread_cond_broadcast(&RWLock->ReadCond);
    pthread_cond_signal(&RWLock->WriteCond);

    pthread_mutex_unlock(&RWLock->Lock);
}
