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

#include "bplib_cla_test_utils.h"
#include "bplib_nc.h"
#include "bpa_fwp_stubs.h"

#include "bplib_bi.h"
#include "bplib_qm_waitqueue.h"
#include "bplib_mem.h"

extern bool BPLib_CLA_AutoEgressEnabled[BPLIB_MAX_NUM_CONTACTS];

void Test_BPLib_CLA_Ingress_NullInstPtrError(void)
{
    BPLib_Status_t ReturnStatus;
    uint32_t ContId = 0;
    uint8_t InputBundleBuffer[30];
    uint32_t Timeout = 0;

    ReturnStatus = BPLib_CLA_Ingress(NULL,
                                     ContId,
                                     InputBundleBuffer,
                                     sizeof(InputBundleBuffer),
                                     Timeout);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);
    UtAssert_STUB_COUNT(BPLib_CLA_IsAControlMsg, 0);
    UtAssert_STUB_COUNT(BPLib_CLA_ProcessControlMessage, 0);
    UtAssert_STUB_COUNT(BPLib_BI_RecvFullBundleIn, 0);
}

void Test_BPLib_CLA_Ingress_NullInputBundleError(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Instance_t InputInstance;
    uint32_t ContId = 0;
    uint8_t InputBundleBuffer[30];
    uint32_t Timeout = 0;

    ReturnStatus = BPLib_CLA_Ingress(&InputInstance,
                                     ContId,
                                     NULL,
                                     sizeof(InputBundleBuffer),
                                     Timeout);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);
    UtAssert_STUB_COUNT(BPLib_CLA_IsAControlMsg, 0);
    UtAssert_STUB_COUNT(BPLib_CLA_ProcessControlMessage, 0);
    UtAssert_STUB_COUNT(BPLib_BI_RecvFullBundleIn, 0);
}

void Test_BPLib_CLA_Ingress_BadContId(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Instance_t InputInstance;
    uint32_t ContId = BPLIB_MAX_NUM_CONTACTS;
    uint8_t InputBundleBuffer[30];
    uint32_t Timeout = 0;

    ReturnStatus = BPLib_CLA_Ingress(&InputInstance,
                                     ContId,
                                     InputBundleBuffer,
                                     sizeof(InputBundleBuffer),
                                     Timeout);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_INVALID_CONT_ID_ERR);
    UtAssert_STUB_COUNT(BPLib_CLA_IsAControlMsg, 0);
    UtAssert_STUB_COUNT(BPLib_CLA_ProcessControlMessage, 0);
    UtAssert_STUB_COUNT(BPLib_BI_RecvFullBundleIn, 0);
}

void Test_BPLib_CLA_Ingress_ControlMessageNominal(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Instance_t InputInstance;
    uint32_t ContId = 0;
    BPLib_CLA_CtrlMsg_t InputControlMessage;
    uint32_t Timeout = 0;

    /*
    ** Set the input buffer to be interpretted as a control message
    */
    memset(&InputControlMessage, 0, sizeof(InputControlMessage));
    strncpy(InputControlMessage.CtrlMsgTag, "BPNMSG", sizeof(InputControlMessage.CtrlMsgTag));
    InputControlMessage.MsgTypes = SentIt;

    ReturnStatus = BPLib_CLA_Ingress(&InputInstance,
                                     ContId,
                                     &InputControlMessage,
                                     sizeof(InputControlMessage),
                                     Timeout);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_SUCCESS);
    UtAssert_STUB_COUNT(BPLib_BI_RecvFullBundleIn, 0);
}

void Test_BPLib_CLA_Ingress_NonControlMessageNominal(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Instance_t InputInstance;
    uint32_t ContId = 0;
    char InputBundleBuffer[30];
    uint32_t Timeout = 0;

    /*
    ** Set the input buffer to be interpretted as a non-control message
    */
    memset(InputBundleBuffer, 0, sizeof(InputBundleBuffer));
    strncpy(InputBundleBuffer, "NOT-MSG", sizeof(InputBundleBuffer));

    ReturnStatus = BPLib_CLA_Ingress(&InputInstance,
                                     ContId,
                                     InputBundleBuffer,
                                     sizeof(InputBundleBuffer),
                                     Timeout);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_SUCCESS);
    UtAssert_STUB_COUNT(BPLib_BI_RecvFullBundleIn, 1);
}

