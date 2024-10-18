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

#ifndef BPLIB_STOR_H
#define BPLIB_STOR_H

/*
** Include
*/

#include "bplib_api_types.h"
#include "bplib_cfg.h"

// TODO BPLIB_FLAG_DIAGNOSTIC (from BPLIB_FLAG_DIAGNOSTIC) should b in bplib.h
#define BPLIB_FLAG_DIAGNOSTIC              0x00000000


/**
 * \brief Storage housekeeping payload
 */

typedef struct BPLib_StorageHkTlm_Payload BPLib_StorageHkTlm_Payload_t;

struct BPLib_StorageHkTlm_Payload
{
    uint32_t    NumBundlesQueued[BPLIB_MAX_NUM_BUNDLE_QUEUES];  /**< \brief Number of bundles queued per queue */
    float       StoragePercentFull;                             /**< \brief Percentage of storage which triggers discarding deleted bundles from storage */
    float       MemInUse;                                       /**< \brief Memory in use */
    uint32_t    MemHighWater;                                   /**< \brief Memory high water mark */
    uint32_t    NumFreeFlashBlocks;                             /**< \brief Free Flash memory statistics counter */
    uint32_t    NumUsedFlashBlocks;                             /**< \brief Used Flash memory statistics counter */
    uint32_t    NumFailedFlashBlocks;                           /**< \brief Failed Flash memory statistics counter */
    uint32_t    FlashErrorCount;                                /**< \brief Flash memory statistics counter */
    
    uint32_t TimeBootEra;                   /**< \brief Boot Era for Monotonic Time */
    int64_t  MonotonicTime;                 /**< \brief Monotonic Time Counter */
    int64_t  CorrelationFactor;             /**< \brief Time Correlation Factor */    
};

/*
** Exported Functions
*/

/**
 * \brief Storage initialization
 *
 *  \par Description
 *       STOR initialization function
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Initialization was successful
 */
int BPLib_STOR_Init(void);

#endif /* BPLIB_STOR_H */
