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
#include "bplib_eid.h"
#include "bplib_qm.h"

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
** Storage Policy Table
*/
typedef struct
{
    BPLib_EID_Pattern_t SrcEIDs[BPLIB_MAX_NUM_STORE_EIDS];
    uint32_t            StorageSize;
} BPLib_STOR_StorageSet_t;

typedef struct
{
    BPLib_STOR_StorageSet_t StorageSet[BPLIB_MAX_NUM_STORE_SET];
} BPLib_STOR_StorageTable_t;

/*
** Externs
*/

extern BPLib_StorageHkTlm_Payload_t BPLib_STOR_StoragePayload;

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

/**
 * \brief Validate Storage Table configurations
 *
 *  \par Description
 *       Validate configuration table parameters
 *
 *  \par Assumptions, External Events, and Notes:
 *       - This function is called by whatever external task handles table management. 
 *         Every time a new Storage table is loaded, this function should be called to
 *         validate its parameters.
 *
 *  \param[in] TblData Pointer to the config table
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Validation was successful
 */
BPLib_Status_t BPLib_STOR_StorageTblValidateFunc(void *TblData);



/**
 * @brief Check the bundle cache fifo for available bundles
 *
 * This function pulls from Bundle Cache and pushes them to the unsorted job queue
 *
 * @param[in] Inst The instance containing the jobs to be sorted.
 * @param[in] MaxBundlesToScan The number of bundles to pull from storage
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Scanning cache was successful
 *  \retval BPLIB_NULL_PTR_ERROR Provided instance pointer was null/invalid
 */
BPLib_Status_t BPLib_STOR_ScanCache(BPLib_Instance_t* Inst, uint32_t MaxBundlesToScan);

/**
 * @brief Cache a bundle
 *
 * This function takes a bundle and puts it in Cache
 *
 * @param[in] Inst The instance with the queue context information
 * @param[in] Bundle The bundle to cache
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Scanning cache was successful
 */
BPLib_Status_t BPLib_STOR_CacheBundle(BPLib_Instance_t *Inst, BPLib_Bundle_t *Bundle);

#endif /* BPLIB_STOR_H */