void Test_BPLib_CLA_Egress_NullInstanceInputError(void)
{
    BPLib_Status_t ReturnStatus;
    uint32_t ContId = 0;
    uint8_t OutputBundleBuffer[30];
    size_t NumBytesCopiedToOutputBuf;
    size_t OutputBundleBufferLength = sizeof(OutputBundleBuffer);
    uint32_t Timeout = 0;

    ReturnStatus = BPLib_CLA_Egress(NULL,
                                    ContId,
                                    OutputBundleBuffer,
                                    &NumBytesCopiedToOutputBuf,
                                    OutputBundleBufferLength,
                                    Timeout);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);
    UtAssert_STUB_COUNT(BPLib_QM_DuctPull, 0);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 0);
}

void Test_BPLib_CLA_Egress_NullBundleOutBufferError(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Instance_t Instance;
    uint32_t ContId = 0;
    uint8_t OutputBundleBuffer[30];
    size_t NumBytesCopiedToOutputBuf;
    size_t OutputBundleBufferLength = sizeof(OutputBundleBuffer);
    uint32_t Timeout = 0;

    ReturnStatus = BPLib_CLA_Egress(&Instance,
                                    ContId,
                                    NULL,
                                    &NumBytesCopiedToOutputBuf,
                                    OutputBundleBufferLength,
                                    Timeout);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);
    UtAssert_STUB_COUNT(BPLib_QM_DuctPull, 0);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 0);
}

void Test_BPLib_CLA_Egress_NullSizeBufferError(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Instance_t Instance;
    uint32_t ContId = 0;
    uint8_t OutputBundleBuffer[30];
    // size_t NumBytesCopiedToOutputBuf;
    size_t OutputBundleBufferLength = sizeof(OutputBundleBuffer);
    uint32_t Timeout = 0;

    ReturnStatus = BPLib_CLA_Egress(&Instance,
                                    ContId,
                                    OutputBundleBuffer,
                                    NULL,
                                    OutputBundleBufferLength,
                                    Timeout);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);
    UtAssert_STUB_COUNT(BPLib_QM_DuctPull, 0);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 0);
}

void Test_BPLib_CLA_Egress_BadContId(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Instance_t Instance;
    uint32_t ContId = BPLIB_MAX_NUM_CONTACTS;
    uint8_t OutputBundleBuffer[30];
    size_t NumBytesCopiedToOutputBuf;
    size_t OutputBundleBufferLength = sizeof(OutputBundleBuffer);
    uint32_t Timeout = 0;

    ReturnStatus = BPLib_CLA_Egress(&Instance,
                                    ContId,
                                    OutputBundleBuffer,
                                    &NumBytesCopiedToOutputBuf,
                                    OutputBundleBufferLength,
                                    Timeout);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_INVALID_CONT_ID_ERR);
    UtAssert_INT32_EQ(NumBytesCopiedToOutputBuf, 0);
    UtAssert_STUB_COUNT(BPLib_QM_DuctPull, 0);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 0);
}

void Test_BPLib_CLA_Egress_QueuePullTimeout(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Instance_t Instance;
    uint32_t ContId = 0;
    uint8_t OutputBundleBuffer[30];
    size_t NumBytesCopiedToOutputBuf;
    size_t OutputBundleBufferLength = sizeof(OutputBundleBuffer);
    uint32_t Timeout = 0;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_QM_DuctPull), BPLIB_TIMEOUT);

    ReturnStatus = BPLib_CLA_Egress(&Instance,
                                    ContId,
                                    OutputBundleBuffer,
                                    &NumBytesCopiedToOutputBuf,
                                    OutputBundleBufferLength,
                                    Timeout);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_CLA_TIMEOUT);
    UtAssert_STUB_COUNT(BPLib_QM_DuctPull, 1);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 0);
}

void Test_BPLib_CLA_Egress_BlobCopyFail(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Instance_t Instance;
    uint32_t ContId = 0;
    uint8_t OutputBundleBuffer[30];
    size_t NumBytesCopiedToOutputBuf;
    size_t OutputBundleBufferLength = sizeof(OutputBundleBuffer);
    uint32_t Timeout = 0;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_QM_DuctPull), BPLIB_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_BI_BlobCopyOut), BPLIB_BUF_LEN_ERROR);

    ReturnStatus = BPLib_CLA_Egress(&Instance,
                                    ContId,
                                    OutputBundleBuffer,
                                    &NumBytesCopiedToOutputBuf,
                                    OutputBundleBufferLength,
                                    Timeout);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_BUF_LEN_ERROR);
    UtAssert_STUB_COUNT(BPLib_QM_DuctPull, 1);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 1);
}

