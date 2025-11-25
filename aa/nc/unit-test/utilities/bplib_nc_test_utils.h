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

#ifndef BPLIB_NC_TEST_UTILS_H
#define BPLIB_NC_TEST_UTILS_H

/* ======== */
/* Includes */
/* ======== */

#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"

#include "bplib_nc_directives.h"
#include "bplib_nc_rwlock.h"
#include "bpa_fwp_stubs.h"     /* For ADUP stubs */
#include "bplib_em_handlers.h" /* For BPLib_EM_SendEvent handler */
#include "bplib_as_handlers.h" /* For BPLib_AS_Increment and BPLib_AS_Decrement handler */


/* ====== */
/* Macros */
/* ====== */

#define ADD_TEST(test) UtTest_Add(test, BPLib_NC_Test_Setup, BPLib_NC_Test_Teardown, #test) /* Add a test case */
#define UT_MAX_TABLE_TYPE_DEPTH 13 /* Max depth number of call details tracked for BPA_TABLEP_TableUpdate */

/* =========== */
/* Global Data */
/* =========== */

extern BPLib_NC_ConfigPtrs_t TestConfigPtrs;
extern uint8 Context_TableType[];
extern BPLib_NC_RWLock_t RWLock;

extern BPLib_PI_ChannelTable_t     TestChanTbl;
extern BPLib_CLA_ContactsTable_t   TestContactsTbl;
extern BPLib_ARP_CRSTable_t        TestCrsTbl;
extern BPLib_PDB_CustodianTable_t  TestCustodianTbl;
extern BPLib_PDB_CustodyTable_t    TestCustodyTbl;
extern BPLib_NC_MibPerNodeConfig_t TestMibPnTbl;
extern BPLib_NC_MIBConfigPSTable_t TestMibPsTbl;
extern BPLib_PDB_ReportToTable_t   TestReportTbl;
extern BPLib_PDB_SrcAuthTable_t    TestAuthTbl;
extern BPLib_PDB_SrcLatencyTable_t TestLatencyTbl;
extern BPLib_STOR_StorageTable_t   TestStorTbl;


/* =================== */
/* Function Prototypes */
/* =================== */

/* Event verification */
void BPLib_NC_Test_Verify_Event(uint16_t EventNum, int32_t EventID, const char* EventText);

/* AS increment argument verification */
void Test_BPLib_NC_VerifyIncrement(BPLib_EID_t EID, BPLib_AS_Counter_t Counter, uint32_t Amount, int16_t CallNum);

/* AS decrement argument verification */
void Test_BPLib_NC_VerifyDecrement(BPLib_EID_t EID, BPLib_AS_Counter_t Counter, uint32_t Amount, int16_t CallNum);

/* Verify values given to BPA_TABLEP_TableUpdate */
void BPNode_Test_TABLEP_TableUpdate(uint8 CallNum, uint8 TableType);

/* Fixtures */
void BPLib_NC_Test_Setup(void);
void BPLib_NC_Test_Teardown(void);
void BPLib_NC_Test_Setup_RWLock(void);
void BPLib_NC_Test_Teardown_RWLock(void);

void TestBplibNc_Register(void);
void TestBplibNcRwlock_Register(void);

#endif /* BPLIB_NC_TEST_UTILS_H */
