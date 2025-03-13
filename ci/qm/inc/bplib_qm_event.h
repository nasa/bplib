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

#ifndef BPLIB_QM_EVENT_H
#define BPLIB_QM_EVENT_H

#include "bplib_api_types.h"

#include <pthread.h>

// I want all the wait constants to be QM_NO_WAIT, QM_WAIT_FOREVER, etc
#define QM_EVENT_NO_WAIT      0L  /**< Constant representing no wait */
#define QM_EVENT_WAIT_FOREVER -1L /**< Constant representing an indefinite wait */

typedef struct BPLib_QM_Event
{
    pthread_mutex_t Lock;
    pthread_cond_t Cond;
    bool IsSet;
} BPLib_QM_Event_t;

void BPLib_QM_EventInit(BPLib_QM_Event_t* Event);

void BPLib_QM_EventSet(BPLib_QM_Event_t* Event);

void BPLib_QM_EventClear(BPLib_QM_Event_t* Event);

bool BPLib_QM_EventWait(BPLib_QM_Event_t* Event);

#endif