void Test_BPLib_CLA_Egress_Nominal(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Instance_t Instance;
    uint32_t ContId = 0;
    uint8_t OutputBundleBuffer[30];
    size_t NumBytesCopiedToOutputBuf;
    size_t OutputBundleBufferLength = sizeof(OutputBundleBuffer);
    uint32_t Timeout = 0;
    BPLib_Bundle_t Bundle;
    BPLib_Bundle_t *BundlePtr = &Bundle;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_QM_DuctPull), BPLIB_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_BI_BlobCopyOut), BPLIB_SUCCESS);
    UT_SetDataBuffer(UT_KEY(BPLib_QM_DuctPull), &BundlePtr, sizeof(BundlePtr), false);

    ReturnStatus = BPLib_CLA_Egress(&Instance,
                                    ContId,
                                    OutputBundleBuffer,
                                    &NumBytesCopiedToOutputBuf,
                                    OutputBundleBufferLength,
                                    Timeout);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_SUCCESS);
    UtAssert_STUB_COUNT(BPLib_QM_DuctPull, 1);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 1);
}

void Test_BPLib_CLA_ContactsTblValidateFunc_Nominal(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_CLA_ContactsTable_t TestTblData;

    /* Set input table to all valid values */
    memset(&TestTblData, 0, sizeof(TestTblData));

    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_PatternIsValid), true);

    /* Run unit test and check results */
    ReturnStatus = BPLib_CLA_ContactsTblValidateFunc(&TestTblData);
    UtAssert_INT32_EQ(ReturnStatus, BPLIB_SUCCESS);
}

void Test_BPLib_CLA_ContactsTblValidateFunc_DtnDestEid(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_CLA_ContactsTable_t TestTblData;

    /* Set input table to all valid values */
    memset(&TestTblData, 0, sizeof(TestTblData));

    TestTblData.ContactSet[0].DestEIDs[0].Scheme = BPLIB_EID_SCHEME_DTN;

    /* Run unit test and check results */
    ReturnStatus = BPLib_CLA_ContactsTblValidateFunc(&TestTblData);
    UtAssert_INT32_EQ(ReturnStatus, BPLIB_INVALID_CONFIG_ERR);
}

void Test_BPLib_CLA_ContactsTblValidateFunc_InvDestEid(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_CLA_ContactsTable_t TestTblData;

    /* Set input table to all valid values */
    memset(&TestTblData, 0, sizeof(TestTblData));

    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_PatternIsValid), false);

    /* Run unit test and check results */
    ReturnStatus = BPLib_CLA_ContactsTblValidateFunc(&TestTblData);
    UtAssert_INT32_EQ(ReturnStatus, BPLIB_INVALID_CONFIG_ERR);
}

void Test_BPLib_CLA_ContactsTblValidateFunc_InvTimeout(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_CLA_ContactsTable_t TestTblData;

    /* Set input table to all valid values */
    memset(&TestTblData, 0, sizeof(TestTblData));

    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_PatternIsValid), true);

    TestTblData.ContactSet[0].RetransmitTimeout = BPLIB_MAX_RETRANSMIT_ALLOWED + 1;

    /* Run unit test and check results */
    ReturnStatus = BPLib_CLA_ContactsTblValidateFunc(&TestTblData);
    UtAssert_INT32_EQ(ReturnStatus, BPLIB_INVALID_CONFIG_ERR);
}

void Test_BPLib_CLA_ContactsTblValidateFunc_InvTimeTrig(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_CLA_ContactsTable_t TestTblData;

    /* Set input table to all valid values */
    memset(&TestTblData, 0, sizeof(TestTblData));

    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_PatternIsValid), true);

    TestTblData.ContactSet[0].CSTimeTrigger = BPLIB_MAX_CS_TIME_TRIGGER_ALLOWED + 1;

    /* Run unit test and check results */
    ReturnStatus = BPLib_CLA_ContactsTblValidateFunc(&TestTblData);
    UtAssert_INT32_EQ(ReturnStatus, BPLIB_INVALID_CONFIG_ERR);
}

