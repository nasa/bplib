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
 * Auto-Generated stub implementations for functions defined in bplib_crc header
 */

#include "bplib_crc.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CRC_Calculate()
 * ----------------------------------------------------
 */
BPLib_CRC_Val_t BPLib_CRC_Calculate(const void *Data, size_t DataLen, BPLib_CRC_Type_t CrcType)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CRC_Calculate, BPLib_CRC_Val_t);

    UT_GenStub_AddParam(BPLib_CRC_Calculate, const void *, Data);
    UT_GenStub_AddParam(BPLib_CRC_Calculate, size_t, DataLen);
    UT_GenStub_AddParam(BPLib_CRC_Calculate, BPLib_CRC_Type_t, CrcType);

    UT_GenStub_Execute(BPLib_CRC_Calculate, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CRC_Calculate, BPLib_CRC_Val_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CRC_Init()
 * ----------------------------------------------------
 */
void BPLib_CRC_Init(void)
{

    UT_GenStub_Execute(BPLib_CRC_Init, Basic, NULL);
}
