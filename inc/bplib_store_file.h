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

#ifndef BPLIB_STORE_FILE_H
#define BPLIB_STORE_FILE_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bplib_os.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct
{
    const char *root_path;      /* local directory used to store bundles as files */
    int         cache_size;     /* number of bundles to store in cache (data_cache_t) */
    bool        flush_on_write; /* true: write-through cache */
} bp_file_attr_t;

typedef struct
{
    FILE *(*open)(const char *filename, const char *modes);
    int (*close)(FILE *stream);
    size_t (*read)(void *dst, size_t size, size_t n, FILE *stream);
    size_t (*write)(const void *src, size_t size, size_t n, FILE *stream);
    int (*seek)(FILE *stream, long int offset, int whence);
    int (*flush)(FILE *stream);
} bp_file_driver_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

/* Application API */
void bplib_store_file_init(bp_file_driver_t *driver);

/* Service API */
bp_handle_t bplib_store_file_create(int type, bp_ipn_t node, bp_ipn_t service, bool recover, void *parm);
int         bplib_store_file_destroy(bp_handle_t h);
int bplib_store_file_enqueue(bp_handle_t h, const void *data1, size_t data1_size, const void *data2, size_t data2_size,
                             int timeout);
int bplib_store_file_dequeue(bp_handle_t h, bp_object_t **object, int timeout);
int bplib_store_file_retrieve(bp_handle_t h, bp_sid_t sid, bp_object_t **object, int timeout);
int bplib_store_file_release(bp_handle_t h, bp_sid_t sid);
int bplib_store_file_relinquish(bp_handle_t h, bp_sid_t sid);
int bplib_store_file_getcount(bp_handle_t h);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* BPLIB_STORE_FILE_H */