void Test_BPLib_CLA_ContactsTblValidateFunc_InvSizeTrig(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_CLA_ContactsTable_t TestTblData;

    /* Set input table to all valid values */
    memset(&TestTblData, 0, sizeof(TestTblData));

    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_PatternIsValid), true);

    TestTblData.ContactSet[0].CSSizeTrigger = BPLIB_MAX_CS_SIZE_TRIGGER_ALLOWED + 1;

    /* Run unit test and check results */
    ReturnStatus = BPLib_CLA_ContactsTblValidateFunc(&TestTblData);
    UtAssert_INT32_EQ(ReturnStatus, BPLIB_INVALID_CONFIG_ERR);
}


void Test_BPLib_CLA_ContactSetup_Nominal(void)
{
    BPLib_Status_t Status;
    uint32_t       ContactId;

    /* Set the ContactId to a valid value */
    ContactId = BPLIB_MAX_NUM_CONTACTS - 1;

    /* Set a valid run state */
    BPLib_CLA_ContactRunStates[ContactId] = BPLIB_CLA_TORNDOWN;

    /* Force BPA_CLAP_ContactSetup to return a success value */
    UT_SetDefaultReturnValue(UT_KEY(BPA_CLAP_ContactSetup), BPLIB_SUCCESS);

    /* Run the function under test */
    Status = BPLib_CLA_ContactSetup(ContactId);

    /* Verify that Status is success */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);

    /* Verify that run state is setup */
    UtAssert_EQ(BPLib_CLA_ContactRunState_t, BPLib_CLA_ContactRunStates[ContactId], BPLIB_CLA_SETUP);
}

void Test_BPLib_CLA_ContactSetup_InvalidContactId(void)
{
    BPLib_Status_t          Status;
    uint32_t                ContactId;

    /* Set the ContactId to an invalid value */
    ContactId = BPLIB_MAX_NUM_CONTACTS + 1;

    /* Run the function under test */
    Status = BPLib_CLA_ContactSetup(ContactId);

    /* Verify that Status is as expected */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_INVALID_CONT_ID_ERR);

    /* Verify that the event was issued */
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 1);
    BPLib_CLA_Test_Verify_Event(0,
                                BPLIB_CLA_INVALID_CONTACT_ID_DBG_EID,
                                "Contact ID must be less than %d, given %d");
}

void Test_BPLib_CLA_ContactSetup_InvalidRunState(void)
{
    BPLib_Status_t Status;
    uint32_t       ContactId;

    ContactId = BPLIB_MAX_NUM_CONTACTS - 1;

    /* Set a invalid run state */
    BPLib_CLA_ContactRunStates[ContactId] = BPLIB_CLA_STARTED;

    /* Run the function under test */
    Status = BPLib_CLA_ContactSetup(ContactId);

    /* Verify the execution status */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_CLA_INCORRECT_STATE);

    /* Verify that run state is unchanged */
    UtAssert_EQ(BPLib_CLA_ContactRunState_t, BPLib_CLA_ContactRunStates[ContactId], BPLIB_CLA_STARTED);

    /* Verify that the correct event was issued */
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 1);
    BPLib_CLA_Test_Verify_Event(0,
                                BPLIB_CLA_CONTACT_NO_STATE_CHG_DBG_EID,
                                "Contact with ID #%d must be torndown");
}

void Test_BPLib_CLA_ContactSetup_CallbackError(void)
{
    BPLib_Status_t          Status;
    uint32_t                ContactId;

    /* Set the ContactId to a valid value */
    ContactId = BPLIB_MAX_NUM_CONTACTS - 1;

    /* Set a valid run state */
    BPLib_CLA_ContactRunStates[ContactId] = BPLIB_CLA_TORNDOWN;

    /* Force BPA_CLAP_ContactSetup to return an error value */
    UT_SetDefaultReturnValue(UT_KEY(BPA_CLAP_ContactSetup), BPLIB_CLA_IO_ERROR);

    /* Run the function under test */
    Status = BPLib_CLA_ContactSetup(ContactId);

    /* Verify that Status is as expected */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_CLA_IO_ERROR);

    /* Verify that run state is setup */
    UtAssert_EQ(BPLib_CLA_ContactRunState_t, BPLib_CLA_ContactRunStates[ContactId], BPLIB_CLA_TORNDOWN);
}

