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

#ifndef BPLIB_MEM_H
#define BPLIB_MEM_H

/*
** Include
*/

// TODO stdint.h, stddef.h, and stdbool.h belong in bplib_api_types.h.
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "bplib.h"

// TODO Return BPlib_MEM_Block_t to an abstract type.
typedef struct BPLib_MEM_Block
{
    /* note that if it becomes necessary to recover bits here,
     * both the type and offset could be reduced in size */
    uint32_t                  parent_offset;
    struct BPLib_MEM_Block_t  *next;
    struct BPLib_MEM_Block_t  *prev;
} BPLib_MEM_Block_t;

//struct BPLib_MEM_Block_abstract // TODO
//{
    /* note that if it becomes necessary to recover bits here,
     * the offset could be reduced in size */
//    uint32_t                  parent_offset;
//    struct BPLib_MEM_Block_t *next;
//    struct BPLib_MEM_Block_t *prev;
//};

/*
** Exported Functions
*/

/**
 * \brief Memory Allocator initialization
 *
 *  \par Description
 *       MEM initialization function
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Initialization was successful
 */
int BPLib_MEM_Init(void);

#endif /* BPLIB_MEM_H */
