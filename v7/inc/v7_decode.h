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

#ifndef V7_DECODE_H
#define V7_DECODE_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "v7_mpool.h"
#include "v7_types.h"

/*
 * On the decode side of things, the bundle buffer is passed in from the network/CLA and all that is known will
 * be a pointer and size.  The first block is always supposed to be primary (per BP) and every block thereafter
 * can be assumed canonical.  Payload of the bundle will be recorded as an offset and size into that block, there
 * is no separate output.
 */
int v7_block_decode_pri(bplib_mpool_bblock_primary_t *cpb, const void *data_ptr, size_t data_size);
int v7_block_decode_canonical(bplib_mpool_bblock_canonical_t *ccb, const void *data_ptr, size_t data_size,
                              bp_blocktype_t payload_block_hint);

#ifdef __cplusplus
}
#endif

#endif /* V7_DECODE_H */