void Test_BPLib_CLA_ContactStart_Nominal(void)
{
    BPLib_Status_t Status;
    uint32_t       ContactId;

    /* Assign a valid contact ID */
    ContactId = BPLIB_MAX_NUM_CONTACTS - 1;

    /* Put the contact in a valid run state */
    BPLib_CLA_ContactRunStates[ContactId] = BPLIB_CLA_SETUP;

    /* Run the function under test */
    Status = BPLib_CLA_ContactStart(ContactId);

    /* Show that the contact was started successfully */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);

    /* Show that the run state transitioned */
    UtAssert_EQ(BPLib_CLA_ContactRunState_t, BPLib_CLA_ContactRunStates[ContactId], BPLIB_CLA_STARTED);
}

void Test_BPLib_CLA_ContactStart_InvalidContactId(void)
{
    BPLib_Status_t Status;
    uint32_t ContactId;

    /* Create an invalid contact ID */
    ContactId = BPLIB_MAX_NUM_CONTACTS + 1;

    /* Run the function under test */
    Status = BPLib_CLA_ContactStart(ContactId);

    /* Show that the function failed */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_INVALID_CONT_ID_ERR);

    /* Verify that the correct event was issued */
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 1);
    BPLib_CLA_Test_Verify_Event(0,
                                BPLIB_CLA_INVALID_CONTACT_ID_DBG_EID,
                                "Contact ID %d is invalid");
}

void Test_BPLib_CLA_ContactStart_InvalidRunState(void)
{
    BPLib_Status_t Status;
    uint32_t       ContactId;

    /* Create a valid contact ID */
    ContactId = BPLIB_MAX_NUM_CONTACTS - 1;

    /* Put the contact in an invalid run state */
    BPLib_CLA_ContactRunStates[ContactId] = BPLIB_CLA_TORNDOWN;

    /* Run the function under test */
    Status = BPLib_CLA_ContactStart(ContactId);

    /* Show that the function failed */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_CLA_INCORRECT_STATE);

    /* Show that the run state did not transition */
    UtAssert_EQ(BPLib_CLA_ContactRunState_t, BPLib_CLA_ContactRunStates[ContactId], BPLIB_CLA_TORNDOWN);

    /* Verify that the correct event was issued */
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 1);
    BPLib_CLA_Test_Verify_Event(0,
                                BPLIB_CLA_CONTACT_NO_STATE_CHG_DBG_EID,
                                "Contact with ID #%d must be setup before starting");
}

void Test_BPLib_CLA_ContactStop_Nominal(void)
{
    BPLib_Status_t Status;
    uint32_t       ContactId;

    /* Assign a valid contact ID */
    ContactId = BPLIB_MAX_NUM_CONTACTS - 1;

    /* Put the contact in a valid run state */
    BPLib_CLA_ContactRunStates[ContactId] = BPLIB_CLA_STARTED;

    /* Run the function under test */
    Status = BPLib_CLA_ContactStop(ContactId);

    /* Show that the contact was started successfully */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);

    /* Show that the run state transitioned */
    UtAssert_EQ(BPLib_CLA_ContactRunState_t, BPLib_CLA_ContactRunStates[ContactId], BPLIB_CLA_STOPPED);
}

void Test_BPLib_CLA_ContactStop_InvalidContactId(void)
{
    BPLib_Status_t Status;
    uint32_t ContactId;

    /* Create an invalid contact ID */
    ContactId = BPLIB_MAX_NUM_CONTACTS + 1;

    /* Run the function under test */
    Status = BPLib_CLA_ContactStop(ContactId);

    /* Show that the function failed */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_INVALID_CONT_ID_ERR);

    /* Verify that the correct event was issued */
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 1);
    BPLib_CLA_Test_Verify_Event(0,
                                BPLIB_CLA_INVALID_CONTACT_ID_DBG_EID,
                                "Contact ID %d is invalid");
}

void Test_BPLib_CLA_ContactStop_InvalidRunState(void)
{
    BPLib_Status_t Status;
    uint32_t       ContactId;

    /* Create a valid contact ID */
    ContactId = BPLIB_MAX_NUM_CONTACTS - 1;

    /* Put the contact in an invalid run state */
    BPLib_CLA_ContactRunStates[ContactId] = BPLIB_CLA_TORNDOWN;

    /* Run the function under test */
    Status = BPLib_CLA_ContactStop(ContactId);

    /* Show that the function failed */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_CLA_INCORRECT_STATE);

    /* Show that the run state transitioned */
    UtAssert_EQ(BPLib_CLA_ContactRunState_t, BPLib_CLA_ContactRunStates[ContactId], BPLIB_CLA_TORNDOWN);

    /* Verify that the correct event was issued */
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 1);
    BPLib_CLA_Test_Verify_Event(0,
                                BPLIB_CLA_CONTACT_NO_STATE_CHG_DBG_EID,
                                "Contact with ID #%d must be started first");
}

