/************************************************************************
 * File: crc.h
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
 *  Alexander Meade, Code 582 NASA GSFC
 *  Joe-Paul Swinski, Code 582 NASA GSFC
 *
 *************************************************************************/

#ifndef _crc_h_
#define _crc_h_

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/*
 * To keep the interface consistent the digest functions do I/O as 32 bit values.
 * For CRC algorithms of lesser width, the value is right-justified (LSB/LSW)
 */
typedef uint32_t bp_crcval_t;

/* Standard parameters for calculating a CRC. */
struct bplib_crc_parameters;
typedef const struct bplib_crc_parameters bplib_crc_parameters_t;

/*
 * CRC algorithms that are implemented in BPLIB
 * These definitions are always fixed/const
 */
extern bplib_crc_parameters_t BPLIB_CRC_NONE;
extern bplib_crc_parameters_t BPLIB_CRC16_X25;
extern bplib_crc_parameters_t BPLIB_CRC32_CASTAGNOLI;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

void    bplib_crc_init(void);

const char *bplib_crc_get_name(bplib_crc_parameters_t *params);
uint8_t     bplib_crc_get_width(bplib_crc_parameters_t *params);
bp_crcval_t bplib_crc_initial_value(bplib_crc_parameters_t *params);
bp_crcval_t bplib_crc_update(bplib_crc_parameters_t *params, bp_crcval_t crc, const void *data, size_t size);
bp_crcval_t bplib_crc_finalize(bplib_crc_parameters_t *params, bp_crcval_t crc);

bp_crcval_t bplib_crc_get(const uint8_t *data, const uint32_t length, bplib_crc_parameters_t *params);

#endif /* _crc_h_ */
