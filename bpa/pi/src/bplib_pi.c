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
BPLib_Status_t BPLib_PI_Ingress(uint8_t ChanId, void *AduPtr, size_t AduSize)
{
    return BPLIB_SUCCESS;
}

/* Egress an ADU */
BPLib_Status_t BPLib_PI_Egress(uint8_t ChanId, void *BundlePtr, size_t BundleSize)
{
    return BPLIB_SUCCESS;
}
