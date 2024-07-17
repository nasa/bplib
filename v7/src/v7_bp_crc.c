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
 * CRC - encode/decode BP CRC values, width dependent on selected algorithm
 * -----------------------------------------------------------------------------------
 */

void v7_encode_crc(v7_encode_state_t *enc)
{
    size_t               crc_len;
    static const uint8_t CRC_PAD_BYTES[sizeof(bp_crcval_t)] = {0};
    UsefulBufC data_buf;
    QCBORError err;

    /*
     * NOTE: Unlike other encode functions, this does not accept a value passed in, as the
     * caller would have no way of knowing it - the CRC reflects the encoded data, but this
     * is called in the process of creating that encoded data.  So the value is pulled
     * from the state of the stream writer.
     *
     * Another bit of circular dependency here:
     *
     * to encode the CRC requires the value, but it has to be encoded in a byte string
     * first because the CRC includes/covers those CBOR encoding bits too.
     *
     * but the byte string cannot be encoded until the value is known....
     *
     * Note that TinyCBOR is being used in a streaming mode, thus we cannot write
     * a string and then "back up" - it may not be possible to reverse the stream.
     *
     * To get around this circular dependency for now, we can predict what the CBOR
     * encoding bytes _should_ be (always a byte string of the CRC length) and update
     * the CRC accordingly, then actually write the CRC.
     */

    crc_len = bplib_crc_get_width(enc->crc_params) / 8;

    if (crc_len > sizeof(CRC_PAD_BYTES))
    {
        enc->error = true;
    }
    else
    {
        enc->crc_flag = true;
        data_buf.ptr = CRC_PAD_BYTES;
        data_buf.len = crc_len;
        QCBOREncode_AddBytes(enc->cbor, data_buf);
        err = QCBOREncode_GetErrorState(enc->cbor);
        if(err != QCBOR_SUCCESS) 
        {
            enc->error = true;
        }
    }
}

void v7_decode_crc(v7_decode_state_t *dec, bp_crcval_t *v)
{
    bp_crcval_t crc_val;
    uint8_t     bytes[1 + sizeof(crc_val)] = {0};
    size_t      i;
    UsefulBufC  buff;

    crc_val = 0;

    
    if (!dec->error)
    {
        QCBORDecode_GetByteString(dec->cbor, &buff);
        memcpy(bytes, buff.ptr, buff.len);
        /* Interpret the bytestring value as an integer */
        for (i = 0; i < buff.len; ++i)
        {
            crc_val <<= 8;
            crc_val |= bytes[i];
        }
    }
    *v = crc_val;
}
