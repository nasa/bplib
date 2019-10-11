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
#ifdef UNITTESTS

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
static void print_hash(rh_hash_t* rh_hash)
{
    int i, j;
    
    printf("\n------------------------\n");
    printf("HASH TABLE\n");
    printf("------------------------\n");
    printf("Size:               %d\n", (int)rh_hash->size);
    printf("Number of Entries:  %d\n", (int)rh_hash->num_entries);
    printf("Maximum Chain:      %d\n", (int)rh_hash->max_chain);
    
    if(rh_hash->oldest_entry != BP_MAX_INDEX)
    {
        printf("Oldest Entry:       %lu\n", (unsigned long)rh_hash->table[rh_hash->oldest_entry].bundle.cid);
    }
    
    if(rh_hash->newest_entry != BP_MAX_INDEX)
    {
        printf("Newest Entry:       %lu\n", (unsigned long)rh_hash->table[rh_hash->newest_entry].bundle.cid);
    }
    
    for(i = 0; i < rh_hash->size; i++)
    {
        printf("[%d] ", i);
        if(rh_hash->table[i].chain == 0)
        {
            printf("EMPTY");
        }
        else
        {
            printf("%-4lu ", (unsigned long)rh_hash->table[i].bundle.cid);
            printf("%u -- ", (int)rh_hash->table[i].chain);
                        
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
//            printf("| %d <-- time --> %d | %d <<-- hash -->> %d", rh_hash->table[i].before, rh_hash->table[i].after, rh_hash->table[i].prev, rh_hash->table[i].next);
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

    const int hash_size = 8;    
    bp_active_bundle_t bundle = {NULL, 0, 0};
    
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


    print_hash(rh_hash);

    cid = 0;
    ut_assert(rh_hash_next  (rh_hash, 7,   &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 1;
    ut_assert(rh_hash_next  (rh_hash, 7,   &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 2;
    ut_assert(rh_hash_next  (rh_hash, 7,   &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 3;
    ut_assert(rh_hash_next  (rh_hash, 7,   &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 4;
    ut_assert(rh_hash_next  (rh_hash, 7,   &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 5;
    ut_assert(rh_hash_next  (rh_hash, 7,   &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 6;
    ut_assert(rh_hash_next  (rh_hash, 7,   &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 7;
    ut_assert(rh_hash_next  (rh_hash, 7,   &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid, &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);


    print_hash(rh_hash);
    
    ut_assert(rh_hash_next(rh_hash, 7, &bundle) == BP_CIDNOTFOUND, "Failed to get CIDNOTFOUND error\n");
    ut_assert(rh_hash->num_entries == 0, "Failed to remove all entries\n");    
    ut_assert(rh_hash_destroy(rh_hash) == BP_SUCCESS, "Failed to destroy hash\n");
}

/*--------------------------------------------------------------------------------------
 * Test #2
 *--------------------------------------------------------------------------------------*/
static void test_2(void)
{
    rh_hash_t* rh_hash;
    bp_val_t cid, max_cid;

    const int hash_size = 8;    
    bp_active_bundle_t bundle = {NULL, 0, 0};
    
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

    max_cid = 11;

    print_hash(rh_hash);    

    cid = 0;
    ut_assert(rh_hash_next  (rh_hash, max_cid,  &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid,      &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    print_hash(rh_hash);    

    cid = 1;
    ut_assert(rh_hash_next  (rh_hash, max_cid,  &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid,      &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 2;
    ut_assert(rh_hash_next  (rh_hash, max_cid,  &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid,      &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 3;
    ut_assert(rh_hash_next  (rh_hash, max_cid,  &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid,      &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 8;
    ut_assert(rh_hash_next  (rh_hash, max_cid,  &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid,      &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 9;
    ut_assert(rh_hash_next  (rh_hash, max_cid,  &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid,      &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 10;
    ut_assert(rh_hash_next  (rh_hash, max_cid,  &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid,      &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    cid = 11;
    ut_assert(rh_hash_next  (rh_hash, max_cid,  &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to get next CID %d\n", cid);
    ut_assert(rh_hash_remove(rh_hash, cid,      &bundle) == BP_SUCCESS && bundle.cid == cid, "Failed to remove CID %d\n", cid);

    print_hash(rh_hash);
    
    ut_assert(rh_hash_next(rh_hash, max_cid, &bundle) == BP_CIDNOTFOUND, "Failed to get CIDNOTFOUND error\n");
    ut_assert(rh_hash->num_entries == 0, "Failed to remove all entries\n");    
    ut_assert(rh_hash_destroy(rh_hash) == BP_SUCCESS, "Failed to destroy hash\n");
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

int ut_rh_hash (void)
{
//   test_1();
    test_2();
    
    return ut_failures();
}

#endif /* UNITTESTS */
