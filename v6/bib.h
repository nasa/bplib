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

#ifndef __BPLIB_BLK_BIB_H__
#define __BPLIB_BLK_BIB_H__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "sdnv.h"
#include "os_api.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define BP_BIB_BLK_TYPE         0xD

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    bp_sdnv_t   block_flags;
    bp_sdnv_t   block_length;
    bp_sdnv_t   security_target_count;
    bp_sdnv_t   security_target_type;
    bp_sdnv_t   security_target_sequence;
    bp_sdnv_t   cipher_suite_id;
    bp_sdnv_t   cipher_suite_flags;
    bp_sdnv_t   security_result_count;
    uint8_t     security_result_type;
    bp_sdnv_t   security_result_length;
    
    // Security data is specified as a union here to avoid mallocing a pointer when
    // creating block definitions.
    union
    {
        uint16_t crc16;
        uint32_t crc32;
    } security_result_data;
} bp_blk_bib_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

void bplib_blk_crc_init      ();
int bplib_blk_bib_read      (void* contents, int size, bp_blk_bib_t* bib, bool update_indices);
int bplib_blk_bib_write     (void* block, int size, bp_blk_bib_t* bib, bool update_indices);
int bplib_blk_bib_update    (void* block, int size, void* payload, int payload_size, bp_blk_bib_t* bib);
int bplib_blk_bib_verify    (void* payload, int payload_size, bp_blk_bib_t* bib);

#endif  /* __BPLIB_BLK_BIB_H__ */
