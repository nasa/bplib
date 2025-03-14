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

#include "bplib_cla.h"

/* ==================== */
/* Function Definitions */
/* ==================== */

BPLib_Status_t BPLib_CLA_Init(void)
{
    return BPLIB_SUCCESS;
}

/* BPLib_CLA_Ingress - Received candidate bundles from CL */
BPLib_Status_t BPLib_CLA_Ingress(BPLib_Instance_t* Inst, uint8_t ContId, const void *Bundle, size_t Size, uint32_t Timeout)
{

    if ((Inst == NULL) || (Bundle == NULL))
    {
        return BPLIB_ERROR;
    }

    /* Not a RFC 9171 bundle. Can be a control message or junk*/
    if (BPLib_CLA_IsAControlMsg((const uint8_t*)Bundle))
    {
        /* Processes the control message and pass to BI*/
        BPLib_CLA_ProcessControlMessage((BPLib_CLA_CtrlMsg_t*)Bundle);
        return BPLIB_SUCCESS;
    }
    else
    {
        /* Receive a RFC 9171 bundle and pass it to BI */
        /* Note: An argument can be made to simply implement RecvFullBundleIn here
        * and do away with BI_RecvFullBundleIn()
        */
        return BPLib_BI_RecvFullBundleIn(Inst, Bundle, Size);
    }
}

/* BPLib_CLA_Egress - Receive bundles from BI and send bundles out to CL */
BPLib_Status_t BPLib_CLA_Egress(BPLib_Instance_t* Inst, uint8_t ContId, void *BundleOut,
                                size_t *Size, size_t BufLen, uint32_t Timeout)
{
    BPLib_Status_t     Status = BPLIB_SUCCESS;
    BPLib_Bundle_t    *Bundle;

    /* Null checks */
    if ((Inst == NULL) || (BundleOut == NULL) || (Size == NULL))
    {
        Status = BPLIB_NULL_PTR_ERROR;
    }

    else if (BPLib_QM_WaitQueueTryPull(&Inst->ContactEgressJobs[ContId], &Bundle, Timeout))
    {
        /* Copy the bundle to the CLA buffer */
        Status = BPLib_MEM_BlobCopyOut(Bundle, BundleOut, BufLen, Size);
        if (Status == BPLIB_SUCCESS)
        {
            printf("Egressing packet of %lu bytes to CLA #%d\n", *Size, ContId);
        }

        /* Free the bundle blocks */
        BPLib_MEM_BundleFree(&Inst->pool, Bundle);
    }
    /* No packet was pulled, presumably queue is empty */
    else
    {
        Status = BPLIB_CLA_TIMEOUT;
    }

    return Status;

}

/* Validate Contacts table data */
BPLib_Status_t BPLib_CLA_ContactsTblValidateFunc(void *TblData)
{
    BPLib_Status_t           ReturnCode = BPLIB_SUCCESS;
    BPLib_CLA_ContactsTable_t *TblDataPtr = (BPLib_CLA_ContactsTable_t *)TblData;

    /* Validate data values are within allowed range */
    if (TblDataPtr[0].ContactSet->PortNum <= 0)
    {
        /* element is out of range, return an appropriate error code */
        ReturnCode = BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE;
    }

    return ReturnCode;
}

BPLib_Status_t BPLib_CLA_ContactSetup(uint32_t ContactId)
{
    /*
    1) Checks if path is available for assignment
    2) Assigns an instance of BI and CLA
    3) Configures assigned instances of BI, CT, EBP, CLA based on configuration
    4) Registers CLA with Storage given the table configuration (Path ID, EID map)
    */

    /* TODO: Verify that table was validated via cFS table upload */

    BPLib_Status_t              Status;
    BPLib_CLA_ContactsSet_t     ContactInfo;
    BPLib_CLA_ContactRunState_t RunState;

    if (ContactId < BPLIB_MAX_NUM_CONTACTS)
    {
        ContactInfo = BPLib_NC_ConfigPtrs.ContactsConfigPtr.ContactSet[ContactId];
        RunState    = BPLib_CLA_GetContactRunState(ContactId);

        if (RunState == BPLIB_CLA_TORNDOWN && RunState != BPLIB_CLA_EXITED)
        { /* Contact has been not been setup if the state is anything other than torn down */
            Status = BPLib_FWP_ProxyCallbacks.BPA_CLAP_ContactSetup(ContactInfo, ContactId);

            if (Status == BPLIB_SUCCESS)
            {
                Status = BPLib_CLA_SetContactRunState(ContactId, BPLIB_CLA_SETUP);
            }
        }
        else
        {
            Status = BPLIB_CLA_INCORRECT_STATE;
            BPLib_EM_SendEvent(BPLIB_CLA_CONTACT_NO_STATE_CHG_DBG_EID,
                                BPLib_EM_EventType_DEBUG,
                                "Contact with ID %d is already set up",
                                ContactId);
        }
    }
    else
    {
        Status = BPLIB_CLA_CONTACTS_MAX_REACHED;
        BPLib_EM_SendEvent(BPLIB_CLA_CONTACTS_MAX_REACHED_DBG_EID,
                            BPLib_EM_EventType_DEBUG,
                            "Max simultaneous contacts allowed has been reached");
    }

    return Status;
}

