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
#include "lrc.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define TEST_PAGE_DATA_SIZE     FLASH_SIM_PAGE_SIZE
#if TEST_PAGE_DATA_SIZE > FLASH_SIM_PAGE_SIZE
#error "Test page data size exceeds physical size of flash simulation page data size"
#endif

#define TEST_DATA_SIZE          (TEST_PAGE_DATA_SIZE * 3 + 200)
#define NUM_BUNDLES             200

/******************************************************************************
 EXTERNAL PROTOTYPES
 ******************************************************************************/

extern int flash_free_reclaim (bp_flash_index_t block);
extern int flash_free_allocate (bp_flash_index_t* block);
extern int flash_data_write (bp_flash_addr_t* addr, uint8_t* data, int size);
extern int flash_data_read (bp_flash_addr_t* addr, uint8_t* data, int size);

/******************************************************************************
 FILE DATA
 ******************************************************************************/

static bp_flash_driver_t flash_driver = {
    .num_blocks = FLASH_SIM_NUM_BLOCKS,
    .pages_per_block = FLASH_SIM_PAGES_PER_BLOCK,
    .page_size = TEST_PAGE_DATA_SIZE,
    .read = bplib_flash_sim_page_read,
    .write = bplib_flash_sim_page_write,
    .erase = bplib_flash_sim_block_erase,
    .isbad = bplib_flash_sim_block_is_bad,
    .phyblk = bplib_flash_sim_physical_block
};

static uint8_t test_data[TEST_DATA_SIZE], read_data[TEST_DATA_SIZE];

/******************************************************************************
 TEST FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * Test #1
 *--------------------------------------------------------------------------------------*/
