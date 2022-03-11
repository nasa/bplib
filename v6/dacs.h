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

#ifndef DACS_H
#define DACS_H

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bundle_types.h"
#include "rb_tree.h"

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int dacs_write(uint8_t *rec, int size, int max_fills_per_dacs, rb_tree_t *tree, uint32_t *flags);
int dacs_read(const uint8_t *rec, int rec_size, int *num_acks, bp_delete_func_t ack, void *ack_parm, uint32_t *flags);

#endif /* DACS_H */
