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

#ifndef BPLIB_STOR_QM_CODEC_H
#define BPLIB_STOR_QM_CODEC_H

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_mem.h"
#include "bplib_stor_cache_types.h"
#include "bplib_stor_cache_block.h"

#include "bplib_stor_qm_decode.h"
#include "bplib_stor_qm_encode.h"

size_t compute_full_bundle_size(BPLib_STOR_CACHE_BblockPrimary_t *cpb);
size_t copy_full_bundle_out(BPLib_STOR_CACHE_BblockPrimary_t *cpb, void *buffer, size_t buf_sz);
size_t copy_full_bundle_in(BPLib_STOR_CACHE_BblockPrimary_t *cpb, const void *buffer, size_t buf_sz);

#endif /* BPLIB_STOR_QM_CODEC_H */
