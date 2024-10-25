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
 * Generated stub function for BPLib_CRCGet()
 * ----------------------------------------------------
 */
BPLib_CRC_Val_t BPLib_CRCGet(const void *data, const uint32_t length, BPLib_CRC_Parameters_t *params)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CRCGet, BPLib_CRC_Val_t);

    UT_GenStub_AddParam(BPLib_CRCGet, const void *, data);
    UT_GenStub_AddParam(BPLib_CRCGet, const uint32_t, length);
    UT_GenStub_AddParam(BPLib_CRCGet, BPLib_CRC_Parameters_t *, params);

    UT_GenStub_Execute(BPLib_CRCGet, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CRCGet, BPLib_CRC_Val_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CRCInit()
 * ----------------------------------------------------
 */
void BPLib_CRCInit(void)
{

    UT_GenStub_Execute(BPLib_CRCInit, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CRC_Finalize()
 * ----------------------------------------------------
 */
BPLib_CRC_Val_t BPLib_CRC_Finalize(BPLib_CRC_Parameters_t *params, BPLib_CRC_Val_t crc)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CRC_Finalize, BPLib_CRC_Val_t);

    UT_GenStub_AddParam(BPLib_CRC_Finalize, BPLib_CRC_Parameters_t *, params);
    UT_GenStub_AddParam(BPLib_CRC_Finalize, BPLib_CRC_Val_t, crc);

    UT_GenStub_Execute(BPLib_CRC_Finalize, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CRC_Finalize, BPLib_CRC_Val_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CRC_GetName()
 * ----------------------------------------------------
 */
const char *BPLib_CRC_GetName(BPLib_CRC_Parameters_t *params)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CRC_GetName, const char *);

    UT_GenStub_AddParam(BPLib_CRC_GetName, BPLib_CRC_Parameters_t *, params);

    UT_GenStub_Execute(BPLib_CRC_GetName, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CRC_GetName, const char *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CRC_GetWidth()
 * ----------------------------------------------------
 */
uint8_t BPLib_CRC_GetWidth(BPLib_CRC_Parameters_t *params)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CRC_GetWidth, uint8_t);

    UT_GenStub_AddParam(BPLib_CRC_GetWidth, BPLib_CRC_Parameters_t *, params);

    UT_GenStub_Execute(BPLib_CRC_GetWidth, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CRC_GetWidth, uint8_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CRC_InitialValue()
 * ----------------------------------------------------
 */
BPLib_CRC_Val_t BPLib_CRC_InitialValue(BPLib_CRC_Parameters_t *params)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CRC_InitialValue, BPLib_CRC_Val_t);

    UT_GenStub_AddParam(BPLib_CRC_InitialValue, BPLib_CRC_Parameters_t *, params);

    UT_GenStub_Execute(BPLib_CRC_InitialValue, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CRC_InitialValue, BPLib_CRC_Val_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CRC_Update()
 * ----------------------------------------------------
 */
BPLib_CRC_Val_t BPLib_CRC_Update(BPLib_CRC_Parameters_t *params, BPLib_CRC_Val_t crc, const void *data, size_t size)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CRC_Update, BPLib_CRC_Val_t);

    UT_GenStub_AddParam(BPLib_CRC_Update, BPLib_CRC_Parameters_t *, params);
    UT_GenStub_AddParam(BPLib_CRC_Update, BPLib_CRC_Val_t, crc);
    UT_GenStub_AddParam(BPLib_CRC_Update, const void *, data);
    UT_GenStub_AddParam(BPLib_CRC_Update, size_t, size);

    UT_GenStub_Execute(BPLib_CRC_Update, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CRC_Update, BPLib_CRC_Val_t);
}
