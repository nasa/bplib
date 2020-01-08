/************************************************************************
 * File: flash_driver_sim.c
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
#include "flash_driver_sim.h"

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * flash_driver_page_read -
 *-------------------------------------------------------------------------------------*/
int flash_driver_page_read (bp_flash_addr_t addr, void* data, int size)
{
    if(addr.type == BP_FLASH_DATA_TYPE)
    {              
    }
    else if(addr.type == BP_FLASH_SPARE_TYPE)
    {
    }
    else
    {
        OS_printf("Invalid flash type to read: %d\n", addr.type);
        return BP_FAILEDSTORE;
    }    
    
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * flash_driver_page_write -
 *-------------------------------------------------------------------------------------*/
int flash_driver_page_write (bp_flash_addr_t addr, void* data, int size)
{
    (void)size;
    
    if(addr.type == BP_FLASH_DATA_TYPE)
    {        
    }
    else if(addr.type == BP_FLASH_SPARE_TYPE)
    {
    }
    else
    {
        OS_printf("Invalid flash type to write: %d\n", addr.type);
        return BP_FAILEDSTORE;
    }    
    
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * flash_driver_block_erase -
 *-------------------------------------------------------------------------------------*/
int flash_driver_block_erase (bp_flash_index_t block)
{
    return BP_SUCCESS;
}
