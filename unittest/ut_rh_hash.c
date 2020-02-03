/************************************************************************
 * File: ut_rh_hash.c
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
#include "rh_hash.h"

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * print_hash
 *-------------------------------------------------------------------------------------*/
static void print_hash(rh_hash_t* rh_hash, const char* message)
{
    int i, j;

    printf("\n------------------------\n");
    printf("HASH TABLE: %s\n", message);
    printf("------------------------\n");
    printf("Size:               %d\n", (int)rh_hash->size);
    printf("Number of Entries:  %d\n", (int)rh_hash->num_entries);

    if(rh_hash->oldest_entry != BP_MAX_INDEX)
    {
        printf("Oldest Entry:       [%d] %lu\n", rh_hash->oldest_entry, (unsigned long)rh_hash->table[rh_hash->oldest_entry].bundle.cid);
    }
    else
    {
        printf("Oldest Entry:       N\n");
    }

    if(rh_hash->newest_entry != BP_MAX_INDEX)
    {
        printf("Newest Entry:       [%d] %lu\n", rh_hash->newest_entry, (unsigned long)rh_hash->table[rh_hash->newest_entry].bundle.cid);
    }
    else
    {
        printf("Newest Entry:       N\n");
    }

    for(i = 0; i < rh_hash->size; i++)
    {
        printf("[%d] ", i);
        if(rh_hash->table[i].bundle.sid == BP_SID_VACANT)
        {
            printf("EMPTY");
        }
        else
        {
            printf("%-4lu -- ", (unsigned long)rh_hash->table[i].bundle.cid);

            j = rh_hash->table[i].next;
            if(j == BP_MAX_INDEX) printf("   ");

            while(j != BP_MAX_INDEX)
            {
                printf("%-2d ", j);
                j = rh_hash->table[j].next;
            }

            printf("| ");
            if(rh_hash->table[i].before != BP_MAX_INDEX)    printf("%d", rh_hash->table[i].before);
            else                                            printf("N");
            printf(" <--t--> ");
            if(rh_hash->table[i].after != BP_MAX_INDEX)     printf("%d", rh_hash->table[i].after);
            else                                            printf("N");
            printf(" | ");
            if(rh_hash->table[i].prev != BP_MAX_INDEX)      printf("%d", rh_hash->table[i].prev);
            else                                            printf("N");
            printf(" <<-h->> ");
            if(rh_hash->table[i].next != BP_MAX_INDEX)      printf("%d", rh_hash->table[i].next);
            else                                            printf("N");
        }
        printf("\n");
    }
}

/******************************************************************************
 TEST FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * Test #1
 *--------------------------------------------------------------------------------------*/
static void test_1(void)
{
    rh_hash_t* rh_hash;
    bp_val_t cid;

    int hash_size = 8;
    bp_active_bundle_t bundle = {1, 0, 0};

    printf("\n==== Test 1: Create/Destroy ====\n");

    ut_assert(rh_hash_create(&rh_hash, hash_size) == BP_SUCCESS, "Failed to create hash\n");

    bundle.cid = 0; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 1; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 2; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 3; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 4; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 5; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 6; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 7; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);

    ut_assert(rh_hash_count(rh_hash) == 8, "Failed to get hash size of 8\n");

    print_hash(rh_hash, "Step 1.1");

    cid = 0;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);
    ut_assert(rh_hash_count (rh_hash) == 7, "Failed to get hash size of 7\n");

    cid = 1;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);
    ut_assert(rh_hash_count (rh_hash) == 6, "Failed to get hash size of 6\n");

    cid = 2;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);
    ut_assert(rh_hash_count (rh_hash) == 5, "Failed to get hash size of 5\n");

    cid = 3;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);
    ut_assert(rh_hash_count (rh_hash) == 4, "Failed to get hash size of 4\n");

    cid = 4;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);
    ut_assert(rh_hash_count (rh_hash) == 3, "Failed to get hash size of 3\n");

    cid = 5;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);
    ut_assert(rh_hash_count (rh_hash) == 2, "Failed to get hash size of 2\n");

    cid = 6;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);
    ut_assert(rh_hash_count (rh_hash) == 1, "Failed to get hash size of 1\n");

    cid = 7;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);
    ut_assert(rh_hash_count (rh_hash) == 0, "Failed to get hash size of 0\n");


    print_hash(rh_hash, "Step 1.2");

    ut_assert(rh_hash_next(rh_hash, &bundle) == BP_CIDNOTFOUND, "Failed to get CIDNOTFOUND error\n");
    ut_assert(rh_hash->num_entries == 0, "Failed to remove all entries\n");
    ut_assert(rh_hash_destroy(rh_hash) == BP_SUCCESS, "Failed to destroy hash\n");
}

