/************************************************************************
 * File: bplib_store_pfile.c
 *
 *  Copyright 2019 United States Government as represented by the
 *  Administrator of the National Aeronautics and Space Administration.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * Maintainer(s):
 *  Joe-Paul Swinski, Code 582 NASA GSFC
 *
 *************************************************************************/

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bplib_os.h"

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
    (void)data;
    (void)size;
    (void)sid;
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
    (void)data;
    (void)size;
    assert(data);
    assert(size);
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

/*--------------------------------------------------------------------------------------
 * bplib_store_pfile_getcount -
 *-------------------------------------------------------------------------------------*/
int bplib_store_pfile_getcount (int handle)
{
    (void)handle;
    return 0;
}
