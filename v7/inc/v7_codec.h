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

#ifndef V7_CODEC_H
#define V7_CODEC_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "v7_mpool.h"
#include "v7_types.h"
#include "v7_decode.h"
#include "v7_encode.h"

size_t v7_compute_full_bundle_size(bplib_mpool_bblock_primary_t *cpb);
size_t v7_copy_full_bundle_out(bplib_mpool_bblock_primary_t *cpb, void *buffer, size_t buf_sz);
size_t v7_copy_full_bundle_in(bplib_mpool_bblock_primary_t *cpb, const void *buffer, size_t buf_sz);

#ifdef __cplusplus
}
#endif

#endif /* V7_CODEC_H */
