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

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bplib_os.h"
#ifdef STOR
#include "bplib_cache.h"
#endif // STOR
#include "BPLib_STOR_CACHE_Module_base_internal.h"

#define BPLIB_BLOCKTYPE_CLA_INTF          0x7b643c85
#define BPLIB_BLOCKTYPE_CLA_INGRESS_BLOCK 0x9580be4a

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/
int bplib_cla_event_impl(void *arg, BPLib_STOR_CACHE_Block_t *intf_block)
{
#ifdef STOR
    BPLib_STOR_CACHE_FlowGenericEvent_t *event;
    BPLib_STOR_CACHE_Flow_t               *flow;

    event = arg;

    /* only care about state change events for now */
    if (event->event_type != BPLib_STOR_CACHE_FlowEventUp && event->event_type != BPLib_STOR_CACHE_FlowEventDown)
    {
        return BP_SUCCESS;
    }

    /* only care about state change events for the local i/f */
    flow = BPLib_STOR_CACHE_FlowCast(intf_block);
    if (flow == NULL || !bp_handle_equal(event->intf_state.intf_id, BPLib_STOR_CACHE_GetExternalId(intf_block)))
    {
        return BP_SUCCESS;
    }

    if (event->event_type == BPLib_STOR_CACHE_FlowEventUp)
    {
        /* Allows bundles to be pushed to flow queues */
        BPLib_STOR_CACHE_FlowEnable(&flow->ingress, BPLIB_MEM_MAX_SUBQ_DEPTH);
        BPLib_STOR_CACHE_FlowEnable(&flow->egress, BPLIB_MEM_MAX_SUBQ_DEPTH);
    }
    else if (event->event_type == BPLib_STOR_CACHE_FlowEventDown)
    {
        /* drop anything already in the egress queue.  Note that
         * ingress is usually empty, as bundles really should not wait there,
         * so that probably has no effect. */
        BPLib_STOR_CACHE_FlowDisable(&flow->ingress);
        BPLib_STOR_CACHE_FlowDisable(&flow->egress);
    }

#endif // STOR

    return BP_SUCCESS;
}

int bplib_generic_bundle_ingress(BPLib_STOR_CACHE_Ref_t flow_ref, const void *content, size_t size, uint64_t time_limit)
{
#ifdef STOR
    BPLib_STOR_CACHE_Flow_t           *flow;
    BPLib_STOR_CACHE_Block_t          *pblk;
    BPLib_STOR_CACHE_Block_t          *rblk;
    BPLib_STOR_CACHE_Ref_t             refptr;
    BPLib_STOR_CACHE_BblockPrimary_t *pri_block;
    size_t                        imported_sz;
    int                           status;

    pblk = NULL;
    flow = BPLib_STOR_CACHE_FlowCast(BPLib_STOR_CACHE_Dereference(flow_ref));
    if (flow == NULL)
    {
        status = bplog(NULL, BP_FLAG_DIAGNOSTIC, "intf_block invalid\n");
    }
    else
    {
        /*
         * Note - it is not yet known whether this might be a regular data bundle or a DACS.  If under memory pressure,
         * then it is critical to allow DACS in, because that should free more blocks, relieving the pressure.
         * Therefore, the block allocation is done with a high-ish priority here, but if it ends up to be a regular
         * bundle and there isn't a lot of memory available, this might get discarded later.
         */
        pblk =
            BPLib_STOR_CACHE_BblockPrimaryAlloc(BPLib_STOR_CACHE_GetParentPoolFromLink(BPLib_STOR_CACHE_Dereference(flow_ref)),
                                             0, NULL, BPLIB_MEM_ALLOC_PRI_MHI, 0);
        if (pblk != NULL)
        {
            imported_sz = BPLib_STOR_CACHE_CopyFullBundleIn(BPLib_STOR_CACHE_BblockPrimaryCast(pblk), content, size);
        }
        else
        {
            imported_sz = 0;
        }

        /* convert the bundle to a dynamically-managed ref */
        refptr = BPLib_STOR_CACHE_RefCreate(pblk);
        if (refptr != NULL)
        {
            /* after conversion, should not use the original */
            pblk = NULL;
        }

        pri_block = BPLib_STOR_CACHE_BblockPrimaryCast(BPLib_STOR_CACHE_Dereference(refptr));

        /*
         * normally the size from the CLA and the size computed from CBOR decoding should agree.
         * For now considering it an error if they do not.
         */
        if (pri_block != NULL && imported_sz == size)
        {
            rblk = BPLib_STOR_CACHE_RefMakeBlock(refptr, BPLIB_BLOCKTYPE_CLA_INGRESS_BLOCK, NULL);
        }
        else
        {
            rblk = NULL;
        }

        if (rblk != NULL)
        {
            pri_block->data.delivery.ingress_intf_id = BPLib_STOR_CACHE_GetExternalId(BPLib_STOR_CACHE_Dereference(flow_ref));
            pri_block->data.delivery.ingress_time    = bplib_os_get_dtntime_ms();

            if (BPLib_STOR_CACHE_FlowTryPush(&flow->ingress, rblk, time_limit))
            {
                status = BP_SUCCESS;
            }
            else
            {
                BPLib_STOR_CACHE_RecycleBlock(rblk);
                status = BP_TIMEOUT;
            }
        }
        else
        {
            bplog(NULL, BP_FLAG_INCOMPLETE, "Bundle did not decode correctly\n");
            status = BP_ERROR;
        }

        if (refptr != NULL)
        {
            /*
             * If successfully sent, the ref will have been duplicated, so the count remains nonzero after this,
             * and the bundle itself continues on its way.  If something failed, the refcount will become zero,
             * and the bundle memory gets freed.
             */
            BPLib_STOR_CACHE_RefRelease(refptr);
            refptr = NULL;
        }

        if (pblk != NULL)
        {
            /* This really shouldn't happen... it means the pblk was allocated but wasn't convertible to a ref.
             * something broke, but recycle it anyway */
            BPLib_STOR_CACHE_RecycleBlock(pblk);
            pblk = NULL;
        }
    }

    return status;

#endif // STOR
    return BP_SUCCESS;
}

