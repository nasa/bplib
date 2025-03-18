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

/*
void Test_BPLib_CLA_Init(void)
{
    UtAssert_INT32_EQ(BPLib_CLA_Init(), BPLIB_SUCCESS);
}

void Test_BPLib_CLA_Ingress_Nominal(void)
{
    uint32_t ContId = 0;
    uint8_t Bundle[5]= {0x9F, 0x0, 0x0, 0x0, 0x0};
    size_t Size = 10;
    uint32_t Timeout = 0;

    UtAssert_INT32_EQ(BPLib_CLA_Ingress(ContId, Bundle, Size, Timeout), BPLIB_SUCCESS);
}

void Test_BPLib_CLA_Egress_Nominal(void)
{
    uint32_t ContId = 0;
    uint8_t Bundle[]= {0x9F, 0x0, 0x0, 0x0, 0x0};
    size_t Size;
    uint32_t Timeout = 0;
    UtAssert_INT32_EQ(BPLib_CLA_Egress(ContId, Bundle, &Size, Timeout), BPLIB_SUCCESS);
}

void Test_BPLib_CLA_Ingress_MsgData(void)
{
    uint32_t ContId = 0;
    char Bundle[8]= {"BPNMSG"};
    size_t Size = 10;
    uint32_t Timeout = 0;
    UtAssert_INT32_EQ(BPLib_CLA_Ingress(ContId, Bundle, Size, Timeout), BPLIB_SUCCESS);
}

void Test_BPLib_CLA_Ingress_NonControlMsg(void)
{
    uint32_t ContId = 0;
    uint8_t Bundle[5]= {1, 0, 0, 0, 0};
    size_t Size = 10;
    uint32_t Timeout = 0;
    UT_SetDefaultReturnValue(UT_KEY(BPLib_CLA_IsAControlMsg), 0);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_CLA_ProcessControlMessage), BPLIB_ERROR);
    UtAssert_INT32_EQ(BPLib_CLA_Ingress(ContId, Bundle, Size, Timeout), BPLIB_SUCCESS);
}

void Test_BPLib_CLA_Ingress_NULLBundle(void)
{
    uint32_t ContId = 0;
    uint8_t* Bundle = NULL;
    size_t Size = 10;
    uint32_t Timeout = 0;
    UtAssert_INT32_EQ(BPLib_CLA_Ingress(ContId, Bundle, Size, Timeout), BPLIB_SUCCESS);
}

void Test_BPLib_CLA_Egress_NULLBundle(void)
{
    uint32_t ContId = 0;
    uint8_t* Bundle = NULL;
    size_t Size;
    uint32_t Timeout = 0;
    UtAssert_INT32_EQ(BPLib_CLA_Egress(ContId, Bundle, &Size, Timeout), BPLIB_SUCCESS);
}

void Test_BPLib_CLA_ContactsTblValidateFunc_Nominal(void)
{
    BPLib_CLA_ContactsTable_t TestTblData;
    memset(&TestTblData, 0, sizeof(TestTblData));
    TestTblData.ContactSet[0].PortNum = 10;
    UtAssert_INT32_EQ((int32) BPLib_CLA_ContactsTblValidateFunc(&TestTblData), (int32) BPLIB_SUCCESS);
}

void Test_BPLib_CLA_ContactsTblValidateFunc_Invalid(void)
{
    BPLib_CLA_ContactsTable_t TestTblData;
    memset(&TestTblData, 0, sizeof(TestTblData));

    // Error case should return BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE
    TestTblData.ContactSet[0].PortNum = 0;

    UtAssert_INT32_EQ(BPLib_CLA_ContactsTblValidateFunc(&TestTblData),
                                                BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE);
}
*/