/*--------------------------------------------------------------------------------------
 * Test #2
 *--------------------------------------------------------------------------------------*/
static void test_2(void)
{
    rh_hash_t* rh_hash;
    bp_val_t cid;

    int hash_size = 8;
    bp_active_bundle_t bundle = {1, 0, 0};

    printf("\n==== Test 2: Chaining ====\n");

    ut_assert(rh_hash_create(&rh_hash, hash_size) == BP_SUCCESS, "Failed to create hash\n");

    bundle.cid = 0;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 1;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 2;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 3;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 8;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 9;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 10; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 11; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);

    print_hash(rh_hash, "Step 2.1");

    cid = 0;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    print_hash(rh_hash, "Step 2.2");

    cid = 1;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 2;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 3;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 8;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 9;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 10;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 11;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    print_hash(rh_hash, "Step 2.3");

    ut_assert(rh_hash_next(rh_hash, &bundle) == BP_CIDNOTFOUND, "Failed to get CIDNOTFOUND error\n");
    ut_assert(rh_hash->num_entries == 0, "Failed to remove all entries\n");
    ut_assert(rh_hash_destroy(rh_hash) == BP_SUCCESS, "Failed to destroy hash\n");
}

/*--------------------------------------------------------------------------------------
 * Test #3
 *--------------------------------------------------------------------------------------*/
static void test_3(void)
{
    rh_hash_t* rh_hash;
    bp_val_t cid;

    int hash_size = 16;
    bp_active_bundle_t bundle = {1, 0, 0};

    printf("\n==== Test 3: Remove First, Middle, Last in Chain ====\n");

    ut_assert(rh_hash_create(&rh_hash, hash_size) == BP_SUCCESS, "Failed to create hash\n");

    bundle.cid = 0;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 16; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 32; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 1;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 17; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 33; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 2;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 18; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 34; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);

    bundle.cid = 3;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 4;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 5;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 6;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 7;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 8;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 9;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);

    print_hash(rh_hash, "Step 3.1");

    /* Remove First */

    cid = 0;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 16;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 32;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    /* Remove Middle */

    cid = 17;
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 33;
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 1;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    /* Remove Last */

    cid = 34;
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 18;
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 2;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    /* Walk Down List */

    print_hash(rh_hash, "Step 3.2");

    cid = 3;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 4;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 5;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 6;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 7;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 8;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 9;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    /* Empty */

    ut_assert(rh_hash_next(rh_hash, &bundle) == BP_CIDNOTFOUND, "Failed to get CIDNOTFOUND error\n");
    ut_assert(rh_hash->num_entries == 0, "Failed to remove all entries\n");
    ut_assert(rh_hash_destroy(rh_hash) == BP_SUCCESS, "Failed to destroy hash\n");
}

/*--------------------------------------------------------------------------------------
 * Test #4
 *--------------------------------------------------------------------------------------*/