void Test_BPLib_CLA_ContactTeardown_Nominal(void)
{
    BPLib_Status_t Status;
    uint32_t       ContactId;
    BPLib_Instance_t Inst;

    /* Assign a valid contact ID */
    ContactId = BPLIB_MAX_NUM_CONTACTS - 1;

    /* Put the contact in a valid run state */
    BPLib_CLA_ContactRunStates[ContactId] = BPLIB_CLA_STOPPED;

    /* Pull one bundle */
    UT_SetDeferredRetcode(UT_KEY(BPLib_QM_WaitQueueTryPull), 1, true);

    /* Run the function under test */
    Status = BPLib_CLA_ContactTeardown(&Inst, ContactId);

    /* Show that the contact was started successfully */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);

    /* Show that the run state transitioned */
    UtAssert_EQ(BPLib_CLA_ContactRunState_t, BPLib_CLA_ContactRunStates[ContactId], BPLIB_CLA_TORNDOWN);

    UtAssert_STUB_COUNT(BPLib_STOR_StoreBundle, 1);
}

void Test_BPLib_CLA_ContactTeardown_StorErr(void)
{
    BPLib_Status_t Status;
    uint32_t       ContactId;
    BPLib_Instance_t Inst;

    /* Assign a valid contact ID */
    ContactId = BPLIB_MAX_NUM_CONTACTS - 1;

    /* Put the contact in a valid run state */
    BPLib_CLA_ContactRunStates[ContactId] = BPLIB_CLA_STOPPED;

    /* Pull one bundle */
    UT_SetDeferredRetcode(UT_KEY(BPLib_QM_WaitQueueTryPull), 1, true);
    UT_SetDeferredRetcode(UT_KEY(BPLib_STOR_StoreBundle), 1, BPLIB_ERROR);

    /* Run the function under test */
    Status = BPLib_CLA_ContactTeardown(&Inst, ContactId);

    /* Show that the contact was started successfully */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);

    /* Show that the run state transitioned */
    UtAssert_EQ(BPLib_CLA_ContactRunState_t, BPLib_CLA_ContactRunStates[ContactId], BPLIB_CLA_TORNDOWN);

    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 1);
    BPLib_CLA_Test_Verify_Event(0,
                                BPLIB_CLA_REMOVE_QUEUE_FLUSH_DGB_EID,
                                "Contact with ID #%d failed to push a bundle back to storage, Status = %d");

    UtAssert_STUB_COUNT(BPLib_STOR_StoreBundle, 1);
}

void Test_BPLib_CLA_ContactTeardown_NullInst(void)
{
    BPLib_Status_t Status;
    uint32_t       ContactId;

    /* Assign a valid contact ID */
    ContactId = BPLIB_MAX_NUM_CONTACTS - 1;

    Status = BPLib_CLA_ContactTeardown(NULL, ContactId);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_NULL_PTR_ERROR);
}

void Test_BPLib_CLA_ContactTeardown_InvalidContactId(void)
{
    BPLib_Status_t Status;
    uint32_t ContactId;
    BPLib_Instance_t Inst;

    /* Create an invalid contact ID */
    ContactId = BPLIB_MAX_NUM_CONTACTS + 1;

    /* Run the function under test */
    Status = BPLib_CLA_ContactTeardown(&Inst, ContactId);

    /* Show that the function failed */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_INVALID_CONT_ID_ERR);

    /* Verify that the correct event was issued */
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 1);
    BPLib_CLA_Test_Verify_Event(0,
                                BPLIB_CLA_INVALID_CONTACT_ID_DBG_EID,
                                "Contact ID %d is invalid");
}

