/************************************************************************
 * File: flash_sim.c
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

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_store_flash.h"
#include "bplib_flash_sim.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define FLASH_SIM_BAD_BLOCK_MARK    0xA5

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    uint8_t*                data;
    uint8_t*                spare;
} flash_driver_page_t;

typedef struct {
    flash_driver_page_t*    pages;
} flash_driver_block_t;

typedef struct {
    flash_driver_block_t*   blocks;
} flash_driver_device_t;

/******************************************************************************
 FILE DATA
 ******************************************************************************/

flash_driver_device_t flash_driver_device;
bool flash_sim_initialized = false;

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_flash_sim_initialize -
 *-------------------------------------------------------------------------------------*/
int bplib_flash_sim_initialize (void)
{
    if(flash_sim_initialized == false)
    {
        flash_sim_initialized = true;

        flash_driver_device.blocks = (flash_driver_block_t*)malloc(FLASH_SIM_NUM_BLOCKS * sizeof(flash_driver_block_t));

        if(flash_driver_device.blocks == NULL) return BP_ERROR;

        int b;
        for(b = 0; b < FLASH_SIM_NUM_BLOCKS; b++)
        {
            flash_driver_device.blocks[b].pages = (flash_driver_page_t*)malloc(FLASH_SIM_PAGES_PER_BLOCK * sizeof(flash_driver_page_t));
            if(flash_driver_device.blocks[b].pages == NULL) return BP_ERROR;

            int p;
            for(p = 0; p < FLASH_SIM_PAGES_PER_BLOCK; p++)
            {
                flash_driver_device.blocks[b].pages[p].data = (uint8_t*)malloc(FLASH_SIM_PAGE_SIZE);
                if(flash_driver_device.blocks[b].pages[p].data == NULL) return BP_ERROR;

                flash_driver_device.blocks[b].pages[p].spare = (uint8_t*)malloc(FLASH_SIM_SPARE_SIZE);
                if(flash_driver_device.blocks[b].pages[p].spare == NULL) return BP_ERROR;
                flash_driver_device.blocks[b].pages[0].spare[0] = 0xFF; /* initialize to good block */
            }
        }
    }

    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_flash_sim_uninitialize -
 *-------------------------------------------------------------------------------------*/
int bplib_flash_sim_uninitialize (void)
{
    if(flash_sim_initialized == true)
    {
        flash_sim_initialized = false;

        int b;
        for(b = 0; b < FLASH_SIM_NUM_BLOCKS; b++)
        {
            int p;
            for(p = 0; p < FLASH_SIM_PAGES_PER_BLOCK; p++)
            {
                free(flash_driver_device.blocks[b].pages[p].data);
                free(flash_driver_device.blocks[b].pages[p].spare);
            }
            free(flash_driver_device.blocks[b].pages);
        }
        free(flash_driver_device.blocks);
    }

    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_flash_sim_page_read -
 *-------------------------------------------------------------------------------------*/
int bplib_flash_sim_page_read (bp_flash_addr_t addr, void* page_data)
{
    memcpy(page_data, flash_driver_device.blocks[addr.block].pages[addr.page].data, FLASH_SIM_PAGE_SIZE);
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_flash_sim_page_write -
 *-------------------------------------------------------------------------------------*/
int bplib_flash_sim_page_write (bp_flash_addr_t addr, void* page_data)
{
    int i;
    uint8_t* byte_ptr = (uint8_t*)page_data;
    for(i = 0; i < FLASH_SIM_PAGE_SIZE; i++)
    {
        flash_driver_device.blocks[addr.block].pages[addr.page].data[i] &= byte_ptr[i];
    }
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_flash_sim_block_erase -
 *-------------------------------------------------------------------------------------*/
int bplib_flash_sim_block_erase (bp_flash_index_t block)
{
    int p;

    for(p = 0; p < FLASH_SIM_PAGES_PER_BLOCK; p++)
    {
        memset(flash_driver_device.blocks[block].pages[p].data, 0xFF, FLASH_SIM_PAGE_SIZE);
        memset(flash_driver_device.blocks[block].pages[p].spare, 0xFF, FLASH_SIM_SPARE_SIZE);
    }

    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_flash_sim_block_is_bad -
 *-------------------------------------------------------------------------------------*/
int bplib_flash_sim_block_is_bad (bp_flash_index_t block)
{
    return flash_driver_device.blocks[block].pages[0].spare[0] == FLASH_SIM_BAD_BLOCK_MARK;
}

/*--------------------------------------------------------------------------------------
 * bp_flash_physical_block -
 *-------------------------------------------------------------------------------------*/
int bplib_flash_sim_physical_block (bp_flash_index_t logblk)
{
    return (int)logblk;
}

/*--------------------------------------------------------------------------------------
 * bplib_flash_sim_block_mark_bad -
 *-------------------------------------------------------------------------------------*/
int bplib_flash_sim_block_mark_bad (bp_flash_index_t block)
{
    flash_driver_device.blocks[block].pages[0].spare[0] = FLASH_SIM_BAD_BLOCK_MARK;
    return BP_SUCCESS;
}
