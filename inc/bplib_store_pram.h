/************************************************************************
 * File: bplib_store_pram.h
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

#ifndef __BPLIB_STORE_PRAM__
#define __BPLIB_STORE_PRAM__

#ifdef __cplusplus
extern "C" {
#endif 

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

void    bplib_store_pram_init           (void);
int     bplib_store_pram_create         (void);
int     bplib_store_pram_destroy        (int handle);
int     bplib_store_pram_enqueue        (int handle, void* data1, int data1_size, void* data2, int data2_size, int timeout);
int     bplib_store_pram_dequeue        (int handle, void** data, int* size, bp_sid_t* sid, int timeout);
int     bplib_store_pram_retrieve       (int handle, void** data, int* size, bp_sid_t sid, int timeout);
int     bplib_store_pram_relinquish     (int handle, bp_sid_t sid);
int     bplib_store_pram_getcount       (int handle);

#ifdef __cplusplus
} // extern "C"
#endif 

#endif
