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
#include "bplib_qm_event.h"

#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>

static void ms_to_abstimeout(uint32_t ms, struct timespec *ts)
{
    if (ts == NULL)
    {
        return;
    }

    clock_gettime(CLOCK_REALTIME, ts);
    ts->tv_sec += ms / 1000;
    ts->tv_nsec += (ms % 1000 * 1000000);
    ts->tv_sec += ts->tv_nsec / 1000000000;
    ts->tv_nsec %= 1000000000;
}

void BPLib_QM_EventInit(BPLib_QM_Event_t* Event)
{
    phtread_mutex_init(&Event->Lock);
    pthread_cond_init(&Event->Cond);
    Event->IsSet = false;
}

void BPLib_QM_EventSet(BPLib_QM_Event_t* Event)
{
    pthread_mutex_lock(&Event->Lock);
    pthread_cond_signal(&Event->Cond);
    Event->IsSet = true;
    pthread_mutex_unlock(&Event->Lock);
}

void BPLib_QM_EventClear(BPLib_QM_Event_t* Event)
{
    pthread_mutex_lock(&Event->Lock);
    Event->IsSet = false;
    pthread_mutex_unlock(&Event->Lock);
}

bool BPLib_QM_EventWait(BPLib_QM_Event_t* Event, int TimeoutMs)
{
    struct timespec Deadline;
    int rc;

    if (Event == NULL)
    {
        return false;
    }

    ms_to_abstimeout((uint32_t)TimeoutMs, &Deadline);
    pthread_mutex_lock(&Event->Lock);
    while (Event->IsSet == false)
    {
        rc = pthread_cond_timedwait(&Event->Cond, &Event->Mutex, &Deadline);
        if (rc != 0)
        {
            if (rc != ETIMEDOUT)
            {
                fprintf(stderr, "BPLib_QM_EventWait NON-TIMEOUT ERROR",
                    stderror(rc));
            }
            pthread_mutex_unlock(&Event->Lock);
            return false;
        }
    }
    pthread_mutex_unlock(&Event->Lock);
    return true;
}