void Test_BPLib_CLA_ContactTeardown_InvalidRunState(void)
{
    BPLib_Status_t Status;
    uint32_t       ContactId;
    BPLib_Instance_t Inst;

    /* Create a valid contact ID */
    ContactId = BPLIB_MAX_NUM_CONTACTS - 1;

    /* Put the contact in an invalid run state */
    BPLib_CLA_ContactRunStates[ContactId] = BPLIB_CLA_STARTED;

    /* Run the function under test */
    Status = BPLib_CLA_ContactTeardown(&Inst, ContactId);

    /* Show that the function failed */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_CLA_INCORRECT_STATE);

    /* Show that the run state was unchanged */
    UtAssert_EQ(BPLib_CLA_ContactRunState_t, BPLib_CLA_ContactRunStates[ContactId], BPLIB_CLA_STARTED);

    /* Verify that the correct event was issued */
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 1);
    BPLib_CLA_Test_Verify_Event(0,
                                BPLIB_CLA_CONTACT_NO_STATE_CHG_DBG_EID,
                                "Contact with ID #%d needs to be stopped or set up first");
}

void Test_BPLib_CLA_SetAutoEgress_Nominal(void)
{
    uint32_t ContId = 0;
    BPLib_CLA_AutoEgressEnabled[ContId] = false;

    UtAssert_EQ(BPLib_Status_t, BPLib_CLA_SetAutoEgress(ContId, true), BPLIB_SUCCESS);
    UtAssert_EQ(bool, BPLib_CLA_AutoEgressEnabled[ContId], true);
}

void Test_BPLib_CLA_SetAutoEgress_ContIdErr(void)
{
    uint32_t ContId = BPLIB_MAX_NUM_CONTACTS;

    UtAssert_EQ(BPLib_Status_t, BPLib_CLA_SetAutoEgress(ContId, true), BPLIB_INVALID_CONT_ID_ERR);
}

void Test_BPLib_CLA_GetAutoEgress_Nominal(void)
{
    uint32_t ContId = 0;
    BPLib_CLA_AutoEgressEnabled[ContId] = true;

    UtAssert_EQ(bool, BPLib_CLA_GetAutoEgress(ContId), true);
}


void Test_BPLib_CLA_GetAutoEgress_ContIdErr(void)
{
    uint32_t ContId = BPLIB_MAX_NUM_CONTACTS;

    UtAssert_EQ(bool, BPLib_CLA_GetAutoEgress(ContId), false);
}

void TestBplibCla_Register(void)
{
    ADD_TEST(Test_BPLib_CLA_Egress_Nominal);

    ADD_TEST(Test_BPLib_CLA_ContactsTblValidateFunc_Nominal);
    ADD_TEST(Test_BPLib_CLA_ContactsTblValidateFunc_DtnDestEid);
    ADD_TEST(Test_BPLib_CLA_ContactsTblValidateFunc_InvDestEid);
    ADD_TEST(Test_BPLib_CLA_ContactsTblValidateFunc_InvTimeout);
    ADD_TEST(Test_BPLib_CLA_ContactsTblValidateFunc_InvTimeTrig);
    ADD_TEST(Test_BPLib_CLA_ContactsTblValidateFunc_InvSizeTrig);

    ADD_TEST(Test_BPLib_CLA_ContactSetup_Nominal);
    ADD_TEST(Test_BPLib_CLA_ContactSetup_InvalidContactId);
    ADD_TEST(Test_BPLib_CLA_ContactSetup_InvalidRunState);
    ADD_TEST(Test_BPLib_CLA_ContactSetup_CallbackError);

    ADD_TEST(Test_BPLib_CLA_ContactStart_Nominal);
    ADD_TEST(Test_BPLib_CLA_ContactStart_InvalidContactId);
    ADD_TEST(Test_BPLib_CLA_ContactStart_InvalidRunState);

    ADD_TEST(Test_BPLib_CLA_ContactStop_Nominal);
    ADD_TEST(Test_BPLib_CLA_ContactStop_InvalidContactId);
    ADD_TEST(Test_BPLib_CLA_ContactStop_InvalidRunState);

    ADD_TEST(Test_BPLib_CLA_ContactTeardown_Nominal);
    ADD_TEST(Test_BPLib_CLA_ContactTeardown_NullInst);
    ADD_TEST(Test_BPLib_CLA_ContactTeardown_InvalidContactId);
    ADD_TEST(Test_BPLib_CLA_ContactTeardown_InvalidRunState);
    ADD_TEST(Test_BPLib_CLA_ContactTeardown_StorErr);

    ADD_TEST(Test_BPLib_CLA_SetAutoEgress_Nominal);
    ADD_TEST(Test_BPLib_CLA_SetAutoEgress_ContIdErr);

    ADD_TEST(Test_BPLib_CLA_GetAutoEgress_Nominal);
    ADD_TEST(Test_BPLib_CLA_GetAutoEgress_ContIdErr);
}