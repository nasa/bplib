/************************************************************************
 * File: bib.h
 *
 *  Copyright 2019-2022 United States Government as represented by the
 *  Administrator of the National Aeronautics and Space Administration.
 *
 *  This software was created at NASA's Goddard Space Flight Center.
 *
 * Maintainer(s):
 *  Karl Sipfle, for Code 580 NASA GSFC
 *
 *************************************************************************/

#ifndef _bib_h_
#define _bib_h_

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "../../tinycbor-main/src/cbor.h"  // KRS (ION also has a cbor.h)
//#include "bplib.h"
#include "../inc/bplib.h"  // KRS
//#include "bundle_types.h"
#include "../lib/bundle_types.h"  // KRS

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    unsigned long blknum;  // v7  KRS
    unsigned long bf;                         /* block flags */
    unsigned long crctype; // v7
    unsigned long this_block_crc;  // v7

    unsigned long security_target_count;
    unsigned long target; // v7, SHOULD ACCOMODATE MORE THAN 1  KRS
    unsigned long context_id;
    unsigned long context_flags;
    unsigned long srcscheme;  // v7
    unsigned long srcnode;  // v7
    unsigned long srcserv;  // v7
    unsigned long security_result_length;

    /* Security data is specified as a union here to avoid
     * allocating arrays when creating block definitions. */
    union
    {
        // v6 uint16_t crc16;
        // v6 uint32_t crc32;
        uint64_t id_value_pair[2];  // v7  // HARDCODED ASSUMPTION OF 1 UINT TARGET RESULT  KRS
    } security_result_data;
} bp_blk_bib_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int     bib_init      (void);
int     bib_update    (void* block, int size, void* payload, int payload_size, bp_blk_bib_t* bib, uint32_t* flags);
int     bib_verify    (void* payload, int payload_size, bp_blk_bib_t* bib, uint32_t* flags);
int bib_decborize(CborValue *recursed, int size, bp_blk_bib_t* bib, uint32_t* flags);  // KRS
int bib_cborize(CborEncoder* encoder, bp_blk_bib_t* bib);  // KRS

#endif  /* _bib_h_ */
