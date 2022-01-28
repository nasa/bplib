/************************************************************************
 * File: bib.h
 *
 *  Copyright 2019 United States Government as represented by the
 *  Administrator of the National Aeronautics and Space Administration.
 *  All Other Rights Reserved.
 *
 *  This software was created at NASA's Goddard Space Flight Center.
 *  This software is governed by the NASA Open Source Agreement and may be
 *  used, distributed and modified only pursuant to the terms of that
 *  agreement.
 *
 * Maintainer(s):
 *  Joe-Paul Swinski, Code 582 NASA GSFC
 *
 *************************************************************************/

#ifndef _bib_h_
#define _bib_h_

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

#endif /* _bib_h_ */
