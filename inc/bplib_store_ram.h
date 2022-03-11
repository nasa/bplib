/*
 * NASA Docket No. GSC-18,587-1 and identified as “The Bundle Protocol Core Flight
 * System Application (BP) v6.5”
 *
 * Copyright © 2020 United States Government as represented by the Administrator of
 * the National Aeronautics and Space Administration. All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef BPLIB_STORE_RAM_H
#define BPLIB_STORE_RAM_H

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

/* Application API */
void bplib_store_ram_init(void);

/* Service API */
bp_handle_t bplib_store_ram_create(int type, bp_ipn_t node, bp_ipn_t service, bool recover, void *parm);

int bplib_store_ram_destroy(bp_handle_t h);
int bplib_store_ram_enqueue(bp_handle_t h, const void *data1, size_t data1_size, const void *data2, size_t data2_size,
                            int timeout);
int bplib_store_ram_dequeue(bp_handle_t h, bp_object_t **object, int timeout);
int bplib_store_ram_retrieve(bp_handle_t h, bp_sid_t sid, bp_object_t **object, int timeout);
int bplib_store_ram_release(bp_handle_t h, bp_sid_t sid);
int bplib_store_ram_relinquish(bp_handle_t h, bp_sid_t sid);
int bplib_store_ram_getcount(bp_handle_t h);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* BPLIB_STORE_RAM_H */