static void test_1(void)
{
    unsigned int i;
    bp_flash_index_t block;

    printf("==== Test 1: Free Block Management ====\n");

    int reclaimed_blocks = bplib_store_flash_init(flash_driver, BP_FLASH_INIT_FORMAT, false);
    ut_assert(reclaimed_blocks == 256, "Failed to reclaim all blocks\n");

    printf("\n==== Step 1.1: Allocate All ====\n");
    for(i = 0; i < flash_driver.num_blocks; i++)
    {
        ut_assert(flash_free_allocate(&block) == BP_SUCCESS, "Failed to allocate block on %dth iteration\n", i);
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
    ut_assert(flash_free_allocate(&block) == BP_ERROR, "Incorrectly succeeded to allocate block when no blocks available\n");

    /* Uninitialize Driver */
    bplib_store_flash_uninit();
}

/*--------------------------------------------------------------------------------------
 * Test #2
 *--------------------------------------------------------------------------------------*/
static void test_2(void)
{
    int i, j;
    int h[FLASH_MAX_STORES];

    printf("\n==== Test 2: Service Creation/Deletion ====\n");

    int reclaimed_blocks = bplib_store_flash_init(flash_driver, BP_FLASH_INIT_FORMAT, false);
    ut_assert(reclaimed_blocks == 256, "Failed to reclaim all blocks\n");

    printf("\n==== Step 2.1: Create Max ====\n");
    for(i = 0; i < FLASH_MAX_STORES; i++)
    {
        h[i] = bplib_store_flash_create(NULL);
        ut_assert(h[i] != BP_INVALID_HANDLE, "Failed to create store on %dth iteration\n", i);
    }

    printf("\n==== Step 2.2: Check Full ====\n");
    j = bplib_store_flash_create(NULL);
    ut_assert(j == BP_INVALID_HANDLE, "Incorrectly created store when no more handles available\n");

    printf("\n==== Step 2.2: Clean Up Stores ====\n");
    for(i = 0; i < FLASH_MAX_STORES; i++)
    {
        ut_assert(bplib_store_flash_destroy(h[i]) == BP_SUCCESS, "Failed to destroy handle %d\n", h[i]);
    }

    printf("\n==== Step 2.3: Check Holes ====\n");
    for(i = 0; i < FLASH_MAX_STORES; i++)
    {
        h[i] = bplib_store_flash_create(NULL);
        ut_assert(h[i] != BP_INVALID_HANDLE, "Failed to create store on %dth iteration\n", i);
    }
    ut_assert(bplib_store_flash_destroy(h[3]) == BP_SUCCESS, "Failed to destroy handle %d\n", h[3]);
    h[3] = bplib_store_flash_create(NULL);
    ut_assert(h[3] != BP_INVALID_HANDLE, "Failed to create store\n", i);

    printf("\n==== Step 2.4: Clean Up Stores ====\n");
    for(i = 0; i < FLASH_MAX_STORES; i++)
    {
        ut_assert(bplib_store_flash_destroy(h[i]) == BP_SUCCESS, "Failed to destroy handle %d\n", h[i]);
    }

    /* Uninitialize Driver */
    bplib_store_flash_uninit();
}

/*--------------------------------------------------------------------------------------
 * Test #3
 *--------------------------------------------------------------------------------------*/
static void test_3(void)
{
    int status, i;
    bp_flash_addr_t addr;

    printf("\n==== Test 3: Read/Write Data ====\n");

    /* Initialize Driver */
    int reclaimed_blocks = bplib_store_flash_init(flash_driver, BP_FLASH_INIT_FORMAT, true);
    ut_assert(reclaimed_blocks == 256, "Failed to reclaim all blocks\n");

    /* Initialize Test Data */
    for(i = 0; i < TEST_DATA_SIZE; i++)
    {
        test_data[i] = i % 0xFF;
        read_data[i] = 0;
    }

    /* Write Test Data */
    status = flash_free_allocate(&addr.block);
    ut_assert(status == BP_SUCCESS, "Failed to allocate free block\n");
    if(status == BP_SUCCESS)
    {
        bp_flash_index_t saved_block = addr.block;
        addr.page = 0;
        status = flash_data_write (&addr, test_data, TEST_DATA_SIZE);
        ut_assert(status == BP_SUCCESS, "Failed to write data: %d\n", status);
        ut_assert(addr.page > 0, "Failed to increment page number: %d\n", addr.page);

        /* Read Test Data */
        addr.block = saved_block;
        addr.page = 0;
        status = flash_data_read (&addr, read_data, TEST_DATA_SIZE);
        ut_assert(status == BP_SUCCESS, "Failed to write data: %d\n", status);
        ut_assert(addr.page > 0, "Failed to increment page number: %d\n", addr.page);
        for(i = 0; i < TEST_DATA_SIZE; i++)
        {
            ut_assert(read_data[i] == test_data[i], "Failed to read correct data at %d, %02X != %02X\n", i, read_data[i], test_data[i]);
        }
    }

    /* Uninitialize Driver */
    bplib_store_flash_uninit();
}

/*--------------------------------------------------------------------------------------
 * Test #4
 *--------------------------------------------------------------------------------------*/
static void test_4(void)
{
    int i, h;

    printf("\n==== Test 4: Enqueue/Dequeue ====\n");

    /* Initialize Driver */
    int reclaimed_blocks = bplib_store_flash_init(flash_driver, BP_FLASH_INIT_FORMAT, true);
    ut_assert(reclaimed_blocks == 256, "Failed to reclaim all blocks\n");

    /* Initialize Test Data */
    for(i = 0; i < TEST_DATA_SIZE; i++)
    {
        test_data[i] = i % 0xFF;
        read_data[i] = 0;
    }

    /* Create Storage Service */
    bp_flash_attr_t attr = {TEST_DATA_SIZE};
    h = bplib_store_flash_create(&attr);
    ut_assert(h != BP_INVALID_HANDLE, "Failed to create storage service\n", i);

    /* Enqueue/Dequeue Test Data */
    bp_object_t* object = NULL;
    ut_assert(bplib_store_flash_enqueue(h, test_data, TEST_DATA_SIZE, NULL, 0, BP_CHECK) == BP_SUCCESS, "Failed to enqueue test data\n");
    ut_assert(bplib_store_flash_dequeue(h, &object, BP_CHECK) == BP_SUCCESS, "Failed to enqueue test data\n");
    if(object != NULL)
    {
        ut_assert(object->header.handle == h, "Incorrect handle in dequeued object: %d != %d\n", object->header.handle, h);
        ut_assert(object->header.size == TEST_DATA_SIZE, "Incorrect size in dequeued object: %d != %d\n", object->header.size, TEST_DATA_SIZE);
        for(i = 0; i < TEST_DATA_SIZE; i++)
        {
            ut_assert((uint8_t)object->data[i] == test_data[i], "Failed to dequeue correct data at %d, %02X != %02X\n", i, (uint8_t)object->data[i], test_data[i]);
        }
    }
    else
    {
        ut_assert(false, "Failed to dequeue object\n");
    }

    /* Destroy Storage Service */
    bplib_store_flash_destroy(h);

    /* Uninitialize Driver */
    bplib_store_flash_uninit();
}

/*--------------------------------------------------------------------------------------
 * Test #5 -
 *-------------------------------------------------------------------------------------*/
static void test_5 (void)
{
    extern void flash_page_encode(uint8_t* page_buffer);
    extern int flash_page_decode(uint8_t* page_buffer);

    int i, status1, status2;
    uint8_t test_buffer[2][TEST_PAGE_DATA_SIZE];

    printf("\n==== Test 5: ECC Encode/Decode ====\n");

    /* Initialize Driver */
    int reclaimed_blocks = bplib_store_flash_init(flash_driver, BP_FLASH_INIT_FORMAT, true);
    ut_assert(reclaimed_blocks == 256, "Failed to reclaim all blocks\n");

    /****************************************/
    /* Duplicate Internal Parameters of ECC */
    /****************************************/
    int ecc_block_size = 7;
    int ecc_code_bytes_per_block = 2;
    int page_data_size = (ecc_block_size * flash_driver.page_size) / (ecc_block_size + ecc_code_bytes_per_block);
    /****************************************/

    /* initialize test buffers */
    for(i = 0; i < page_data_size; i++)
    {
        test_buffer[0][i] = 0xA5;
        test_buffer[1][i] = 0x5A;
    }

    /* encode page */
    lrc_encode(test_buffer[0], page_data_size);
    lrc_encode(test_buffer[1], page_data_size);

    /* decode page - nominal, no errors */
    printf("\n==== Step 5.1: Nominal Decode - No Errors ====\n");
    status1 = lrc_decode(test_buffer[0], page_data_size);
    status2 = lrc_decode(test_buffer[1], page_data_size);
    ut_assert(status1 == BP_ECC_NO_ERRORS, "Failed to decode page buffer[0] with no errors (%d)\n", status1);
    ut_assert(status2 == BP_ECC_NO_ERRORS, "Failed to decode page buffer[1] with no errors (%d)\n", status2);

    /* decode page - SBE errors */
    printf("\n==== Step 5.2: Correctable Errors in Data ====\n");
    test_buffer[0][0] ^= 0x40;
    test_buffer[0][8] ^= 0x10;
    test_buffer[0][50] ^= 0x02;
    status1 = lrc_decode(test_buffer[0], page_data_size);
    ut_assert(status1 == BP_ECC_COR_ERRORS, "Failed to decode page buffer[0] with correctable errors (%d)\n", status1);

    /* decode page - ECC errors */
    printf("\n==== Step 5.3: Correctable Errors in ECC ====\n");
    test_buffer[1][page_data_size] ^= 0x01;
    test_buffer[1][page_data_size+8] ^= 0x08;
    test_buffer[1][page_data_size+50] ^= 0x08;
    status2 = lrc_decode(test_buffer[1], page_data_size);
    ut_assert(status2 == BP_ECC_COR_ERRORS, "Failed to decode page buffer[1] with correctable errors (%d)\n", status2);

    /* decode page - MBEerrors */
    printf("\n==== Step 5.4: Uncorrectable Errors in Data ====\n");
    test_buffer[0][0] ^= 0x40;
    test_buffer[0][0+1] ^= 0x40;
    status1 = lrc_decode(test_buffer[0], page_data_size);
    ut_assert(status1 == BP_ECC_UNCOR_ERRORS, "Failed to detect uncorrectable errors in page buffer[0]: (%d)\n", status1);

    test_buffer[0][8] ^= 0x10;
    test_buffer[0][8+1] ^= 0x10;
    status1 = lrc_decode(test_buffer[0], page_data_size);
    ut_assert(status1 == BP_ECC_UNCOR_ERRORS, "Failed to detect uncorrectable errors in page buffer[0]: (%d)\n", status1);

    test_buffer[0][50] ^= 0x02;
    test_buffer[0][50+1] ^= 0x02;
    status1 = lrc_decode(test_buffer[0], page_data_size);
    ut_assert(status1 == BP_ECC_UNCOR_ERRORS, "Failed to detect uncorrectable errors in page buffer[0]: (%d)\n", status1);

    /* decode page - MBEerrors in Spare */
    printf("\n==== Step 5.5: Uncorrectable Errors in ECC ====\n");
    test_buffer[1][page_data_size + 20] ^= 0x03;
    status2 = lrc_decode(test_buffer[1], page_data_size);
    ut_assert(status2 == BP_ECC_UNCOR_ERRORS, "Failed to detect uncorrectable errors in page buffer[1]: (%d)\n", status2);

    test_buffer[1][page_data_size + 28] ^= 0x18;
    status2 = lrc_decode(test_buffer[1], page_data_size);
    ut_assert(status2 == BP_ECC_UNCOR_ERRORS, "Failed to detect uncorrectable errors in page buffer[1]: (%d)\n", status2);

    test_buffer[1][page_data_size+44] ^= 0x24;
    status2 = lrc_decode(test_buffer[1], page_data_size);
    ut_assert(status2 == BP_ECC_UNCOR_ERRORS, "Failed to detect uncorrectable errors in page buffer[1]: (%d)\n", status2);

    /* Uninitialize Driver */
    bplib_store_flash_uninit();
}

/*--------------------------------------------------------------------------------------
 * Test #6
 *--------------------------------------------------------------------------------------*/
static void test_6(void)
{
    int i, h, b;
    bp_sid_t sids[NUM_BUNDLES];

    printf("\n==== Test 6: Relinquish ====\n");

    /* Initialize Driver */
    int reclaimed_blocks = bplib_store_flash_init(flash_driver, BP_FLASH_INIT_FORMAT, true);
    ut_assert(reclaimed_blocks == 256, "Failed to reclaim all blocks\n");

    /* Initialize Test Data */
    for(i = 0; i < TEST_DATA_SIZE; i++)
    {
        test_data[i] = i % 0xFF;
        read_data[i] = 0;
    }

    /* Create Storage Service */
    bp_flash_attr_t attr = {TEST_DATA_SIZE};
    h = bplib_store_flash_create(&attr);
    ut_assert(h != BP_INVALID_HANDLE, "Failed to create storage service\n", i);

    /* Enqueue Test Data */
    for(b = 0; b < NUM_BUNDLES; b++)
    {
        ut_assert(bplib_store_flash_enqueue(h, test_data, TEST_DATA_SIZE, NULL, 0, BP_CHECK) == BP_SUCCESS, "Failed to enqueue test data\n");
    }

    /* Dequeue Test Data */
    for(b = 0; b < NUM_BUNDLES; b++)
    {
        bp_object_t* object = NULL;
        ut_assert(bplib_store_flash_dequeue(h, &object, BP_CHECK) == BP_SUCCESS, "Failed to enqueue test data\n");
        bplib_store_flash_release(h, object->header.sid);
        sids[b] = object->header.sid;
    }

    /* Relinquish Test Data */
    for(b = 0; b < NUM_BUNDLES ; b++)
    {
        ut_assert(bplib_store_flash_relinquish(h, sids[b]) == BP_SUCCESS, "Failed to relinquish test data\n");
    }

    /* Destroy Storage Service */
    bplib_store_flash_destroy(h);

    /* Uninitialize Driver */
    bplib_store_flash_uninit();
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

int ut_flash (void)
{
    ut_reset();

    /* Global Setup */

    bplib_flash_sim_initialize();

    /* Test Cases */

    test_1();
    test_2();
    test_3();
    test_4();
    test_5();
    test_6();

    /* Clean Up */

    bplib_flash_sim_uninitialize();

    return ut_failures();
}
