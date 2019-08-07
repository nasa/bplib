/************************************************************************
 * File: sdnv.h
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

#ifndef __BPLIB_SDNV_H__
#define __BPLIB_SDNV_H__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "os_api.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    uint32_t    value;
    uint32_t    index;
    uint32_t    width;
} bp_sdnv_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int bplib_sdnv_read(uint8_t* block, int size, bp_sdnv_t* sdnv, uint16_t* flags);
int bplib_sdnv_write(uint8_t* block, int size, bp_sdnv_t sdnv, uint16_t* flags);

#endif  /* __BPLIB_SDNV_H__ */
