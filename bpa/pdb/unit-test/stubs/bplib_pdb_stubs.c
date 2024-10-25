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
 * Auto-Generated stub implementations for functions defined in bplib_pdb header
 */

#include "bplib_pdb.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_PDB_Init()
 * ----------------------------------------------------
 */
int BPLib_PDB_Init(void)
{
    UT_GenStub_SetupReturnBuffer(BPLib_PDB_Init, int);

    UT_GenStub_Execute(BPLib_PDB_Init, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_PDB_Init, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_PDB_CustodianAuthTblValidateFunc()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_PDB_CustodianAuthTblValidateFunc(void *TblData) 
{
  UT_GenStub_SetupReturnBuffer(BPLib_PDB_CustodianAuthTblValidateFunc,
                               BPLib_Status_t);

  UT_GenStub_AddParam(BPLib_PDB_CustodianAuthTblValidateFunc, void *, TblData);

  UT_GenStub_Execute(BPLib_PDB_CustodianAuthTblValidateFunc, Basic, NULL);

  return UT_GenStub_GetReturnValue(BPLib_PDB_CustodianAuthTblValidateFunc,
                                   BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_PDB_CustodyAuthTblValidateFunc()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_PDB_CustodyAuthTblValidateFunc(void *TblData) 
{
  UT_GenStub_SetupReturnBuffer(BPLib_PDB_CustodyAuthTblValidateFunc,
                               BPLib_Status_t);

  UT_GenStub_AddParam(BPLib_PDB_CustodyAuthTblValidateFunc, void *, TblData);

  UT_GenStub_Execute(BPLib_PDB_CustodyAuthTblValidateFunc, Basic, NULL);

  return UT_GenStub_GetReturnValue(BPLib_PDB_CustodyAuthTblValidateFunc,
                                   BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_PDB_ReportToAuthTblValidateFunc()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_PDB_ReportToAuthTblValidateFunc(void *TblData) 
{
  UT_GenStub_SetupReturnBuffer(BPLib_PDB_ReportToAuthTblValidateFunc,
                               BPLib_Status_t);

  UT_GenStub_AddParam(BPLib_PDB_ReportToAuthTblValidateFunc, void *, TblData);

  UT_GenStub_Execute(BPLib_PDB_ReportToAuthTblValidateFunc, Basic, NULL);

  return UT_GenStub_GetReturnValue(BPLib_PDB_ReportToAuthTblValidateFunc,
                                   BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_PDB_SrcAuthTblValidateFunc()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_PDB_SrcAuthTblValidateFunc(void *TblData) 
{
  UT_GenStub_SetupReturnBuffer(BPLib_PDB_SrcAuthTblValidateFunc, BPLib_Status_t);

  UT_GenStub_AddParam(BPLib_PDB_SrcAuthTblValidateFunc, void *, TblData);

  UT_GenStub_Execute(BPLib_PDB_SrcAuthTblValidateFunc, Basic, NULL);

  return UT_GenStub_GetReturnValue(BPLib_PDB_SrcAuthTblValidateFunc,
                                   BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_PDB_SrcLatencyTblValidateFunc()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_PDB_SrcLatencyTblValidateFunc(void *TblData) 
{
  UT_GenStub_SetupReturnBuffer(BPLib_PDB_SrcLatencyTblValidateFunc,
                               BPLib_Status_t);

  UT_GenStub_AddParam(BPLib_PDB_SrcLatencyTblValidateFunc, void *, TblData);

  UT_GenStub_Execute(BPLib_PDB_SrcLatencyTblValidateFunc, Basic, NULL);

  return UT_GenStub_GetReturnValue(BPLib_PDB_SrcLatencyTblValidateFunc,
                                   BPLib_Status_t);
}
