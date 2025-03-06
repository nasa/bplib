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
#ifndef BPLIB_BPCAT_TYPES_H
#define BPLIB_BPCAT_TYPES_H

#include "bplib.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <signal.h>

/* Error Codes */
#define BPCAT_SUCCESS         (0L)
#define BPCAT_NULL_PTR_ERR    (-1L)
#define BPCAT_FWP_ERR         (-2L)
#define BPCAT_SOCKET_ERR      (-3L)
#define BPCAT_TASK_INIT_ERR   (-4L)

typedef int BPCat_Status_t;

typedef struct BPCat_AppData
{
    volatile sig_atomic_t Running;
    void* PoolMem;
    BPLib_Instance_t BPLibInst;
} BPCat_AppData_t;

#endif /* BPLIB_BPCAT_TYPES_H */
