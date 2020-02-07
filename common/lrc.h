/************************************************************************
 * File: lrc.h
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
#ifndef _lrc_h_
#define _lrc_h_

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

int     lrc_init            (int frame_size);
void    lrc_uninit          (void);
void    lrc_buffer_encode   (uint8_t* data_buffer, int data_size, uint8_t* ecc_buffer);
int     lrc_buffer_decode   (uint8_t* data_buffer, int data_size, uint8_t* ecc_buffer);

#endif /* _lrc_h_ */