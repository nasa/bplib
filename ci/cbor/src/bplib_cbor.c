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
** Include
*/

#include <stdio.h>

#include "bplib_cbor.h"

#include "qcbor/qcbor_encode.h"
#include "qcbor/qcbor_decode.h"


// Temporary local prototype for the simple QCBOR test function.
int32_t BPLib_CBOR_SimpleValuesTest1(void);


/*
 ** Macros
 */

#define CheckResults(Enc, Expected) \
   UsefulBuf_Compare(Enc, (UsefulBufC){Expected, sizeof(Expected)})

/*
** Function Definitions
*/

int32_t BPLib_CBOR_Init(void) {
    int status;
    status = BPLib_CBOR_SimpleValuesTest1();
    fprintf(stderr, "BPLib_CBOR_Init: QCBOR test returned %d.", status);
    return BPLIB_SUCCESS;
}

// One big buffer that can be used by any test in this file.

static uint8_t spBigBuf[2200];

/*
 85                  # array(5)
   F5               # primitive(21)
   F4               # primitive(20)
   F6               # primitive(22)
   F7               # primitive(23)
   A1               # map(1)
      65            # text(5)
         554E446566 # "UNDef"
      F7            # primitive(23)
 */
static const uint8_t spExpectedEncodedSimple[] = {
   0x85, 0xf5, 0xf4, 0xf6, 0xf7, 0xa1, 0x65, 0x55, 0x4e, 0x44, 0x65, 0x66, 0xf7};

int32_t BPLib_CBOR_SimpleValuesTest1(void)
{
   QCBOREncodeContext ECtx;
   int nReturn = 0;

   QCBOREncode_Init(&ECtx, UsefulBuf_FROM_BYTE_ARRAY(spBigBuf));
   QCBOREncode_OpenArray(&ECtx);

   QCBOREncode_AddBool(&ECtx, true);
   QCBOREncode_AddBool(&ECtx, false);
   QCBOREncode_AddNULL(&ECtx);
   QCBOREncode_AddUndef(&ECtx);

   QCBOREncode_OpenMap(&ECtx);

   QCBOREncode_AddUndefToMapSZ(&ECtx, "UNDef");
   QCBOREncode_CloseMap(&ECtx);

   QCBOREncode_CloseArray(&ECtx);

   UsefulBufC ECBOR;
   if(QCBOREncode_Finish(&ECtx, &ECBOR)) {
      nReturn = -1;
   }

   if(CheckResults(ECBOR, spExpectedEncodedSimple))
      return -2;

   return(nReturn);
}
