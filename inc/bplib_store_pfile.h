/************************************************************************
 * File: bplib_store_pfile.h
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

#ifndef __BPLIB_STORE_PFILE__
#define __BPLIB_STORE_PFILE__

#ifdef __cplusplus
extern "C" {
#endif 

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    const char* root_path;
} pfile_attr_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

/* Application API */
void    bplib_store_pfile_init          (void);

/* Service API */
int     bplib_store_pfile_create        (void* parm);
int     bplib_store_pfile_destroy       (int handle);
int     bplib_store_pfile_enqueue       (int handle, void* data1, int data1_size, void* data2, int data2_size, int timeout);
int     bplib_store_pfile_dequeue       (int handle, void** data, int* size, bp_sid_t* sid, int timeout);
int     bplib_store_pfile_retrieve      (int handle, void** data, int* size, bp_sid_t sid, int timeout);
int     bplib_store_pfile_relinquish    (int handle, bp_sid_t sid);
int     bplib_store_pfile_getcount      (int handle);

#ifdef __cplusplus
} // extern "C"
#endif 

#endif