static void test_4(void)
{
    rh_hash_t* rh_hash;

    int hash_size = 16;
    bp_active_bundle_t bundle = {1, 0, 0};

    printf("\n==== Test 4: Duplicates ====\n");

    ut_assert(rh_hash_create(&rh_hash, hash_size) == BP_SUCCESS, "Failed to create hash\n");

    bundle.cid = 0;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 16; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 32; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 1;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 17; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 33; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 2;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 18; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 34; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);

    print_hash(rh_hash, "Step 4.1");

    bundle.cid = 0;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_DUPLICATECID, "Failed to reject duplicate CID %d\n", bundle.cid);
    bundle.cid = 16; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_DUPLICATECID, "Failed to reject duplicate CID %d\n", bundle.cid);
    bundle.cid = 32; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_DUPLICATECID, "Failed to reject duplicate CID %d\n", bundle.cid);
    bundle.cid = 1;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_DUPLICATECID, "Failed to reject duplicate CID %d\n", bundle.cid);
    bundle.cid = 17; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_DUPLICATECID, "Failed to reject duplicate CID %d\n", bundle.cid);
    bundle.cid = 33; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_DUPLICATECID, "Failed to reject duplicate CID %d\n", bundle.cid);
    bundle.cid = 2;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_DUPLICATECID, "Failed to reject duplicate CID %d\n", bundle.cid);
    bundle.cid = 18; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_DUPLICATECID, "Failed to reject duplicate CID %d\n", bundle.cid);
    bundle.cid = 34; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_DUPLICATECID, "Failed to reject duplicate CID %d\n", bundle.cid);

    print_hash(rh_hash, "Step 4.1.1");

    bundle.cid = 0;  ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to overwrite duplicate CID %d\n", bundle.cid);
    bundle.cid = 16; ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to overwrite duplicate CID %d\n", bundle.cid);
    bundle.cid = 32; ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to overwrite duplicate CID %d\n", bundle.cid);
    bundle.cid = 1;  ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to overwrite duplicate CID %d\n", bundle.cid);
    bundle.cid = 17; ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to overwrite duplicate CID %d\n", bundle.cid);
    bundle.cid = 33; ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to overwrite duplicate CID %d\n", bundle.cid);
    bundle.cid = 2;  ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to overwrite duplicate CID %d\n", bundle.cid);
    bundle.cid = 18; ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to overwrite duplicate CID %d\n", bundle.cid);
    bundle.cid = 34; ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to overwrite duplicate CID %d\n", bundle.cid);

    print_hash(rh_hash, "Step 4.1.2");

    bundle.cid = 3;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 4;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 5;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 6;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 7;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 8;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 9;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);

    print_hash(rh_hash, "Step 4.2");

    bundle.cid = 0;  ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to overwrite duplicate CID %d\n", bundle.cid);
    bundle.cid = 16; ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to overwrite duplicate CID %d\n", bundle.cid);
    bundle.cid = 32; ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to overwrite duplicate CID %d\n", bundle.cid);
    bundle.cid = 1;  ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to overwrite duplicate CID %d\n", bundle.cid);
    bundle.cid = 17; ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to overwrite duplicate CID %d\n", bundle.cid);
    bundle.cid = 33; ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to overwrite duplicate CID %d\n", bundle.cid);
    bundle.cid = 2;  ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to overwrite duplicate CID %d\n", bundle.cid);
    bundle.cid = 18; ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to overwrite duplicate CID %d\n", bundle.cid);
    bundle.cid = 34; ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to overwrite duplicate CID %d\n", bundle.cid);

    bundle.cid = 3;  ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to overwrite duplicate CID %d\n", bundle.cid);
    bundle.cid = 4;  ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to overwrite duplicate CID %d\n", bundle.cid);
    bundle.cid = 5;  ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to overwrite duplicate CID %d\n", bundle.cid);
    bundle.cid = 6;  ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to overwrite duplicate CID %d\n", bundle.cid);
    bundle.cid = 7;  ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to overwrite duplicate CID %d\n", bundle.cid);
    bundle.cid = 8;  ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to overwrite duplicate CID %d\n", bundle.cid);
    bundle.cid = 9;  ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to overwrite duplicate CID %d\n", bundle.cid);

    /* Check Size */

    bundle.cid = 35; ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_ACTIVETABLEFULL, "Failed to detect full active table\n");
    ut_assert(rh_hash_count(rh_hash) == hash_size, "Failed to get hash size of %d\n", hash_size);

    /* Destroy */

    ut_assert(rh_hash_destroy(rh_hash) == BP_SUCCESS, "Failed to destroy hash\n");
}

/*--------------------------------------------------------------------------------------
 * Test #5
 *--------------------------------------------------------------------------------------*/
