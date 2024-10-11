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

#ifndef BPLIB_STOR_QM_ENCODE_H
#define BPLIB_STOR_QM_ENCODE_H

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_mem.h"
#include "bplib_stor_cache_types.h"

/*
 * On the encode side of things, the block types are known ahead of time.  Encoding of a payload block is separate
 * because the data needs to be passed in, but for all other canonical block types all the information should already be
 * in the logical data - so nothing extra is needed (but this may change as more block types get implemented, too).
 * One possible option would be to pass in NULL/0 for the block types that do not have separate data, to keep the APIs
 * more consistent.
 */
int v7_block_encode_pri(BPLib_STOR_CACHE_BblockPrimary_t *cpb);
int v7_block_encode_pay(BPLib_STOR_CACHE_BblockCanonical_t *ccb, const void *data_ptr, size_t data_size);

int v7_block_encode_canonical(BPLib_STOR_CACHE_BblockCanonical_t *ccb);

#endif /* BPLIB_STOR_QM_ENCODE_H */
