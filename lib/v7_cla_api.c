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
#include "v7.h"
#include "v7_mpool.h"
#include "v7_mpool_bblocks.h"
#include "v7_mpool_flows.h"
#include "v7_mpool_ref.h"
#include "v7_codec.h"
#include "v7_cache.h"
#include "bplib_routing.h"

#define BPLIB_BLOCKTYPE_CLA_INTF          0x7b643c85
#define BPLIB_BLOCKTYPE_CLA_INGRESS_BLOCK 0x9580be4a

typedef struct bplib_cla_stats
{
    uintmax_t ingress_byte_count;
    uintmax_t egress_byte_count;

} bplib_cla_stats_t;

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/
int bplib_cla_event_impl(void *arg, bplib_mpool_block_t *intf_block)
{
    bplib_mpool_flow_generic_event_t *event;
    bplib_mpool_flow_t               *flow;

    event = arg;

    /* only care about state change events for now */
    if (event->event_type != bplib_mpool_flow_event_up && event->event_type != bplib_mpool_flow_event_down)
    {
        return BP_SUCCESS;
    }

    /* only care about state change events for the local i/f */
    flow = bplib_mpool_flow_cast(intf_block);
    if (flow == NULL || !bp_handle_equal(event->intf_state.intf_id, bplib_mpool_get_external_id(intf_block)))
    {
        return BP_SUCCESS;
    }

    if (event->event_type == bplib_mpool_flow_event_up)
    {
        /* Allows bundles to be pushed to flow queues */
        bplib_mpool_flow_enable(&flow->ingress, BP_MPOOL_MAX_SUBQ_DEPTH);
        bplib_mpool_flow_enable(&flow->egress, BP_MPOOL_MAX_SUBQ_DEPTH);
    }
    else if (event->event_type == bplib_mpool_flow_event_down)
    {
        /* drop anything already in the egress queue.  Note that
         * ingress is usually empty, as bundles really should not wait there,
         * so that probably has no effect. */
        bplib_mpool_flow_disable(&flow->ingress);
        bplib_mpool_flow_disable(&flow->egress);
    }

    return BP_SUCCESS;
}

int bplib_generic_bundle_ingress(bplib_mpool_ref_t flow_ref, const void *content, size_t size, uint64_t time_limit)
{
    bplib_mpool_flow_t           *flow;
    bplib_mpool_block_t          *pblk;
    bplib_mpool_block_t          *rblk;
    bplib_mpool_ref_t             refptr;
    bplib_mpool_bblock_primary_t *pri_block;
    size_t                        imported_sz;
    int                           status;

    pblk = NULL;
    flow = bplib_mpool_flow_cast(bplib_mpool_dereference(flow_ref));
    if (flow == NULL)
    {
        status = bplog(NULL, BP_FLAG_DIAGNOSTIC, "intf_block invalid\n");
    }
    else
    {
        pblk =
            bplib_mpool_bblock_primary_alloc(bplib_mpool_get_parent_pool_from_link(bplib_mpool_dereference(flow_ref)));
        if (pblk != NULL)
        {
            imported_sz = v7_copy_full_bundle_in(bplib_mpool_bblock_primary_cast(pblk), content, size);
        }
        else
        {
            imported_sz = 0;
        }

        /* convert the bundle to a dynamically-managed ref */
        refptr = bplib_mpool_ref_create(pblk);
        if (refptr != NULL)
        {
            /* after conversion, should not use the original */
            pblk = NULL;
        }

        pri_block = bplib_mpool_bblock_primary_cast(bplib_mpool_dereference(refptr));

        /*
         * normally the size from the CLA and the size computed from CBOR decoding should agree.
         * For now considering it an error if they do not.
         */
        if (pri_block != NULL && imported_sz == size)
        {
            rblk = bplib_mpool_ref_make_block(refptr, BPLIB_BLOCKTYPE_CLA_INGRESS_BLOCK, NULL);
        }
        else
        {
            rblk = NULL;
        }

        if (rblk != NULL)
        {
            pri_block->data.delivery.ingress_intf_id = bplib_mpool_get_external_id(bplib_mpool_dereference(flow_ref));
            pri_block->data.delivery.ingress_time    = bplib_os_get_dtntime_ms();

            if (bplib_mpool_flow_try_push(&flow->ingress, rblk, time_limit))
            {
                status = BP_SUCCESS;
            }
            else
            {
                bplib_mpool_recycle_block(rblk);
                status = BP_TIMEOUT;
            }
        }
        else
        {
            status = bplog(NULL, BP_FLAG_INCOMPLETE, "Bundle did not decode correctly\n");
        }

        if (refptr != NULL)
        {
            /*
             * If successfully sent, the ref will have been duplicated, so the count remains nonzero after this,
             * and the bundle itself continues on its way.  If something failed, the refcount will become zero,
             * and the bundle memory gets freed.
             */
            bplib_mpool_ref_release(refptr);
            refptr = NULL;
        }

        if (pblk != NULL)
        {
            /* This really shouldn't happen... it means the pblk was allocated but wasn't convertible to a ref.
             * something broke, but recycle it anyway */
            bplib_mpool_recycle_block(pblk);
            pblk = NULL;
        }
    }

    return status;
}

