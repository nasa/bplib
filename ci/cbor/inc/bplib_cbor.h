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

#ifndef BPLIB_CBOR_H
#define BPLIB_CBOR_H

/*
** Include
*/

// TODO Remove stdint and stddef if not required
// #include <stdint.h>
// #include <stddef.h>

#include "bplib.h"
#include "bplib_api_types.h"
#include "bplib_bblocks.h"

/*
** Exported Functions
*/

/**
 * @brief The CBOR APIs provide various CBOR encode and decode functions that
 * use the BPLib_Bundle_t data structure for both input and output.
 * BPLib_Bundle_t has the BPLib_BBlocks_t bundle data structure and a blob for CBOR data.
 * The functions that are specific to a canonical block type are provided for convenience.
 */
BPLib_Status_t BPLib_CBOR_DecodeBundle(const void* CandBundle, size_t CandBundleLen, BPLib_Bundle_t *bundle);


#endif /* BPLIB_CBOR_H */
