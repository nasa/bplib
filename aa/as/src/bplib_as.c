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

/* ======== */
/* Includes */
/* ======== */

#include "bplib_as.h"

/* ==================== */
/* Function Definitions */
/* ==================== */

BPLib_Status_t BPLib_AS_Init(void) 
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_AS_RequestInterpreter()
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    * Receives requests to increment counters or set statistics
    * etermines whether request is valid
    * Generates and sends status messages with current telemetry to TLMP
    * Maintains set of source EID patterns
    */

    return Status;
}

BPLib_Status_t BPLib_AS_StatisticsDatabase()
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    * Maintains current values of statistics
    * Maintains map of MIB elements accessed by source EID pattern to counter values
    */

    return Status;
}
