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

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in bplib_stor_qm header
 */

#include "bplib_api_types.h"
#include "bplib_stor_cache_types.h"
#include "bplib_stor_cache_block.h"
#include "bplib_stor_qm.h"

#include "utgenstub.h"

BPLib_STOR_CACHE_Block_t retblk;  // For Test_BPLib_STOR_QM_InstantiateStub.

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_Add()
 * ----------------------------------------------------
 */
int BPLib_STOR_QM_Add(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Ipn_t dest, BPLib_Ipn_t mask, BPLib_Handle_t intf_id)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_Add, int);

    UT_GenStub_AddParam(BPLib_STOR_QM_Add, BPLib_STOR_QM_QueueTbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_QM_Add, BPLib_Ipn_t, dest);
    UT_GenStub_AddParam(BPLib_STOR_QM_Add, BPLib_Ipn_t, mask);
    UT_GenStub_AddParam(BPLib_STOR_QM_Add, BPLib_Handle_t, intf_id);

    UT_GenStub_Execute(BPLib_STOR_QM_Add, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_Add, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_AllocTable()
 * ----------------------------------------------------
 */
BPLib_STOR_QM_QueueTbl_t *BPLib_STOR_QM_AllocTable(uint32_t max_routes, size_t cache_mem_size)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_AllocTable, BPLib_STOR_QM_QueueTbl_t *);

    UT_GenStub_AddParam(BPLib_STOR_QM_AllocTable, uint32_t, max_routes);
    UT_GenStub_AddParam(BPLib_STOR_QM_AllocTable, size_t, cache_mem_size);

    UT_GenStub_Execute(BPLib_STOR_QM_AllocTable, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_AllocTable, BPLib_STOR_QM_QueueTbl_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_BindSubIntf()
 * ----------------------------------------------------
 */
BPLib_Handle_t BPLib_STOR_QM_BindSubIntf(BPLib_STOR_CACHE_Block_t *duct_block, BPLib_Handle_t parent_intf_id)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_BindSubIntf, BPLib_Handle_t);

    UT_GenStub_AddParam(BPLib_STOR_QM_BindSubIntf, BPLib_STOR_CACHE_Block_t *, duct_block);
    UT_GenStub_AddParam(BPLib_STOR_QM_BindSubIntf, BPLib_Handle_t, parent_intf_id);

    UT_GenStub_Execute(BPLib_STOR_QM_BindSubIntf, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_BindSubIntf, BPLib_Handle_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_Del()
 * ----------------------------------------------------
 */
int BPLib_STOR_QM_Del(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Ipn_t dest, BPLib_Ipn_t mask, BPLib_Handle_t intf_id)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_Del, int);

    UT_GenStub_AddParam(BPLib_STOR_QM_Del, BPLib_STOR_QM_QueueTbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_QM_Del, BPLib_Ipn_t, dest);
    UT_GenStub_AddParam(BPLib_STOR_QM_Del, BPLib_Ipn_t, mask);
    UT_GenStub_AddParam(BPLib_STOR_QM_Del, BPLib_Handle_t, intf_id);

    UT_GenStub_Execute(BPLib_STOR_QM_Del, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_Del, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_DelIntf()
 * ----------------------------------------------------
 */
int BPLib_STOR_QM_DelIntf(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Handle_t intf_id)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_DelIntf, int);

    UT_GenStub_AddParam(BPLib_STOR_QM_DelIntf, BPLib_STOR_QM_QueueTbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_QM_DelIntf, BPLib_Handle_t, intf_id);

    UT_GenStub_Execute(BPLib_STOR_QM_DelIntf, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_DelIntf, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_ForwardBaseintfBundle()
 * ----------------------------------------------------
 */
int BPLib_STOR_QM_ForwardBaseintfBundle(BPLib_STOR_CACHE_Block_t *duct_block, void *forward_arg)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_ForwardBaseintfBundle, int);

    UT_GenStub_AddParam(BPLib_STOR_QM_ForwardBaseintfBundle, BPLib_STOR_CACHE_Block_t *, duct_block);
    UT_GenStub_AddParam(BPLib_STOR_QM_ForwardBaseintfBundle, void *, forward_arg);

    UT_GenStub_Execute(BPLib_STOR_QM_ForwardBaseintfBundle, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_ForwardBaseintfBundle, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_GetIntfControlblock()
 * ----------------------------------------------------
 */
BPLib_STOR_CACHE_Ref_t BPLib_STOR_QM_GetIntfControlblock(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Handle_t intf_id)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_GetIntfControlblock, BPLib_STOR_CACHE_Ref_t);

    UT_GenStub_AddParam(BPLib_STOR_QM_GetIntfControlblock, BPLib_STOR_QM_QueueTbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_QM_GetIntfControlblock, BPLib_Handle_t, intf_id);

    UT_GenStub_Execute(BPLib_STOR_QM_GetIntfControlblock, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_GetIntfControlblock, BPLib_STOR_CACHE_Ref_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_GetNextAvailIntf()
 * ----------------------------------------------------
 */
BPLib_Handle_t BPLib_STOR_QM_GetNextAvailIntf(const BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Ipn_t dest)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_GetNextAvailIntf, BPLib_Handle_t);

    UT_GenStub_AddParam(BPLib_STOR_QM_GetNextAvailIntf, const BPLib_STOR_QM_QueueTbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_QM_GetNextAvailIntf, BPLib_Ipn_t, dest);

    UT_GenStub_Execute(BPLib_STOR_QM_GetNextAvailIntf, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_GetNextAvailIntf, BPLib_Handle_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_GetNextIntfWithFlags()
 * ----------------------------------------------------
 */
BPLib_Handle_t BPLib_STOR_QM_GetNextIntfWithFlags(const BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Ipn_t dest, uint32_t req_flags,
                                               uint32_t flag_mask)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_GetNextIntfWithFlags, BPLib_Handle_t);

    UT_GenStub_AddParam(BPLib_STOR_QM_GetNextIntfWithFlags, const BPLib_STOR_QM_QueueTbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_QM_GetNextIntfWithFlags, BPLib_Ipn_t, dest);
    UT_GenStub_AddParam(BPLib_STOR_QM_GetNextIntfWithFlags, uint32_t, req_flags);
    UT_GenStub_AddParam(BPLib_STOR_QM_GetNextIntfWithFlags, uint32_t, flag_mask);

    UT_GenStub_Execute(BPLib_STOR_QM_GetNextIntfWithFlags, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_GetNextIntfWithFlags, BPLib_Handle_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_GetQtblPool()
 * ----------------------------------------------------
 */
BPLib_STOR_CACHE_Pool_t *BPLib_STOR_QM_GetQtblPool(const BPLib_STOR_QM_QueueTbl_t *tbl)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_GetQtblPool, BPLib_STOR_CACHE_Pool_t *);

    UT_GenStub_AddParam(BPLib_STOR_QM_GetQtblPool, const BPLib_STOR_QM_QueueTbl_t *, tbl);

    UT_GenStub_Execute(BPLib_STOR_QM_GetQtblPool, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_GetQtblPool, BPLib_STOR_CACHE_Pool_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_IngressBaseintfForwarder()
 * ----------------------------------------------------
 */
int BPLib_STOR_QM_IngressBaseintfForwarder(void *arg, BPLib_STOR_CACHE_Block_t *subq_src)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_IngressBaseintfForwarder, int);

    UT_GenStub_AddParam(BPLib_STOR_QM_IngressBaseintfForwarder, void *, arg);
    UT_GenStub_AddParam(BPLib_STOR_QM_IngressBaseintfForwarder, BPLib_STOR_CACHE_Block_t *, subq_src);

    UT_GenStub_Execute(BPLib_STOR_QM_IngressBaseintfForwarder, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_IngressBaseintfForwarder, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_IngressQueueSingleBundle()
 * ----------------------------------------------------
 */
void BPLib_STOR_QM_IngressQueueSingleBundle(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_STOR_CACHE_Block_t *pblk)
{
    UT_GenStub_AddParam(BPLib_STOR_QM_IngressQueueSingleBundle, BPLib_STOR_QM_QueueTbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_QM_IngressQueueSingleBundle, BPLib_STOR_CACHE_Block_t *, pblk);

    UT_GenStub_Execute(BPLib_STOR_QM_IngressQueueSingleBundle, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_IngressRouteSingleBundle()
 * ----------------------------------------------------
 */
void BPLib_STOR_QM_IngressRouteSingleBundle(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_STOR_CACHE_Block_t *pblk)
{
    UT_GenStub_AddParam(BPLib_STOR_QM_IngressRouteSingleBundle, BPLib_STOR_QM_QueueTbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_QM_IngressRouteSingleBundle, BPLib_STOR_CACHE_Block_t *, pblk);

    UT_GenStub_Execute(BPLib_STOR_QM_IngressRouteSingleBundle, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_IngressToParent()
 * ----------------------------------------------------
 */
int BPLib_STOR_QM_IngressToParent(void *arg, BPLib_STOR_CACHE_Block_t *subq_src)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_IngressToParent, int);

    UT_GenStub_AddParam(BPLib_STOR_QM_IngressToParent, void *, arg);
    UT_GenStub_AddParam(BPLib_STOR_QM_IngressToParent, BPLib_STOR_CACHE_Block_t *, subq_src);

    UT_GenStub_Execute(BPLib_STOR_QM_IngressToParent, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_IngressToParent, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_IntfSetFlags()
 * ----------------------------------------------------
 */
int BPLib_STOR_QM_IntfSetFlags(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Handle_t intf_id, uint32_t flags)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_IntfSetFlags, int);

    UT_GenStub_AddParam(BPLib_STOR_QM_IntfSetFlags, BPLib_STOR_QM_QueueTbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_QM_IntfSetFlags, BPLib_Handle_t, intf_id);
    UT_GenStub_AddParam(BPLib_STOR_QM_IntfSetFlags, uint32_t, flags);

    UT_GenStub_Execute(BPLib_STOR_QM_IntfSetFlags, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_IntfSetFlags, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_IntfUnsetFlags()
 * ----------------------------------------------------
 */
int BPLib_STOR_QM_IntfUnsetFlags(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Handle_t intf_id, uint32_t flags)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_IntfUnsetFlags, int);

    UT_GenStub_AddParam(BPLib_STOR_QM_IntfUnsetFlags, BPLib_STOR_QM_QueueTbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_QM_IntfUnsetFlags, BPLib_Handle_t, intf_id);
    UT_GenStub_AddParam(BPLib_STOR_QM_IntfUnsetFlags, uint32_t, flags);

    UT_GenStub_Execute(BPLib_STOR_QM_IntfUnsetFlags, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_IntfUnsetFlags, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_MaintenanceCompleteWait()
 * ----------------------------------------------------
 */
void BPLib_STOR_QM_MaintenanceCompleteWait(BPLib_STOR_QM_QueueTbl_t *tbl)
{
    UT_GenStub_AddParam(BPLib_STOR_QM_MaintenanceCompleteWait, BPLib_STOR_QM_QueueTbl_t *, tbl);

    UT_GenStub_Execute(BPLib_STOR_QM_MaintenanceCompleteWait, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_MaintenanceRequestWait()
 * ----------------------------------------------------
 */
void BPLib_STOR_QM_MaintenanceRequestWait(BPLib_STOR_QM_QueueTbl_t *tbl)
{
    UT_GenStub_AddParam(BPLib_STOR_QM_MaintenanceRequestWait, BPLib_STOR_QM_QueueTbl_t *, tbl);

    UT_GenStub_Execute(BPLib_STOR_QM_MaintenanceRequestWait, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_PeriodicMaintenance()
 * ----------------------------------------------------
 */
void BPLib_STOR_QM_PeriodicMaintenance(BPLib_STOR_QM_QueueTbl_t *tbl)
{
    UT_GenStub_AddParam(BPLib_STOR_QM_PeriodicMaintenance, BPLib_STOR_QM_QueueTbl_t *, tbl);

    UT_GenStub_Execute(BPLib_STOR_QM_PeriodicMaintenance, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_ProcessActiveFlows()
 * ----------------------------------------------------
 */
void BPLib_STOR_QM_ProcessActiveFlows(BPLib_STOR_QM_QueueTbl_t *tbl)
{
    UT_GenStub_AddParam(BPLib_STOR_QM_ProcessActiveFlows, BPLib_STOR_QM_QueueTbl_t *, tbl);

    UT_GenStub_Execute(BPLib_STOR_QM_ProcessActiveFlows, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_PushEgressBundle()
 * ----------------------------------------------------
 */
int BPLib_STOR_QM_PushEgressBundle(const BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Handle_t intf_id,
                                   BPLib_STOR_CACHE_Block_t *cb)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_PushEgressBundle, int);

    UT_GenStub_AddParam(BPLib_STOR_QM_PushEgressBundle, const BPLib_STOR_QM_QueueTbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_QM_PushEgressBundle, BPLib_Handle_t, intf_id);
    UT_GenStub_AddParam(BPLib_STOR_QM_PushEgressBundle, BPLib_STOR_CACHE_Block_t *, cb);

    UT_GenStub_Execute(BPLib_STOR_QM_PushEgressBundle, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_PushEgressBundle, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_PushIngressBundle()
 * ----------------------------------------------------
 */
int BPLib_STOR_QM_PushIngressBundle(const BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Handle_t intf_id,
                                    BPLib_STOR_CACHE_Block_t *cb)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_PushIngressBundle, int);

    UT_GenStub_AddParam(BPLib_STOR_QM_PushIngressBundle, const BPLib_STOR_QM_QueueTbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_QM_PushIngressBundle, BPLib_Handle_t, intf_id);
    UT_GenStub_AddParam(BPLib_STOR_QM_PushIngressBundle, BPLib_STOR_CACHE_Block_t *, cb);

    UT_GenStub_Execute(BPLib_STOR_QM_PushIngressBundle, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_PushIngressBundle, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_RegisterEventHandler()
 * ----------------------------------------------------
 */
int BPLib_STOR_QM_RegisterEventHandler(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Handle_t intf_id,
                                       BPLib_STOR_QM_CallbackFunc_t event)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_RegisterEventHandler, int);

    UT_GenStub_AddParam(BPLib_STOR_QM_RegisterEventHandler, BPLib_STOR_QM_QueueTbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_QM_RegisterEventHandler, BPLib_Handle_t, intf_id);
    UT_GenStub_AddParam(BPLib_STOR_QM_RegisterEventHandler, BPLib_STOR_QM_CallbackFunc_t, event);

    UT_GenStub_Execute(BPLib_STOR_QM_RegisterEventHandler, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_RegisterEventHandler, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_RegisterForwardEgressHandler()
 * ----------------------------------------------------
 */
int BPLib_STOR_QM_RegisterForwardEgressHandler(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Handle_t intf_id,
                                               BPLib_STOR_QM_CallbackFunc_t egress)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_RegisterForwardEgressHandler, int);

    UT_GenStub_AddParam(BPLib_STOR_QM_RegisterForwardEgressHandler, BPLib_STOR_QM_QueueTbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_QM_RegisterForwardEgressHandler, BPLib_Handle_t, intf_id);
    UT_GenStub_AddParam(BPLib_STOR_QM_RegisterForwardEgressHandler, BPLib_STOR_QM_CallbackFunc_t, egress);

    UT_GenStub_Execute(BPLib_STOR_QM_RegisterForwardEgressHandler, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_RegisterForwardEgressHandler, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_RegisterForwardIngressHandler()
 * ----------------------------------------------------
 */
int BPLib_STOR_QM_RegisterForwardIngressHandler(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Handle_t intf_id,
                                                BPLib_STOR_QM_CallbackFunc_t ingress)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_RegisterForwardIngressHandler, int);

    UT_GenStub_AddParam(BPLib_STOR_QM_RegisterForwardIngressHandler, BPLib_STOR_QM_QueueTbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_QM_RegisterForwardIngressHandler, BPLib_Handle_t, intf_id);
    UT_GenStub_AddParam(BPLib_STOR_QM_RegisterForwardIngressHandler, BPLib_STOR_QM_CallbackFunc_t, ingress);

    UT_GenStub_Execute(BPLib_STOR_QM_RegisterForwardIngressHandler, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_RegisterForwardIngressHandler, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_RegisterGenericIntf()
 * ----------------------------------------------------
 */
BPLib_Handle_t BPLib_STOR_QM_RegisterGenericIntf(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Handle_t parent_intf_id,
                                              BPLib_STOR_CACHE_Block_t *duct_block)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_RegisterGenericIntf, BPLib_Handle_t);

    UT_GenStub_AddParam(BPLib_STOR_QM_RegisterGenericIntf, BPLib_STOR_QM_QueueTbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_QM_RegisterGenericIntf, BPLib_Handle_t, parent_intf_id);
    UT_GenStub_AddParam(BPLib_STOR_QM_RegisterGenericIntf, BPLib_STOR_CACHE_Block_t *, duct_block);

    UT_GenStub_Execute(BPLib_STOR_QM_RegisterGenericIntf, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_RegisterGenericIntf, BPLib_Handle_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_ReleaseIntfControlblock()
 * ----------------------------------------------------
 */
void BPLib_STOR_QM_ReleaseIntfControlblock(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_STOR_CACHE_Ref_t refptr)
{
    UT_GenStub_AddParam(BPLib_STOR_QM_ReleaseIntfControlblock, BPLib_STOR_QM_QueueTbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_QM_ReleaseIntfControlblock, BPLib_STOR_CACHE_Ref_t, refptr);

    UT_GenStub_Execute(BPLib_STOR_QM_ReleaseIntfControlblock, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_SetMaintenanceRequest()
 * ----------------------------------------------------
 */
void BPLib_STOR_QM_SetMaintenanceRequest(BPLib_STOR_QM_QueueTbl_t *tbl)
{
    UT_GenStub_AddParam(BPLib_STOR_QM_SetMaintenanceRequest, BPLib_STOR_QM_QueueTbl_t *, tbl);

    UT_GenStub_Execute(BPLib_STOR_QM_SetMaintenanceRequest, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_DuctInit()  // TODO Manually added.
 * ----------------------------------------------------
 */
void BPLib_STOR_QM_DuctInit(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_QM_Duct_t *fblk)
{
    UT_GenStub_AddParam(BPLib_STOR_QM_DuctInit, BPLib_STOR_CACHE_Block_t *, base_block);
    UT_GenStub_AddParam(BPLib_STOR_QM_DuctInit, BPLib_STOR_QM_Duct_t *, fblk);

    UT_GenStub_Execute(BPLib_STOR_QM_DuctInit, Basic, NULL);
}

BPLib_STOR_CACHE_Block_t *Test_BPLib_STOR_QM_InstantiateStub(BPLib_STOR_CACHE_Ref_t parent_ref, void *init_arg)
{
    memset(&retblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    return &retblk;
}

int Test_BPLib_STOR_QM_ConfigureStub(BPLib_STOR_CACHE_Pool_t *svc, BPLib_Handle_t cache_intf_id, int key,
                                        BPLib_STOR_QM_ModuleValtype_t vt, const void *val)
{
    return UT_DEFAULT_IMPL(Test_BPLib_STOR_QM_ConfigureStub);
}

int Test_BPLib_STOR_CACHE_QueryStub(BPLib_STOR_CACHE_Pool_t *svc, BPLib_Handle_t module_intf_id, int key,
                                    BPLib_STOR_QM_ModuleValtype_t vt, const void **val)
{
    return UT_DEFAULT_IMPL(Test_BPLib_STOR_CACHE_QueryStub);
}

int Test_BPLib_STOR_CACHE_StartstopStub(BPLib_STOR_CACHE_Block_t *svc, BPLib_Handle_t module_intf_id)
{
    return UT_DEFAULT_IMPL(Test_BPLib_STOR_CACHE_StartstopStub);
}

int Test_BPLib_STOR_CACHE_OffloadStub(BPLib_STOR_CACHE_Block_t *svc, BPLib_STOR_CACHE_Sid_t *sid, BPLib_STOR_CACHE_Block_t *pblk)
{
    return UT_DEFAULT_IMPL(Test_BPLib_STOR_CACHE_OffloadStub);
}

int Test_BPLib_STOR_CACHE_RestoreStub(BPLib_STOR_CACHE_Block_t *svc, BPLib_STOR_CACHE_Sid_t sid, BPLib_STOR_CACHE_Block_t **pblk)
{
    return UT_DEFAULT_IMPL(Test_BPLib_STOR_CACHE_RestoreStub);
}

int Test_BPLib_STOR_CACHE_ReleaseStub(BPLib_STOR_CACHE_Block_t *svc, BPLib_STOR_CACHE_Sid_t sid)
{
    return UT_DEFAULT_IMPL(Test_BPLib_STOR_CACHE_ReleaseStub);
}