void Test_BPLib_CLA_ContactSetup_Nominal(void)
{
    BPLib_Status_t          Status;
    uint32_t                ContactId;
    BPLib_CLA_ContactsSet_t ContactInfo;

    /* Set the ContactId to a valid value */
    ContactId = BPLIB_MAX_NUM_CONTACTS - 1;

    /* Prime the Contacts Configuration to return valid values */
    memset((void*) &ContactInfo, 0, sizeof(BPLib_CLA_ContactsSet_t));
    strncpy(ContactInfo.CLAddr, "0.0.0.0", sizeof(char) * 7); // I know a char is a byte, it's just robust this way...
    ContactInfo.PortNum = 1;
    BPLib_NC_ConfigPtrs.ContactsConfigPtr->ContactSet[ContactId] = ContactInfo;

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
    BPLib_CLA_ContactsSet_t ContactInfo;

    /* Set the ContactId to a valid value */
    ContactId = BPLIB_MAX_NUM_CONTACTS + 1;

    /* Set a valid run state */
    BPLib_CLA_ContactRunStates[ContactId] = BPLIB_CLA_TORNDOWN;

    /* Run the function under test */
    Status = BPLib_CLA_ContactSetup(ContactId);

    /* Verify that Status is success */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_CLA_CONTACTS_MAX_REACHED);

    /* Verify that run state is unchanged */
    UtAssert_EQ(BPLib_CLA_ContactRunState_t, BPLib_CLA_ContactRunStates[ContactId], BPLIB_CLA_TORNDOWN);

    /* Verify that the event was issued */
    BPLib_CLA_Test_Verify_Event(0,
                                BPLIB_CLA_CONTACTS_MAX_REACHED_DBG_EID,
                                "Max simultaneous contacts allowed (%d) has been reached");
}

void Test_BPLib_CLA_ContactSetup_InvalidRunState(void)
{
    BPLib_Status_t Status;
    uint32_t       ContactId;

    /* Set a invalid run state */
    BPLib_CLA_ContactRunStates[ContactId] = BPLIB_CLA_STARTED;

    /* Run the function under test */
    Status = BPLib_CLA_ContactSetup(ContactId);

    /* Verify that run state is unchanged */
    UtAssert_EQ(BPLib_CLA_ContactRunState_t, BPLib_CLA_ContactRunStates[ContactId], BPLIB_CLA_STARTED);

    /* Verify that the correct event was issued */
    BPLib_CLA_Test_Verify_Event(0,
                                BPLIB_CLA_CONTACT_NO_STATE_CHG_DBG_EID,
                                "Contact with ID %d is already set up or has exited");
}

void Test_BPLib_CLA_ContactSetup_CallbackError(void)
{

}

void Test_BPLib_CLA_ContactSetup_SetContactStateError(void)
{

}

void TestBplibCla_Register(void)
{
    /*
    UtTest_Add(Test_BPLib_CLA_Init, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Init");
    UtTest_Add(Test_BPLib_CLA_Ingress_Nominal, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Ingress_Nominal");
    UtTest_Add(Test_BPLib_CLA_Egress_Nominal, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Egress_Nominal");
    UtTest_Add(Test_BPLib_CLA_Ingress_MsgData, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Ingress_MsgData");
    UtTest_Add(Test_BPLib_CLA_Ingress_NonControlMsg, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Ingress_NonControlMsg");
    UtTest_Add(Test_BPLib_CLA_Ingress_NULLBundle, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Ingress_NULLBundle");
    UtTest_Add(Test_BPLib_CLA_Egress_NULLBundle, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Egress_NULLBundle");
    UtTest_Add(Test_BPLib_CLA_ContactsTblValidateFunc_Nominal, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_ContactsTblValidateFunc_Nominal");
    UtTest_Add(Test_BPLib_CLA_ContactsTblValidateFunc_Invalid, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_ContactsTblValidateFunc_Invalid");
    */
    UtTest_Add(Test_BPLib_CLA_ContactSetup_Nominal, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_ContactSetup_Nominal");
    UtTest_Add(Test_BPLib_CLA_ContactSetup_InvalidContactId, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_ContactSetup_InvalidContactId");
    UtTest_Add(Test_BPLib_CLA_ContactSetup_InvalidRunState, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_ContactSetup_InvalidRunState");
    UtTest_Add(Test_BPLib_CLA_ContactSetup_CallbackError, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_ContactSetup_CallbackError");
    UtTest_Add(Test_BPLib_CLA_ContactSetup_SetContactStateError, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_ContactSetup_SetContactStateError");
}
