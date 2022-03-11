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

#ifndef PAY_H
#define PAY_H

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bundle_types.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct
{
    bp_field_t     bf;      /* block flags */
    bp_field_t     blklen;  /* block length */
    const uint8_t *payptr;  /* pointer to payload */
    int            paysize; /* size in bytes of payload */
} bp_blk_pay_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int pay_read(const void *block, int size, bp_blk_pay_t *pay, bool update_indices, uint32_t *flags);
int pay_write(void *block, int size, bp_blk_pay_t *pay, bool update_indices, uint32_t *flags);

#endif /* PAY_H */
