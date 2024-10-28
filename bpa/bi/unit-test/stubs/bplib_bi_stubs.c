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
 * Auto-Generated stub implementations for functions defined in bplib_bi header
 */

#include "bplib_bi.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_BI_Init()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_BI_Init(void) 
{
  UT_GenStub_SetupReturnBuffer(BPLib_BI_Init, BPLib_Status_t);

  UT_GenStub_Execute(BPLib_BI_Init, Basic, NULL);

  return UT_GenStub_GetReturnValue(BPLib_BI_Init, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_BI_RecvCtrlMsg()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_BI_RecvCtrlMsg(BPLib_CLA_CtrlMsg_t *CtrlMsg) 
{
  UT_GenStub_SetupReturnBuffer(BPLib_BI_RecvCtrlMsg, BPLib_Status_t);

  UT_GenStub_AddParam(BPLib_BI_RecvCtrlMsg, BPLib_CLA_CtrlMsg_t *, CtrlMsg);

  UT_GenStub_Execute(BPLib_BI_RecvCtrlMsg, Basic, NULL);

  return UT_GenStub_GetReturnValue(BPLib_BI_RecvCtrlMsg, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_BI_RecvFullBundleIn()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_BI_RecvFullBundleIn(const void *BundleInPtr, size_t Size) 
{
  UT_GenStub_SetupReturnBuffer(BPLib_BI_RecvFullBundleIn, BPLib_Status_t);

  UT_GenStub_AddParam(BPLib_BI_RecvFullBundleIn, const void *, BundleInPtr);
  UT_GenStub_AddParam(BPLib_BI_RecvFullBundleIn, size_t, Size);

  UT_GenStub_Execute(BPLib_BI_RecvFullBundleIn, Basic, NULL);

  return UT_GenStub_GetReturnValue(BPLib_BI_RecvFullBundleIn, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_BI_SendFullBundleOut()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_BI_SendFullBundleOut(void *BundleOutPtr, size_t *Size) 
{
  UT_GenStub_SetupReturnBuffer(BPLib_BI_SendFullBundleOut, BPLib_Status_t);

  UT_GenStub_AddParam(BPLib_BI_SendFullBundleOut, void *, BundleOutPtr);
  UT_GenStub_AddParam(BPLib_BI_SendFullBundleOut, size_t *, Size);

  UT_GenStub_Execute(BPLib_BI_SendFullBundleOut, Basic, NULL);

  return UT_GenStub_GetReturnValue(BPLib_BI_SendFullBundleOut, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_BI_ValidateBundle()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_BI_ValidateBundle(void) 
{
  UT_GenStub_SetupReturnBuffer(BPLib_BI_ValidateBundle, BPLib_Status_t);

  UT_GenStub_Execute(BPLib_BI_ValidateBundle, Basic, NULL);

  return UT_GenStub_GetReturnValue(BPLib_BI_ValidateBundle, BPLib_Status_t);
}
