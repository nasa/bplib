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
 * Auto-Generated stub implementations for functions defined in crc header
 */

#include "crc.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_crc_finalize()
 * ----------------------------------------------------
 */
bp_crcval_t bplib_crc_finalize(bplib_crc_parameters_t *params, bp_crcval_t crc)
{
    UT_GenStub_SetupReturnBuffer(bplib_crc_finalize, bp_crcval_t);

    UT_GenStub_AddParam(bplib_crc_finalize, bplib_crc_parameters_t *, params);
    UT_GenStub_AddParam(bplib_crc_finalize, bp_crcval_t, crc);

    UT_GenStub_Execute(bplib_crc_finalize, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_crc_finalize, bp_crcval_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_crc_get()
 * ----------------------------------------------------
 */
bp_crcval_t bplib_crc_get(const void *data, const uint32_t length, bplib_crc_parameters_t *params)
{
    UT_GenStub_SetupReturnBuffer(bplib_crc_get, bp_crcval_t);

    UT_GenStub_AddParam(bplib_crc_get, const void *, data);
    UT_GenStub_AddParam(bplib_crc_get, const uint32_t, length);
    UT_GenStub_AddParam(bplib_crc_get, bplib_crc_parameters_t *, params);

    UT_GenStub_Execute(bplib_crc_get, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_crc_get, bp_crcval_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_crc_get_name()
 * ----------------------------------------------------
 */
const char *bplib_crc_get_name(bplib_crc_parameters_t *params)
{
    UT_GenStub_SetupReturnBuffer(bplib_crc_get_name, const char *);

    UT_GenStub_AddParam(bplib_crc_get_name, bplib_crc_parameters_t *, params);

    UT_GenStub_Execute(bplib_crc_get_name, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_crc_get_name, const char *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_crc_get_width()
 * ----------------------------------------------------
 */
uint8_t bplib_crc_get_width(bplib_crc_parameters_t *params)
{
    UT_GenStub_SetupReturnBuffer(bplib_crc_get_width, uint8_t);

    UT_GenStub_AddParam(bplib_crc_get_width, bplib_crc_parameters_t *, params);

    UT_GenStub_Execute(bplib_crc_get_width, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_crc_get_width, uint8_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_crc_init()
 * ----------------------------------------------------
 */
void bplib_crc_init(void)
{

    UT_GenStub_Execute(bplib_crc_init, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_crc_initial_value()
 * ----------------------------------------------------
 */
bp_crcval_t bplib_crc_initial_value(bplib_crc_parameters_t *params)
{
    UT_GenStub_SetupReturnBuffer(bplib_crc_initial_value, bp_crcval_t);

    UT_GenStub_AddParam(bplib_crc_initial_value, bplib_crc_parameters_t *, params);

    UT_GenStub_Execute(bplib_crc_initial_value, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_crc_initial_value, bp_crcval_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_crc_update()
 * ----------------------------------------------------
 */
bp_crcval_t bplib_crc_update(bplib_crc_parameters_t *params, bp_crcval_t crc, const void *data, size_t size)
{
    UT_GenStub_SetupReturnBuffer(bplib_crc_update, bp_crcval_t);

    UT_GenStub_AddParam(bplib_crc_update, bplib_crc_parameters_t *, params);
    UT_GenStub_AddParam(bplib_crc_update, bp_crcval_t, crc);
    UT_GenStub_AddParam(bplib_crc_update, const void *, data);
    UT_GenStub_AddParam(bplib_crc_update, size_t, size);

    UT_GenStub_Execute(bplib_crc_update, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_crc_update, bp_crcval_t);
}