int bplib_generic_bundle_egress(BPLib_STOR_CACHE_Ref_t flow_ref, void *content, size_t *size, uint64_t time_limit)
{
#ifdef STOR
    BPLib_STOR_CACHE_Flow_t           *flow;
    BPLib_STOR_CACHE_BblockPrimary_t *cpb;
    BPLib_STOR_CACHE_Block_t          *pblk;
    size_t                        export_sz;
    int                           status;

    pblk = NULL;
    flow = BPLib_STOR_CACHE_FlowCast(BPLib_STOR_CACHE_Dereference(flow_ref));
    if (flow == NULL)
    {
        status = bplog(NULL, BP_FLAG_DIAGNOSTIC, "intf_block invalid\n");
    }
    else
    {
        /* this removes it from the list */
        /* NOTE: after this point a valid bundle has to be put somewhere (either onto another queue or recycled) */
        pblk = BPLib_STOR_CACHE_FlowTryPull(&flow->egress, time_limit);
        if (pblk == NULL)
        {
            /* queue is empty */
            status = BP_TIMEOUT;
        }
        else
        {
            cpb = BPLib_STOR_CACHE_BblockPrimaryCast(pblk);
            if (cpb == NULL)
            {
                /* entry wasn't a bundle? */
                status = BP_ERROR;
            }
            else
            {
                export_sz = BPLib_STOR_CACHE_ComputeFullBundleSize(cpb);

                if (export_sz > *size)
                {
                    /* buffer too small */
                    status = BP_ERROR;
                }
                else
                {

                    *size = BPLib_STOR_CACHE_CopyFullBundleOut(cpb, content, *size);

                    if (export_sz != *size)
                    {
                        /* something went wrong during copy */
                        status = BP_ERROR;
                    }
                    else
                    {
                        /* indicate that this has been sent out the intf */
                        cpb->data.delivery.egress_intf_id =
                            BPLib_STOR_CACHE_GetExternalId(BPLib_STOR_CACHE_Dereference(flow_ref));
                        cpb->data.delivery.egress_time = bplib_os_get_dtntime_ms();

                        status = BP_SUCCESS;
                    }
                }
            }

            BPLib_STOR_CACHE_RecycleBlock(pblk);
        }
    }

    return status;
#endif // STOR
    return BP_SUCCESS;
}

void bplib_cla_init(BPLib_STOR_CACHE_Pool_t *pool)
{
#ifdef STOR
    BPLib_STOR_CACHE_RegisterBlocktype(pool, BPLIB_BLOCKTYPE_CLA_INTF, NULL, sizeof(bplib_cla_stats_t));
    BPLib_STOR_CACHE_RegisterBlocktype(pool, BPLIB_BLOCKTYPE_CLA_INGRESS_BLOCK, NULL, 0);
#endif // STOR

}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

