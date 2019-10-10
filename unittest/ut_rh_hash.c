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
            printf("%4u      ", (int)rh_hash->table[j].chain);
            printf("(%8lu  ", (unsigned long)rh_hash->table[j].hash);
            printf("%lu  ) ", (unsigned long)rh_hash->table[j].bundle.cid);
            
            j = i;            
            while(j != BP_MAX_INDEX)
            {
                printf("%d ", j);
                j = rh_hash->table[j].next;
            }            
        }
        printf("\n");
    }
}

/******************************************************************************
 TEST FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * Test #1: Create/Destroy
 *--------------------------------------------------------------------------------------*/
static void test_1(void)
{
    const int hash_size = 8;    
    rh_hash_t* rh_hash;
    bp_active_bundle_t bundle = {NULL, 0, 0};
    
    printf("\n==== Test 1 ====\n");

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
    
    ut_assert(rh_hash_remove(rh_hash, 0, &bundle) == BP_SUCCESS && bundle.cid == 0, "Failed to remove CID %d\n", bundle.cid);
    ut_assert(rh_hash_remove(rh_hash, 1, &bundle) == BP_SUCCESS && bundle.cid == 1, "Failed to remove CID %d\n", bundle.cid);
    ut_assert(rh_hash_remove(rh_hash, 2, &bundle) == BP_SUCCESS && bundle.cid == 2, "Failed to remove CID %d\n", bundle.cid);
    ut_assert(rh_hash_remove(rh_hash, 3, &bundle) == BP_SUCCESS && bundle.cid == 3, "Failed to remove CID %d\n", bundle.cid);
    ut_assert(rh_hash_remove(rh_hash, 4, &bundle) == BP_SUCCESS && bundle.cid == 4, "Failed to remove CID %d\n", bundle.cid);
    ut_assert(rh_hash_remove(rh_hash, 5, &bundle) == BP_SUCCESS && bundle.cid == 5, "Failed to remove CID %d\n", bundle.cid);
    ut_assert(rh_hash_remove(rh_hash, 6, &bundle) == BP_SUCCESS && bundle.cid == 6, "Failed to remove CID %d\n", bundle.cid);
    ut_assert(rh_hash_remove(rh_hash, 7, &bundle) == BP_SUCCESS && bundle.cid == 7, "Failed to remove CID %d\n", bundle.cid);

    print_hash(rh_hash);

    ut_assert(rh_hash_destroy(rh_hash) == BP_SUCCESS, "Failed to destroy hash\n");
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

int ut_rh_hash (void)
{
    test_1();
    
    /* Return Failures */
    return ut_failures();
}

#endif /* UNITTESTS */
