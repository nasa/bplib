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

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "v7_decode_internal.h"
#include "v7_encode_internal.h"

/*
 * -----------------------------------------------------------------------------------
 * IMPLEMENTATION
 * Helpers for encoding/decoding of individual protocol elements
 * -----------------------------------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------------
 * CONTAINER - encode/decode generic BP containers.
 * Caller must supply a callback function for the content inside the container.
 * -----------------------------------------------------------------------------------
 */

void v7_encode_container(v7_encode_state_t *enc, size_t entries, v7_encode_func_t func, const void *arg)
{

    if (!enc->error)
    {
        QCBOREncode_OpenArray(enc->cbor);

        /* call the handler impl */
        func(enc, arg);
        
        QCBOREncode_CloseArray(enc->cbor);
    }
}

void v7_decode_container(v7_decode_state_t *dec, size_t entries, v7_decode_func_t func, void *arg)
{
    QCBORItem item;

    if (!dec->error)
    {
        
        QCBORDecode_EnterArray(dec->cbor, &item);
        if (item.uDataType != QCBOR_TYPE_ARRAY)
        {
            dec->error = true;
        }
        else
        {
            /* call the handler impl */
            func(dec, arg);
                        
            QCBORDecode_ExitArray(dec->cbor);
        }
    }
}