bp_handle_t bplib_create_cla_intf(bplib_routetbl_t *rtbl)
{
#ifdef STOR
    BPLib_STOR_CACHE_Block_t *sblk;
    bp_handle_t          self_intf_id;
    BPLib_STOR_CACHE_Pool_t       *pool;

    pool = bplib_route_get_mpool(rtbl);

    /* register CLA API module */
    bplib_cla_init(pool);

    /* Allocate Blocks */
    sblk = BPLib_STOR_CACHE_FlowAlloc(pool, BPLIB_BLOCKTYPE_CLA_INTF, NULL);
    if (sblk == NULL)
    {
        bplog(NULL, BP_FLAG_OUT_OF_MEMORY, "Failed to allocate intf block\n");
        return BP_INVALID_HANDLE;
    }

    self_intf_id = bplib_route_register_generic_intf(rtbl, BP_INVALID_HANDLE, sblk);
    if (bp_handle_is_valid(self_intf_id))
    {
        bplib_route_register_forward_ingress_handler(rtbl, self_intf_id, bplib_route_ingress_baseintf_forwarder);
        bplib_route_register_event_handler(rtbl, self_intf_id, bplib_cla_event_impl);
    }
    else
    {
        BPLib_STOR_CACHE_RecycleBlock(sblk);
    }

    return self_intf_id;
#endif // STOR

    bp_handle_t self_intf_id;
    self_intf_id.hdl = 0;
    return self_intf_id;

}

int bplib_cla_egress(bplib_routetbl_t *rtbl, bp_handle_t intf_id, void *bundle, size_t *size, uint32_t timeout)
{
#ifdef STOR
    BPLib_STOR_CACHE_Ref_t  flow_ref;
    int                status;
    bplib_cla_stats_t *stats;
    uint64_t           egress_time_limit;

    /* preemptively trigger the maintenance task to run */
    /* this may help in the event that the queue is currently empty but
     * there is data somewhere else in the pool that is headed here (assuming
     * timeout is nonzero). */
    bplib_route_set_maintenance_request(rtbl);

    if (timeout == 0)
    {
        egress_time_limit = 0;
    }
    else
    {
        egress_time_limit = bplib_os_get_dtntime_ms() + timeout;
    }

    flow_ref = bplib_route_get_intf_controlblock(rtbl, intf_id);
    if (flow_ref == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Intf ID invalid\n");
        return BP_ERROR;
    }

    stats = BPLib_STOR_CACHE_GenericDataCast(BPLib_STOR_CACHE_Dereference(flow_ref), BPLIB_BLOCKTYPE_CLA_INTF);
    if (stats == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Intf ID is not a CLA\n");
        status = BP_ERROR;
    }
    else
    {
        status = bplib_generic_bundle_egress(flow_ref, bundle, size, egress_time_limit);
        if (status == BP_SUCCESS)
        {
            stats->egress_byte_count += *size;
        }
    }

    bplib_route_release_intf_controlblock(rtbl, flow_ref);

    return status;
#endif // STOR

    return BP_SUCCESS;
}

int bplib_cla_ingress(bplib_routetbl_t *rtbl, bp_handle_t intf_id, const void *bundle, size_t size, uint32_t timeout)
{
#ifdef STOR
    BPLib_STOR_CACHE_Ref_t  flow_ref;
    int                status;
    bplib_cla_stats_t *stats;
    uint64_t           ingress_time_limit;

    flow_ref = bplib_route_get_intf_controlblock(rtbl, intf_id);
    if (flow_ref == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Intf ID invalid\n");
        return BP_ERROR;
    }

    stats = BPLib_STOR_CACHE_GenericDataCast(BPLib_STOR_CACHE_Dereference(flow_ref), BPLIB_BLOCKTYPE_CLA_INTF);
    if (stats == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Intf ID is not a CLA\n");
        status = BP_ERROR;
    }
    else
    {
        if (timeout == 0)
        {
            ingress_time_limit = 0;
        }
        else
        {
            ingress_time_limit = bplib_os_get_dtntime_ms() + timeout;
        }

        status = bplib_generic_bundle_ingress(flow_ref, bundle, size, ingress_time_limit);

        if (status == BP_SUCCESS)
        {
            stats->ingress_byte_count += size;
        }
    }

    bplib_route_release_intf_controlblock(rtbl, flow_ref);

    /* trigger the maintenance task to run */
    bplib_route_set_maintenance_request(rtbl);

    return status;
#endif // STOR

    return BP_SUCCESS;
}