BPLib_Status_t BPLib_CLA_ContactStart(uint32_t ContactId)
{
    BPLib_Status_t              Status;
    BPLib_CLA_ContactRunState_t RunState;

    RunState = BPLib_CLA_GetContactRunState(ContactId);

    if (RunState != BPLIB_CLA_TORNDOWN && RunState != BPLIB_CLA_EXITED)
    { /* Contact must be set up before running */
        BPLib_FWP_ProxyCallbacks.BPA_CLAP_ContactStart(ContactId);
        Status = BPLib_CLA_SetContactRunState(ContactId, BPLIB_CLA_STARTED);
    }
    else
    {
        Status = BPLIB_CLA_INCORRECT_STATE;
        BPLib_EM_SendEvent(BPLIB_CLA_CONTACT_NO_STATE_CHG_DBG_EID,
                            BPLib_EM_EventType_DEBUG,
                            "Contact with ID %d needs to be setup first",
                            ContactId);
    }

    return Status;
}

BPLib_Status_t BPLib_CLA_ContactStop(uint32_t ContactId)
{
    BPLib_Status_t              Status;
    BPLib_CLA_ContactRunState_t RunState;

    RunState = BPLib_CLA_GetContactRunState(ContactId);

    if (RunState == BPLIB_CLA_STARTED && RunState != BPLIB_CLA_EXITED)
    {
        BPLib_FWP_ProxyCallbacks.BPA_CLAP_ContactStop(ContactId);
        Status = BPLib_CLA_SetContactRunState(ContactId, BPLIB_CLA_STOPPED);
    }
    else
    {
        Status = BPLIB_CLA_INCORRECT_STATE;
        BPLib_EM_SendEvent(BPLIB_CLA_CONTACT_NO_STATE_CHG_DBG_EID,
                            BPLib_EM_EventType_DEBUG,
                            "Contact with ID %d needs to be started first",
                            ContactId);
    }

    return Status;
}

BPLib_Status_t BPLib_CLA_ContactTeardown(uint32_t ContactId)
{
    BPLib_Status_t              Status;
    BPLib_CLA_ContactRunState_t RunState;

    RunState = BPLib_CLA_GetContactRunState(ContactId);

    if (RunState == BPLIB_CLA_STOPPED && RunState != BPLIB_CLA_EXITED)
    {
        BPLib_FWP_ProxyCallbacks.BPA_CLAP_ContactTeardown(ContactId);
        Status = BPLib_CLA_SetContactRunState(ContactId, BPLIB_CLA_TORNDOWN);
    }
    else
    {
        Status = BPLIB_CLA_INCORRECT_STATE;
        BPLib_EM_SendEvent(BPLIB_CLA_CONTACT_NO_STATE_CHG_DBG_EID,
                            BPLib_EM_EventType_DEBUG,
                            "Contact with ID %d needs to be stopped first",
                            ContactId);
    }

    return Status;
}

BPLib_CLA_ContactRunState_t BPLib_CLA_GetContactRunState(uint32_t ContactId)
{
    return BPLib_CLA_ContactRunStates[ContactId];
}

BPLib_Status_t BPLib_CLA_SetContactRunState(uint32_t ContactId, BPLib_CLA_ContactRunState_t RunState)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (RunState == BPLIB_CLA_STOPPED  && BPLib_CLA_ContactRunStates[ContactId] == BPLIB_CLA_TORNDOWN ||
        RunState == BPLIB_CLA_STARTED  && BPLib_CLA_ContactRunStates[ContactId] == BPLIB_CLA_TORNDOWN ||
        RunState == BPLIB_CLA_SETUP    && BPLib_CLA_ContactRunStates[ContactId] == BPLIB_CLA_STARTED  ||
        RunState == BPLIB_CLA_TORNDOWN && BPLib_CLA_ContactRunStates[ContactId] == BPLIB_CLA_EXITED   ||
        RunState == BPLIB_CLA_STOPPED  && BPLib_CLA_ContactRunStates[ContactId] == BPLIB_CLA_EXITED   ||
        RunState == BPLIB_CLA_STARTED  && BPLib_CLA_ContactRunStates[ContactId] == BPLIB_CLA_EXITED   ||
        RunState == BPLIB_CLA_SETUP    && BPLib_CLA_ContactRunStates[ContactId] == BPLIB_CLA_EXITED)
    {
        Status = BPLIB_CLA_INCORRECT_STATE;
    }
    else
    {
        BPLib_CLA_ContactRunStates[ContactId] = RunState;
    }

    return Status;
}