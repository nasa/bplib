/******************************************************************************
 * Filename     : bplib_store_pfile.c
 * Purpose      : Bundle Protocol Library
 *                POSIX-Compliant File System
 *                Storage Service
 * Design Notes :
 ******************************************************************************/

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <assert.h>
#include "bplib.h"

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_store_pfile_create -
 *-------------------------------------------------------------------------------------*/
int bplib_store_pfile_create (void)
{
    return BP_INVALID_HANDLE;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_pfile_destroy -
 *-------------------------------------------------------------------------------------*/
int bplib_store_pfile_destroy (int handle)
{
    (void)handle;
    return 0;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_pfile_enqueue -
 *-------------------------------------------------------------------------------------*/
int bplib_store_pfile_enqueue (int handle, void* data1, int data1_size, void* data2, int data2_size, int timeout)
{
    (void)handle;
    (void)data1;
    (void)data1_size;
    (void)data2;
    (void)data2_size;
    (void)timeout;
    return 0;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_pfile_dequeue -
 *-------------------------------------------------------------------------------------*/
int bplib_store_pfile_dequeue (int handle, void** data, int* size, bp_sid_t* sid, int timeout)
{
    (void)handle;
    (void)timeout;
    assert(data);
    assert(size);
    assert(sid);
    return 0;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_pfile_retrieve -
 *-------------------------------------------------------------------------------------*/
int bplib_store_pfile_retrieve (int handle, void** data, int* size, bp_sid_t sid, int timeout)
{
    (void)handle;
    (void)sid;
    (void)timeout;
    assert(data);
    assert(size);
    return 0;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_pfile_refresh -
 *-------------------------------------------------------------------------------------*/
int bplib_store_pfile_refresh (int handle, void* data, int size, int offset, bp_sid_t sid, int timeout)
{
    (void)handle;
    (void)size;
    (void)offset;
    (void)sid;
    (void)timeout;
    assert(data);
    return 0;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_pfile_relinquish -
 *-------------------------------------------------------------------------------------*/
int bplib_store_pfile_relinquish (int handle, bp_sid_t sid)
{
    (void)handle;
    (void)sid;
    return 0;
}

