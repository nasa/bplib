/************************************************************************
 * File: bplib_flash_sim.h
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

#ifndef _bplib_flash_sim_h_
#define _bplib_flash_sim_h_

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

#define FLASH_SIM_NUM_BLOCKS      256
#define FLASH_SIM_PAGES_PER_BLOCK 128
#define FLASH_SIM_PAGE_SIZE       4096
#define FLASH_SIM_SPARE_SIZE      128

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int bplib_flash_sim_initialize(void);
int bplib_flash_sim_uninitialize(void);
int bplib_flash_sim_page_read(bp_flash_addr_t addr, void *page_data);
int bplib_flash_sim_page_write(bp_flash_addr_t addr, void *page_data);
int bplib_flash_sim_block_erase(bp_flash_index_t block);
int bplib_flash_sim_block_is_bad(bp_flash_index_t block);
int bplib_flash_sim_physical_block(bp_flash_index_t logblk);
int bplib_flash_sim_block_mark_bad(bp_flash_index_t block);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* _bplib_flash_sim_h_ */
