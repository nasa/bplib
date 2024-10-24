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

/*
 * Include
 */

#include "bplib_pdb_test_utils.h"

/*
** Test function for
** int BPLib_PDB_Init()
*/
void Test_BPLib_PDB_Init(void)
{
    UtAssert_INT32_EQ(BPLib_PDB_Init(), BPLIB_SUCCESS);
}

void Test_BPLib_PD_CustodianAuthTblValidateFunc_Nominal(void)
{
    BPLib_PD_CustodianTable_t TestTblData;
    memset(&TestTblData, 0, sizeof(TestTblData));
    UtAssert_INT32_EQ((int32) BPLib_PD_CustodianAuthTblValidateFunc(&TestTblData), (int32) BPLIB_SUCCESS);     
}

void Test_BPLib_PD_CustodianAuthTblValidateFunc_Invalid(void)
{
    BPLib_PD_CustodianTable_t TestTblData;
    memset(&TestTblData, 0, sizeof(TestTblData));

    /* Error case should return BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE */

    UtAssert_INT32_NEQ(BPLib_PD_CustodianAuthTblValidateFunc(&TestTblData), 
                                                BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE);
}

void Test_BPLib_PD_CustodyAuthTblValidateFunc_Nominal(void)
{
    BPLib_PD_CustodyTable_t TestTblData;
    memset(&TestTblData, 0, sizeof(TestTblData));
    UtAssert_INT32_EQ((int32) BPLib_PD_CustodyAuthTblValidateFunc(&TestTblData), (int32) BPLIB_SUCCESS);     
}

void Test_BPLib_PD_CustodyAuthTblValidateFunc_Invalid(void)
{
    BPLib_PD_CustodyTable_t TestTblData;
    memset(&TestTblData, 0, sizeof(TestTblData));

    /* Error case should return BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE */

    UtAssert_INT32_NEQ(BPLib_PD_CustodyAuthTblValidateFunc(&TestTblData), 
                                                BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE);
}

void Test_BPLib_PD_ReportToAuthTblValidateFunc_Nominal(void)
{
    BPLib_PD_ReportToTable_t TestTblData;
    memset(&TestTblData, 0, sizeof(TestTblData));
    UtAssert_INT32_EQ((int32) BPLib_PD_ReportToAuthTblValidateFunc(&TestTblData), (int32) BPLIB_SUCCESS);     
}

void Test_BPLib_PD_ReportToAuthTblValidateFunc_Invalid(void)
{
    BPLib_PD_ReportToTable_t TestTblData;
    memset(&TestTblData, 0, sizeof(TestTblData));

    /* Error case should return BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE */

    UtAssert_INT32_NEQ(BPLib_PD_ReportToAuthTblValidateFunc(&TestTblData), 
                                                BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE);
}

void Test_BPLib_PD_SrcAuthTblValidateFunc_Nominal(void)
{
    BPLib_PD_SrcAuthTable_t TestTblData;
    memset(&TestTblData, 0, sizeof(TestTblData));
    UtAssert_INT32_EQ((int32) BPLib_PD_SrcAuthTblValidateFunc(&TestTblData), (int32) BPLIB_SUCCESS);     
}

void Test_BPLib_PD_SrcAuthTblValidateFunc_Invalid(void)
{
    BPLib_PD_SrcAuthTable_t TestTblData;
    memset(&TestTblData, 0, sizeof(TestTblData));

    /* Error case should return BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE */

    UtAssert_INT32_NEQ(BPLib_PD_SrcAuthTblValidateFunc(&TestTblData), 
                                                BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE);
}

void Test_BPLib_PD_SrcLatencyTblValidateFunc_Nominal(void)
{
    BPLib_PD_SrcLatencyTable_t TestTblData;
    memset(&TestTblData, 0, sizeof(TestTblData));
    UtAssert_INT32_EQ((int32) BPLib_PD_SrcLatencyTblValidateFunc(&TestTblData), (int32) BPLIB_SUCCESS);     
}

void Test_BPLib_PD_SrcLatencyTblValidateFunc_Invalid(void)
{
    BPLib_PD_SrcLatencyTable_t TestTblData;
    memset(&TestTblData, 0, sizeof(TestTblData));

    /* Error case should return BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE */

    UtAssert_INT32_NEQ(BPLib_PD_SrcLatencyTblValidateFunc(&TestTblData), 
                                                BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE);
}

void TestBplibPdb_Register(void)
{
    UtTest_Add(Test_BPLib_PDB_Init, BPLib_PDB_Test_Setup, BPLib_PDB_Test_Teardown, "Test_BPLib_PDB_Init");
    
    UtTest_Add(Test_BPLib_PD_CustodianAuthTblValidateFunc_Nominal, BPLib_PDB_Test_Setup, BPLib_PDB_Test_Teardown, "Test_BPLib_PD_CustodianAuthTblValidateFunc_Nominal");
    UtTest_Add(Test_BPLib_PD_CustodianAuthTblValidateFunc_Invalid, BPLib_PDB_Test_Setup, BPLib_PDB_Test_Teardown, "Test_BPLib_PD_CustodianAuthTblValidateFunc_Invalid");

    UtTest_Add(Test_BPLib_PD_CustodyAuthTblValidateFunc_Nominal, BPLib_PDB_Test_Setup, BPLib_PDB_Test_Teardown, "Test_BPLib_PD_CustodyAuthTblValidateFunc_Nominal");
    UtTest_Add(Test_BPLib_PD_CustodyAuthTblValidateFunc_Invalid, BPLib_PDB_Test_Setup, BPLib_PDB_Test_Teardown, "Test_BPLib_PD_CustodyAuthTblValidateFunc_Invalid");
    
    UtTest_Add(Test_BPLib_PD_ReportToAuthTblValidateFunc_Nominal, BPLib_PDB_Test_Setup, BPLib_PDB_Test_Teardown, "Test_BPLib_PD_ReportToAuthTblValidateFunc_Nominal");
    UtTest_Add(Test_BPLib_PD_ReportToAuthTblValidateFunc_Invalid, BPLib_PDB_Test_Setup, BPLib_PDB_Test_Teardown, "Test_BPLib_PD_ReportToAuthTblValidateFunc_Invalid");

    UtTest_Add(Test_BPLib_PD_SrcAuthTblValidateFunc_Nominal, BPLib_PDB_Test_Setup, BPLib_PDB_Test_Teardown, "Test_BPLib_PD_SrcAuthTblValidateFunc_Nominal");
    UtTest_Add(Test_BPLib_PD_SrcAuthTblValidateFunc_Invalid, BPLib_PDB_Test_Setup, BPLib_PDB_Test_Teardown, "Test_BPLib_PD_SrcAuthTblValidateFunc_Invalid");

    UtTest_Add(Test_BPLib_PD_SrcLatencyTblValidateFunc_Nominal, BPLib_PDB_Test_Setup, BPLib_PDB_Test_Teardown, "Test_BPLib_PD_SrcLatencyTblValidateFunc_Nominal");
    UtTest_Add(Test_BPLib_PD_SrcLatencyTblValidateFunc_Invalid, BPLib_PDB_Test_Setup, BPLib_PDB_Test_Teardown, "Test_BPLib_PD_SrcLatencyTblValidateFunc_Invalid");
    
}
