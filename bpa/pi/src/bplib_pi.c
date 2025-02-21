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

/**
 * @file
 *
 * Public APIs for Payload Interface
 */

/*
** Include
*/

#include "bplib_pi.h"
#include "bplib_mem.h"

#include <stdio.h>


/*
** Function Definitions
*/

/* Add application configurations */
BPLib_Status_t BPLib_PI_AddApplication(uint8_t ChanId)
{
    return BPLIB_SUCCESS;
}

/* Remove application configurations */
BPLib_Status_t BPLib_PI_RemoveApplication(uint8_t ChanId)
{
    return BPLIB_SUCCESS;
}

/* Validate channel configuration parameters */
BPLib_Status_t BPLib_PI_ValidateConfigs(void *TblData)
{
    BPLib_PI_ChannelTable_t *TblDataPtr = (BPLib_PI_ChannelTable_t *)TblData;

    /* Temporary check, implement full validation TODO */
    if (TblDataPtr->Configs[0].MaxBundlePayloadSize == -1)
    {
        return BPLIB_PI_INVALID_CONFIG_ERROR;
    }

    return BPLIB_SUCCESS;
}

/* Ingress an ADU */
BPLib_Status_t BPLib_PI_Ingress(BPLib_Instance_t* Inst, uint8_t ChanId, 
                                                            void *AduPtr, size_t AduSize)
{
    BPLib_Bundle_t    *NewBundle;

    if ((Inst == NULL) || (AduPtr == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    /* Allocate Bundle based on AduSize */
    NewBundle = BPLib_MEM_BundleAlloc(&Inst->pool, (const void*)AduPtr, AduSize);
    if (NewBundle == NULL)
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    /* TODO fully fill out primary block fields */
    NewBundle->blocks.pri_blk.version = BPLIB_BUNDLE_PROTOCOL_VERSION;

    /* Temporary code to allow for routing between chan 0 and 1, will be replaced */
    if (ChanId == 0)
    {
        /* this will route it back to the contact egress, after cache */
        NewBundle->blocks.pri_blk.dest_eid.node_number = BPLIB_TEMPORARY_EID_NODE_NUM_FOR_CONTACT_ROUTES;
        NewBundle->blocks.pri_blk.dest_eid.service_number = BPLIB_TEMPORARY_EID_SERVICE_NUM_FOR_CONTACT_ROUTES;
    }
    else
    {
        /* this will route it back to the channel egress, after cache */
        NewBundle->blocks.pri_blk.dest_eid.node_number = BPLIB_TEMPORARY_EID_NODE_NUM_FOR_CHANNEL_ROUTES;
        /* this will route it back to the channel 1 */
        NewBundle->blocks.pri_blk.dest_eid.service_number = BPLIB_TEMPORARY_EID_SERVICE_NUM_FOR_CHANNEL_1_ROUTES;
    }

    printf("Ingressing packet of %lu bytes from ADU via channel #%d\n", (unsigned long)AduSize, ChanId);

    return BPLib_QM_AddUnsortedJob(Inst, NewBundle, CHANNEL_IN_PI_TO_EBP, 
                                                        QM_PRI_NORMAL, QM_WAIT_FOREVER);
}

/* Egress an ADU */
BPLib_Status_t BPLib_PI_Egress(BPLib_Instance_t *Inst, uint8_t ChanId, void *AduPtr, 
                                    size_t *AduSize, size_t BufLen, uint32_t Timeout)
{
    BPLib_Bundle_t    *Bundle;
    BPLib_Status_t     Status = BPLIB_SUCCESS;

    /* Null checks */
    if ((Inst == NULL) || (AduPtr == NULL) || (AduSize == NULL))
    {
        Status = BPLIB_NULL_PTR_ERROR;
    }
    /* Get the next bundle in the channel egress queue */
    else if (BPLib_QM_WaitQueueTryPull(&Inst->ChannelEgressJobs[ChanId], &Bundle, Timeout))
    {
        /* Copy out the contents of the bundle payload to the return pointer */
        Status = BPLib_MEM_BlobCopyOut(Bundle, AduPtr, BufLen, AduSize);

        /* Free the bundle */
        BPLib_MEM_BundleFree(&Inst->pool, Bundle);

        if (Status == BPLIB_SUCCESS)
        {
            printf("Egressing packet of %lu bytes to ADU via channel #%d\n", *AduSize, ChanId);
        }
    }
    /* No packet was pulled, presumably queue is empty */
    else 
    {
        Status = BPLIB_PI_TIMEOUT;
    }

    return Status;
}
