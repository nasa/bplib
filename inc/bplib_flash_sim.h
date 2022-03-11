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

#ifndef BPLIB_FLASH_SIM_H
#define BPLIB_FLASH_SIM_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_store_flash.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define FLASH_SIM_NUM_BLOCKS      256
#define FLASH_SIM_PAGES_PER_BLOCK 128
#define FLASH_SIM_PAGE_SIZE       4096
#define FLASH_SIM_SPARE_SIZE      128

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int bplib_flash_sim_initialize(void);
int bplib_flash_sim_uninitialize(void);
int bplib_flash_sim_page_read(bp_flash_addr_t addr, void *page_data);
int bplib_flash_sim_page_write(bp_flash_addr_t addr, void *page_data);
int bplib_flash_sim_block_erase(bp_flash_index_t block);
int bplib_flash_sim_block_is_bad(bp_flash_index_t block);
int bplib_flash_sim_physical_block(bp_flash_index_t logblk);
int bplib_flash_sim_block_mark_bad(bp_flash_index_t block);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* BPLIB_FLASH_SIM_H */