int bplib_generic_bundle_egress(bplib_mpool_ref_t flow_ref, void *content, size_t *size, uint64_t time_limit)
{
    bplib_mpool_flow_t           *flow;
    bplib_mpool_bblock_primary_t *cpb;
    bplib_mpool_block_t          *pblk;
    size_t                        export_sz;
    int                           status;

    pblk = NULL;
    flow = bplib_mpool_flow_cast(bplib_mpool_dereference(flow_ref));
    if (flow == NULL)
    {
        status = bplog(NULL, BP_FLAG_DIAGNOSTIC, "intf_block invalid\n");
    }
    else
    {
        /* this removes it from the list */
        /* NOTE: after this point a valid bundle has to be put somewhere (either onto another queue or recycled) */
        pblk = bplib_mpool_flow_try_pull(&flow->egress, time_limit);
        if (pblk == NULL)
        {
            /* queue is empty */
            status = BP_TIMEOUT;
        }
        else
        {
            cpb = bplib_mpool_bblock_primary_cast(pblk);
            if (cpb == NULL)
            {
                /* entry wasn't a bundle? */
                status = BP_ERROR;
            }
            else
            {
                export_sz = v7_compute_full_bundle_size(cpb);

                if (export_sz > *size)
                {
                    /* buffer too small */
                    status = BP_ERROR;
                }
                else
                {

                    *size = v7_copy_full_bundle_out(cpb, content, *size);

                    if (export_sz != *size)
                    {
                        /* something went wrong during copy */
                        status = BP_ERROR;
                    }
                    else
                    {
                        /* indicate that this has been sent out the intf */
                        cpb->data.delivery.egress_intf_id =
                            bplib_mpool_get_external_id(bplib_mpool_dereference(flow_ref));
                        cpb->data.delivery.egress_time = bplib_os_get_dtntime_ms();

                        status = BP_SUCCESS;
                    }
                }
            }

            bplib_mpool_recycle_block(pblk);
        }
    }

    return status;
}

void bplib_cla_init(bplib_mpool_t *pool)
{
    bplib_mpool_register_blocktype(pool, BPLIB_BLOCKTYPE_CLA_INTF, NULL, sizeof(bplib_cla_stats_t));
    bplib_mpool_register_blocktype(pool, BPLIB_BLOCKTYPE_CLA_INGRESS_BLOCK, NULL, 0);
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

bp_handle_t bplib_create_cla_intf(bplib_routetbl_t *rtbl)
{
    bplib_mpool_block_t *sblk;
    bp_handle_t          self_intf_id;
    bplib_mpool_t       *pool;

    pool = bplib_route_get_mpool(rtbl);

    /* register CLA API module */
    bplib_cla_init(pool);

    /* Allocate Blocks */
    sblk = bplib_mpool_flow_alloc(pool, BPLIB_BLOCKTYPE_CLA_INTF, NULL);
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
    else if (sblk != NULL)
    {
        bplib_mpool_recycle_block(sblk);
    }

    return self_intf_id;
}

int bplib_cla_egress(bplib_routetbl_t *rtbl, bp_handle_t intf_id, void *bundle, size_t *size, uint32_t timeout)
{
    bplib_mpool_ref_t  flow_ref;
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

    stats = bplib_mpool_generic_data_cast(bplib_mpool_dereference(flow_ref), BPLIB_BLOCKTYPE_CLA_INTF);
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
}

int bplib_cla_ingress(bplib_routetbl_t *rtbl, bp_handle_t intf_id, const void *bundle, size_t size, uint32_t timeout)
{
    bplib_mpool_ref_t  flow_ref;
    int                status;
    bplib_cla_stats_t *stats;
    uint64_t           ingress_time_limit;

    flow_ref = bplib_route_get_intf_controlblock(rtbl, intf_id);
    if (flow_ref == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Intf ID invalid\n");
        return BP_ERROR;
    }

    stats = bplib_mpool_generic_data_cast(bplib_mpool_dereference(flow_ref), BPLIB_BLOCKTYPE_CLA_INTF);
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
}
