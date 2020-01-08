/************************************************************************
 * File: flash_driver_sim.h
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

#ifndef _flash_driver_sim_h_
#define _flash_driver_sim_h_

#ifdef __cplusplus
extern "C" {
#endif 

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_store_flash.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define FLASH_DRIVER_NUM_BLOCKS         2048
#define FLASH_DRIVER_PAGES_PER_BLOCK    128
#define FLASH_DRIVER_DATA_SIZE          4096
#define FLASH_DRIVER_SPARE_SIZE         128

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int     flash_driver_page_read      (bp_flash_addr_t addr, void* data, int size);
int     flash_driver_page_write     (bp_flash_addr_t addr, void* data, int size);
int     flash_driver_block_erase    (bp_flash_index_t block);

#ifdef __cplusplus
} // extern "C"
#endif 

#endif /* _flash_driver_sim_h_ */
