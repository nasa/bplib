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

#ifndef BPLIB_STOR_QM_H
#define BPLIB_STOR_QM_H

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdint.h>
#include "bplib_api_types.h"
#include "bplib_stor_cache_types.h"
#include "bplib_stor_cache_block.h"

#include "bplib_stor_qm_types.h"
#include "bplib_stor_qm_codec.h"
#include "bplib_stor_qm_dataservice.h"
#include "bplib_stor_qm_decode.h"
#include "bplib_stor_qm_ducts.h"
#include "bplib_stor_qm_encode.h"
#include "bplib_stor_qm_job.h"
#include "bplib_stor_qm_module_api.h"
#include "bplib_stor_qm_module_api_content.h"
#include "bplib_stor_qm_mpstream.h"

/* JPHFIX: These are duplicate of the flow flags, should be consolidated */
#define BPLIB_INTF_STATE_ADMIN_UP 0x01
#define BPLIB_INTF_STATE_OPER_UP  0x02

/******************************************************************************
 GLOBALS
 ******************************************************************************/

// TODO Define the global Queue Table, formerly the global Route Table.
/**
 * QM must define BPLib_GlobalData.QueueTBL formerly BP_GlobalData.RouteTbl
 * Then client code can have code like:
 * 
 *     // Test queue table with 1MB of cache
 *     BP_GlobalData.QueueTbl = BPLib_STOR_QM_AllocQueueTbl(10, 16 << 20);
 *     if (BP_GlobalData.QueueTbl == NULL)
 *     {
 *         fprintf(stderr, "%s(): BPLib_STOR_QM_AllocQueueTbl\n", __func__);
 *         return CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
 *     }
 */


/******************************************************************************
 DEFINES
 ******************************************************************************/


/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/
/**
 * @brief Callback frunction for various mpool block actions
 *
 * This is a generic API for a function to handle various events/conditions
 * that occur at the block level.  The generic argument supplies the context
 * information.
 *
 * The return value depends on the context and may or may not be used, it
 * should should return 0 unless otherwise specified.
 */
typedef int (*BPLib_STOR_QM_CallbackFunc_t)(void *, BPLib_STOR_CACHE_Block_t *);

BPLib_STOR_QM_QueueTbl_t *BPLib_STOR_QM_AllocTable(uint32_t max_routes, size_t cache_mem_size);
BPLib_STOR_CACHE_Pool_t *BPLib_STOR_QM_GetQtblPool(const BPLib_STOR_QM_QueueTbl_t *tbl);

BPLib_Handle_t BPLib_STOR_QM_RegisterGenericIntf(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Handle_t parent_intf_id,
                                              BPLib_STOR_CACHE_Block_t *flow_block);

void BPLib_STOR_QM_IngressRouteSingleBundle(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_STOR_CACHE_Block_t *pblk);
int  BPLib_STOR_QM_IngressBaseintfForwarder(void *arg, BPLib_STOR_CACHE_Block_t *subq_src);
int  BPLib_STOR_QM_IngressToParent(void *arg, BPLib_STOR_CACHE_Block_t *subq_src);

BPLib_Handle_t       BPLib_STOR_QM_BindSubIntf(BPLib_STOR_CACHE_Block_t *flow_block, BPLib_Handle_t parent_intf_id);
BPLib_STOR_CACHE_Ref_t BPLib_STOR_QM_GetIntfControlblock(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Handle_t intf_id);
void              BPLib_STOR_QM_ReleaseIntfControlblock(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_STOR_CACHE_Ref_t refptr);

void BPLib_STOR_QM_IngressQueueSingleBundle(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_STOR_CACHE_Block_t *pblk);

int BPLib_STOR_QM_RegisterForwardIngressHandler(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Handle_t intf_id,
                                                 BPLib_STOR_QM_CallbackFunc_t ingress);
int BPLib_STOR_QM_RegisterForwardEgressHandler(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Handle_t intf_id,
                                                BPLib_STOR_QM_CallbackFunc_t egress);
int BPLib_STOR_QM_RegisterEventHandler(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Handle_t intf_id, BPLib_STOR_QM_CallbackFunc_t event);

int         BPLib_STOR_QM_DelIntf(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Handle_t intf_id);
BPLib_Handle_t BPLib_STOR_QM_GetNextIntfWithFlags(const BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Ipn_t dest, uint32_t req_flags,
                                                 uint32_t flag_mask);
BPLib_Handle_t BPLib_STOR_QM_GetNextAvailIntf(const BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Ipn_t dest);
int         BPLib_STOR_QM_Add(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Ipn_t dest, BPLib_Ipn_t mask, BPLib_Handle_t intf_id);
int         BPLib_STOR_QM_Del(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Ipn_t dest, BPLib_Ipn_t mask, BPLib_Handle_t intf_id);

int BPLib_STOR_QM_IntfSetFlags(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Handle_t intf_id, uint32_t flags);
int BPLib_STOR_QM_IntfUnsetFlags(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Handle_t intf_id, uint32_t flags);

int BPLib_STOR_QM_PushIngressBundle(const BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Handle_t intf_id, BPLib_STOR_CACHE_Block_t *cb);
int BPLib_STOR_QM_PushEgressBundle(const BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Handle_t intf_id, BPLib_STOR_CACHE_Block_t *cb);

int BPLib_STOR_QM_ForwardBaseintfBundle(BPLib_STOR_CACHE_Block_t *flow_block, void *forward_arg);

void BPLib_STOR_QM_SetMaintenanceRequest(BPLib_STOR_QM_QueueTbl_t *tbl);
void BPLib_STOR_QM_MaintenanceRequestWait(BPLib_STOR_QM_QueueTbl_t *tbl);
void BPLib_STOR_QM_MaintenanceCompleteWait(BPLib_STOR_QM_QueueTbl_t *tbl);

void BPLib_STOR_QM_ProcessActiveFlows(BPLib_STOR_QM_QueueTbl_t *tbl);
void BPLib_STOR_QM_PeriodicMaintenance(BPLib_STOR_QM_QueueTbl_t *tbl);

#endif // BPLIB_STOR_QM_H