static void test_5(void)
{
    rh_hash_t* rh_hash;
    bp_val_t cid;

    int hash_size = 8;
    bp_active_bundle_t bundle = {1, 0, 0};

    printf("\n==== Test 5: Retransverse ====\n");

    ut_assert(rh_hash_create(&rh_hash, hash_size) == BP_SUCCESS, "Failed to create hash\n");

    bundle.cid = 0; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 1; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 2; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);

    ut_assert(rh_hash_count(rh_hash) == 3, "Failed to get hash size of 3\n");

    print_hash(rh_hash, "Step 5.1 - Waiting for DACS");

    cid = 0;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);

    cid = 0;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);

    cid = 0;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);

    print_hash(rh_hash, "Step 5.2 - Received Partial Acknowledgment");

    cid = 0;
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);
    ut_assert(rh_hash_count (rh_hash) == 2, "Failed to get hash size of 2\n");

    cid = 1;
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);
    ut_assert(rh_hash_count (rh_hash) == 1, "Failed to get hash size of 1\n");

    cid = 2;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);

    cid = 2;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);

    print_hash(rh_hash, "Step 5.3 - Retransmit and Acknowledgment");

    cid = 2;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);

    print_hash(rh_hash, "Step 5.3.1");

    cid = 2;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);

    cid = 2;
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);
    ut_assert(rh_hash_count (rh_hash) == 0, "Failed to get hash size of 0\n");

    print_hash(rh_hash, "Step 5.4 - Empty");

    cid = 2;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_CIDNOTFOUND && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_count (rh_hash) == 0, "Failed to get hash size of 0\n");

    cid = 2;
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_CIDNOTFOUND && bundle.cid == cid, "Failed to remove CID %d\n", cid);
    ut_assert(rh_hash_count (rh_hash) == 0, "Failed to get hash size of 0\n");

    cid = 2;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_CIDNOTFOUND, "Failed to get CIDNOTFOUND error for %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_CIDNOTFOUND, "Failed to get CIDNOTFOUND error when removing %d\n", cid);
    ut_assert(rh_hash_count (rh_hash) == 0, "Failed to get hash size of 0\n");

    ut_assert(rh_hash_destroy(rh_hash) == BP_SUCCESS, "Failed to destroy hash\n");
}

/*--------------------------------------------------------------------------------------
 * Test #6
 *--------------------------------------------------------------------------------------*/
