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

#ifndef CBUF_H
#define CBUF_H

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bundle_types.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* Circular Buffer Control Structure */
typedef struct
{
    bp_active_bundle_t *table;       /* circular array of active bundkes */
    bp_index_t          size;        /* maximum (allocated) size of circular array */
    bp_index_t          num_entries; /* number of bundles in the circular array */
    bp_val_t            newest_cid;  /* most recent custody id to be inserted into array */
    bp_val_t            oldest_cid;  /* oldest custody id still present in the array */
} cbuf_t;

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

int cbuf_create(cbuf_t **cbuf, int size);
int cbuf_destroy(cbuf_t *cbuf);
int cbuf_add(cbuf_t *cbuf, bp_active_bundle_t bundle, bool overwrite);
int cbuf_next(cbuf_t *cbuf, bp_active_bundle_t *bundle);
int cbuf_remove(cbuf_t *cbuf, bp_val_t cid, bp_active_bundle_t *bundle);
int cbuf_available(cbuf_t *cbuf, bp_val_t cid);
int cbuf_count(cbuf_t *cbuf);

#endif /* CBUF_H */
