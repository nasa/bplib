/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking 
 * (DTN) Bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this 
 * file except in compliance with the License. You may obtain a copy of the License at 
 *
 * http://www.apache.org/licenses/LICENSE-2.0 
 *
 * Unless required by applicable law or agreed to in writing, software distributed under 
 * the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF 
 * ANY KIND, either express or implied. See the License for the specific language 
 * governing permissions and limitations under the License. The copyright notice to be 
 * included in the software is as follows: 
 *
 * Copyright 2025 United States Government as represented by the Administrator of the 
 * National Aeronautics and Space Administration. All Rights Reserved.
 *
 */

/*
 * Include
 */

#include "bplib_cla_test_utils.h"

/*
** Test function for
** bool BPLib_CLA_IsAControlMsg(const void *Bundle)
*/
void Test_BPLib_CLA_IsAControlMsgNominal(void)
{
    BPLib_CLA_CtrlMsg_t CtrlMsg;
    strcpy(CtrlMsg.CtrlMsgTag, "BPNMSG");
    UtAssert_BOOL_TRUE(BPLib_CLA_IsAControlMsg(&CtrlMsg, 6));
}

void Test_BPLib_CLA_ProcessControlMessageNominal(void)
{
    BPLib_CLA_CtrlMsg_t CtrlMsg;
    strcpy(CtrlMsg.CtrlMsgTag, "BPNMSG");
    UtAssert_INT32_EQ(BPLib_CLA_ProcessControlMessage(&CtrlMsg), BPLIB_SUCCESS);
}

void Test_BPLib_CLA_ProcessControlMessageSentIt(void)
{
    BPLib_CLA_CtrlMsg_t CtrlMsg;
    strcpy(CtrlMsg.CtrlMsgTag, "BPNMSG");
    CtrlMsg.MsgTypes = 0;
    UtAssert_INT32_EQ(BPLib_CLA_ProcessControlMessage(&CtrlMsg), BPLIB_SUCCESS);
}

void Test_BPLib_CLA_ProcessControlMessageComplete(void)
{
    BPLib_CLA_CtrlMsg_t CtrlMsg;
    strcpy(CtrlMsg.CtrlMsgTag, "BPNMSG");
    CtrlMsg.MsgTypes = 1;
    UtAssert_INT32_EQ(BPLib_CLA_ProcessControlMessage(&CtrlMsg), BPLIB_SUCCESS);
}

void TestBplibClaInternal_Register(void)
{
    UtTest_Add(Test_BPLib_CLA_IsAControlMsgNominal, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_IsAControlMsgNominal");
    UtTest_Add(Test_BPLib_CLA_ProcessControlMessageNominal, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_ProcessControlMessageNominal");
    UtTest_Add(Test_BPLib_CLA_ProcessControlMessageSentIt, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_ProcessControlMessageSentIt");
    UtTest_Add(Test_BPLib_CLA_ProcessControlMessageComplete, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_ProcessControlMessageComplete");
}