static void test_6(void)
{
    rh_hash_t* rh_hash;
    bp_val_t cid, newcid;
    int i, j;

    int hash_size = 8;
    bp_active_bundle_t bundle = {1, 0, 0};

    printf("\n==== Test 6: Full Hash ====\n");

    ut_assert(rh_hash_create(&rh_hash, hash_size) == BP_SUCCESS, "Failed to create hash\n");

    bundle.cid = 0; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 1; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 2; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 3; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 4; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 5; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 6; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 7; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);

    ut_assert(rh_hash_count(rh_hash) == 8, "Failed to get hash size of 8\n");

    print_hash(rh_hash, "Step 6.1 - Verify Failed Add on Full Hash");

    bundle.cid = 0; ut_assert(rh_hash_add(rh_hash, bundle, false) != BP_SUCCESS, "Failed to error on adding CID to full hash, %d\n", bundle.cid);
    bundle.cid = 8; ut_assert(rh_hash_add(rh_hash, bundle, false) != BP_SUCCESS, "Failed to error on adding CID to full hash, %d\n", bundle.cid);
    bundle.cid = 9; ut_assert(rh_hash_add(rh_hash, bundle, false) != BP_SUCCESS, "Failed to error on adding CID to full hash, %d\n", bundle.cid);

    print_hash(rh_hash, "Step 6.2 - Verify Failed Add on Changing Full Hash");

    for(cid = 0; cid < (unsigned int)hash_size; cid++)
    {
        bundle.cid = cid;

        ut_assert(rh_hash_add(rh_hash, bundle, false) != BP_SUCCESS, "Failed to error on adding CID to full hash, %d\n", bundle.cid);
        ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS, "Failed to remove CID %d\n", cid);

        ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);

        bundle.cid = cid + hash_size;
        ut_assert(rh_hash_add(rh_hash, bundle, false) != BP_SUCCESS, "Failed to error on adding CID to full hash, %d\n", bundle.cid);

        bundle.cid = cid + hash_size + 1;
        ut_assert(rh_hash_add(rh_hash, bundle, false) != BP_SUCCESS, "Failed to error on adding CID to full hash, %d\n", bundle.cid);
    }

    print_hash(rh_hash, "Step 6.3 - Verify Failed Add on Overwritten Full Hash");

    for(cid = 0; cid < (unsigned int)hash_size; cid++)
    {
        bundle.cid = cid;

        ut_assert(rh_hash_add(rh_hash, bundle, false) != BP_SUCCESS, "Failed to error on adding CID to full hash, %d\n", bundle.cid);
        ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to overwrite CID %d\n", bundle.cid);

        bundle.cid = cid + hash_size;
        ut_assert(rh_hash_add(rh_hash, bundle, false) != BP_SUCCESS, "Failed to error on adding CID to full hash, %d\n", bundle.cid);

        bundle.cid = cid + hash_size + 1;
        ut_assert(rh_hash_add(rh_hash, bundle, false) != BP_SUCCESS, "Failed to error on adding CID to full hash, %d\n", bundle.cid);
    }

    print_hash(rh_hash, "Step 6.4 - Verify Failed Add on Changing/Overwritten Full Hash");

    newcid = hash_size;
    for(j = 0; j < 10; j++)
    {
        for(i = 0; i < hash_size/2; i++)
        {
            bundle.cid = rh_hash->table[i].bundle.cid;
            cid = bundle.cid;

            ut_assert(rh_hash_add(rh_hash, bundle, false) != BP_SUCCESS, "Failed to error on adding CID to full hash, %d\n", bundle.cid);

            /* Overwrite CID */
            ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to overwrite CID %d\n", bundle.cid);

            /* Replace CID (in place) */
            ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS, "Failed to remove CID %d\n", cid);
            bundle.cid = cid + newcid++;
            ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);

            /* Replace CID (out of place) */
            cid = rh_hash->table[(i + (hash_size/2)) % hash_size].bundle.cid;
            ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS, "Failed to remove CID %d\n", cid);
            bundle.cid = cid + newcid++;
            ut_assert(rh_hash_add(rh_hash, bundle, true) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);

            /* Check Size */
            bundle.cid = newcid;
            rh_hash_add(rh_hash, bundle, false);
            bundle.cid = newcid + 1;
            rh_hash_add(rh_hash, bundle, false);
            ut_assert(rh_hash_count(rh_hash) == 8, "Failed to get hash size of 8: %d\n", rh_hash_count(rh_hash));
        }
    }

    print_hash(rh_hash, "Step 6.5 - Final Hash");

    ut_assert(rh_hash_destroy(rh_hash) == BP_SUCCESS, "Failed to destroy hash\n");
}

/*--------------------------------------------------------------------------------------
 * Test #7
 *--------------------------------------------------------------------------------------*/
static void test_7(void)
{
    rh_hash_t* rh_hash;
    bp_val_t cid;

    int hash_size = 16;
    bp_active_bundle_t bundle = {1, 0, 0};

    printf("\n==== Test 7: Collisions - First, Middle, Last in Chain ====\n");

    ut_assert(rh_hash_create(&rh_hash, hash_size) == BP_SUCCESS, "Failed to create hash\n");

    bundle.cid = 0;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 16; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 32; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 1;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 17; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 33; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 2;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 18; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 34; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);

    bundle.cid = 40; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 50; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 66; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 48; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 35; ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 8;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);
    bundle.cid = 9;  ut_assert(rh_hash_add(rh_hash, bundle, false) == BP_SUCCESS, "Failed to add CID %d\n", bundle.cid);

    print_hash(rh_hash, "Step 7.1");

    cid = 0;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 16;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 32;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 17;
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 33;
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 1;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 34;
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 18;
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 2;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 40;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 50;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 66;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 48;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 35;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 8;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 9;
    ut_assert(rh_hash_next  (rh_hash, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    /* Empty */

    ut_assert(rh_hash_next(rh_hash, &bundle) == BP_CIDNOTFOUND, "Failed to get CIDNOTFOUND error\n");
    ut_assert(rh_hash->num_entries == 0, "Failed to remove all entries\n");
    ut_assert(rh_hash_destroy(rh_hash) == BP_SUCCESS, "Failed to destroy hash\n");
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

int ut_rh_hash (void)
{
    ut_reset();

    test_1();
    test_2();
    test_3();
    test_4();
    test_5();
    test_6();
    test_7();

    return ut_failures();
}
