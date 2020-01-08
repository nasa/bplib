/************************************************************************
 * File: ut_flash.c
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

#include "ut_assert.h"
#include "bplib_store_flash.h"
#include "bplib_flash_sim.h"

/******************************************************************************
 EXTERNAL PROTOTYPES
 ******************************************************************************/

extern int flash_free_reclaim (bp_flash_index_t block);
extern int flash_free_allocate (bp_flash_index_t* block);
extern int flash_data_write (bp_flash_addr_t* addr, uint8_t* data, int size);
extern int flash_data_read (bp_flash_addr_t* addr, uint8_t* data, int size);
extern int flash_data_delete (bp_flash_addr_t* addr, int size);

/******************************************************************************
 FILE DATA
 ******************************************************************************/

bp_flash_driver_t flash_driver = {
    .num_blocks = FLASH_SIM_NUM_BLOCKS,
    .pages_per_block = FLASH_SIM_PAGES_PER_BLOCK,
    .data_size = FLASH_SIM_DATA_SIZE,
    .spare_size = FLASH_SIM_SPARE_SIZE,
    .read = bplib_flash_sim_page_read,
    .write = bplib_flash_sim_page_write,
    .erase = bplib_flash_sim_block_erase
};

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/


/******************************************************************************
 TEST FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * Test #1 - Free Block Management
 *--------------------------------------------------------------------------------------*/
static void test_1(void)
{
    unsigned int i;
    bp_flash_index_t block;

    printf("\n==== Test 1: Free Block Management ====\n");

    int reclaimed_blocks = bplib_store_flash_init(flash_driver);
    printf("Number of Blocks Reclaimed: %d\n", reclaimed_blocks);

    printf("\n==== Step 1.1: Allocate All ====\n");
    for(i = 0; i < flash_driver.num_blocks; i++)
    {
        ut_assert(flash_free_allocate(&block) == BP_SUCCESS, "Failed to allocate block\n");
        ut_assert(block == i, "Failed to allocate block %d, allocated %d instead\n", i, block);        
    }

    printf("\n==== Step 1.2: Reclaim All In Reverse Order ====\n");
    for(i = 0; i < flash_driver.num_blocks; i++)
    {
        ut_assert(flash_free_reclaim(flash_driver.num_blocks - i - 1) == BP_SUCCESS, "Failed to reclaim block\n");
    }

    printf("\n==== Step 1.3: Re-Allocate All ====\n");
    for(i = 0; i < flash_driver.num_blocks; i++)
    {
        ut_assert(flash_free_allocate(&block) == BP_SUCCESS, "Failed to allocate block\n");
        ut_assert(block == flash_driver.num_blocks - i - 1, "Failed to allocate block %d, allocated %d instead\n", flash_driver.num_blocks - i - 1, block);        
    }

    printf("\n==== Step 1.4: Attempt Allocate On Empty List ====\n");
    ut_assert(flash_free_allocate(&block) == BP_FAILEDSTORE, "Incorrectly succeeded to allocate block when no blocks available\n");
}

/*--------------------------------------------------------------------------------------
 * Test #2
 *--------------------------------------------------------------------------------------*/
static void test_2(void)
{
    printf("\n==== Test 2: Service Creation/Deletion ====\n");

    int h = bplib_store_flash_create(NULL);
    printf("Handle Created: %d\n", h);
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

int ut_flash (void)
{
    test_1();
    test_2();
    
    return ut_failures();
}
