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

#ifndef RH_HASH_H
#define RH_HASH_H

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bundle_types.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct
{
    bp_active_bundle_t bundle; /* active bundle stored a this node */
    bp_index_t         next;   /* next entry in chain */
    bp_index_t         prev;   /* previous entry in chain */
    bp_index_t         after;  /* next entry added to hash (time ordered) */
    bp_index_t         before; /* previous entry added to hash (time ordered) */
} rh_hash_node_t;

typedef struct
{
    rh_hash_node_t *table;        /* hash table of active bundles */
    bp_index_t      size;         /* maximum (allocated) size of hash table */
    bp_index_t      num_entries;  /* number of active bundles in the hash table */
    bp_index_t      oldest_entry; /* oldest bundle in the hash table */
    bp_index_t      newest_entry; /* most recent bundle to be added to the hash table */
} rh_hash_t;

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

int rh_hash_create(rh_hash_t **rh_hash, int size);
int rh_hash_destroy(rh_hash_t *rh_hash);
int rh_hash_add(rh_hash_t *rh_hash, bp_active_bundle_t bundle, bool overwrite);
int rh_hash_next(rh_hash_t *rh_hash, bp_active_bundle_t *bundle);
int rh_hash_remove(rh_hash_t *rh_hash, bp_val_t cid, bp_active_bundle_t *bundle);
int rh_hash_available(rh_hash_t *rh_hash, bp_val_t cid);
int rh_hash_count(rh_hash_t *rh_hash);

#endif /* RH_HASH_H */
