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

#ifndef BIB_H
#define BIB_H

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
    bp_field_t bf;     /* block flags */
    bp_field_t blklen; /* block length */
    bp_field_t security_target_count;
    uint8_t    security_target_type;
    bp_field_t cipher_suite_id;
    bp_field_t cipher_suite_flags;
    bp_field_t compound_length;
    uint8_t    security_result_type;
    bp_field_t security_result_length;

    /* Security data is specified as a union here to avoid
     * allocating arrays when creating block definitions. */
    union
    {
        uint16_t crc16;
        uint32_t crc32;
    } security_result_data;
} bp_blk_bib_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int bib_init(void);
int bib_read(const void *block, int size, bp_blk_bib_t *bib, bool update_indices, uint32_t *flags);
int bib_write(void *block, int size, bp_blk_bib_t *bib, bool update_indices, uint32_t *flags);
int bib_update(void *block, int size, const void *payload, int payload_size, bp_blk_bib_t *bib, uint32_t *flags);
int bib_verify(const void *payload, int payload_size, bp_blk_bib_t *bib, uint32_t *flags);

#endif /* BIB_H */
