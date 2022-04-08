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
#include "v7_codec.h"
#include "v7_cache.h"
#include "bplib_routing.h"

#define BPLIB_ROUTING_SIGNATURE_CLA_BASE 0x7b643c85

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

int bplib_generic_bundle_ingress(bplib_mpool_t *pool, bplib_mpool_block_t *intf_block, const void *content, size_t size,
                                 uint32_t timeout)
{
    bplib_mpool_flow_t          *flow;
    bplib_mpool_block_t         *pblk;
    bplib_mpool_block_t         *rblk;
    bplib_mpool_refptr_t        *refptr;
    bplib_mpool_primary_block_t *pri_block;
    size_t                       imported_sz;
    int                          status;

    pblk = NULL;
    flow = bplib_mpool_cast_flow(intf_block);
    if (flow == NULL)
    {
        status = bplog(NULL, BP_FLAG_DIAGNOSTIC, "intf_block invalid\n");
    }
    else
    {
        imported_sz = v7_copy_full_bundle_in(pool, &pblk, content, size);

        /* convert the bundle to a dynamically-managed ref */
        refptr = bplib_mpool_make_dynamic_object(pool, pblk);
        if (refptr != NULL)
        {
            /* after conversion, should not use the original */
            pblk = NULL;
        }

        pri_block = bplib_mpool_cast_primary(bplib_mpool_get_reference_target(refptr));

        /*
         * normally the size from the CLA and the size computed from CBOR decoding should agree.
         * For now considering it an error if they do not.
         */
        if (pri_block != NULL && imported_sz == size)
        {
            rblk = bplib_mpool_make_block_ref(pool, refptr, NULL, NULL);
        }
        else
        {
            rblk = NULL;
        }

        if (rblk != NULL)
        {
            pri_block->delivery_data.ingress_intf_id = flow->external_id;
            pri_block->delivery_data.ingress_time    = bplib_os_get_dtntime_ms();

            bplib_mpool_append_subq_bundle(&flow->input, rblk);
            bplib_mpool_mark_flow_active(pool, flow);

            status = BP_SUCCESS;
        }
        else
        {
            status = bplog(NULL, BP_FLAG_INCOMPLETE, "Bundle did not decode correctly\n");
            ++flow->input.stats.drop_count;
        }

        if (refptr != NULL)
        {
            /*
             * If successfully sent, the ref will have been duplicated, so the count remains nonzero after this,
             * and the bundle itself continues on its way.  If something failed, the refcount will become zero,
             * and the bundle memory gets freed.
             */
            bplib_mpool_release_light_reference(pool, refptr);
            refptr = NULL;
        }

        if (pblk != NULL)
        {
            /* This really shouldn't happen... it means the pblk was allocated but wasn't convertible to a ref.
             * something broke, but recycle it anyway */
            bplib_mpool_recycle_block(pool, pblk);
            pblk = NULL;
        }
    }

    return status;
}

int bplib_generic_bundle_egress(bplib_mpool_t *pool, bplib_mpool_block_t *flow_block, void *content, size_t *size,
                                uint32_t timeout)
{
    bplib_mpool_flow_t          *flow;
    bplib_mpool_primary_block_t *cpb;
    bplib_mpool_block_t         *pblk;
    size_t                       export_sz;
    int                          status;

    pblk = NULL;
    flow = bplib_mpool_cast_flow(flow_block);
    if (flow == NULL)
    {
        status = bplog(NULL, BP_FLAG_DIAGNOSTIC, "intf_block invalid\n");
    }
    else
    {
        /* this removes it from the list */
        /* NOTE: after this point a valid bundle has to be put somewhere (either onto another queue or recycled) */
        pblk = bplib_mpool_shift_subq_bundle(&flow->output);
        cpb  = bplib_mpool_cast_primary(pblk);
        if (cpb == NULL)
        {
            /* queue is empty */
            status = -1;
        }
        else
        {
            export_sz = v7_compute_full_bundle_size(pool, cpb);

            if (export_sz > *size)
            {
                /* buffer too small */
                status = -1;
                ++flow->output.stats.error_count;
            }
            else
            {

                *size = v7_copy_full_bundle_out(cpb, content, *size);

                if (export_sz != *size)
                {
                    /* something went wrong during copy */
                    ++flow->output.stats.error_count;
                    status = -1;
                }
                else
                {
                    /* indicate that this has been sent out the intf */
                    cpb->delivery_data.egress_intf_id = flow->external_id;
                    cpb->delivery_data.egress_time    = bplib_os_get_dtntime_ms();

                    status = BP_SUCCESS;
                }
            }
        }

        if (pblk != NULL)
        {
            bplib_mpool_recycle_block(pool, pblk);
        }
    }

    return status;
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

bp_handle_t bplib_create_cla_intf(bplib_routetbl_t *rtbl)
{
    bplib_mpool_block_t  *sblk;
    bplib_mpool_refptr_t *blkref;
    bp_handle_t           self_intf_id;
    bplib_mpool_t        *pool;

    pool = bplib_route_get_mpool(rtbl);

    /* Allocate Blocks */
    sblk = bplib_mpool_alloc_flow(pool, BPLIB_ROUTING_SIGNATURE_CLA_BASE, sizeof(bplib_cla_stats_t));
    if (sblk == NULL)
    {
        bplog(NULL, BP_FLAG_OUT_OF_MEMORY, "Failed to allocate intf block\n");
        return BP_INVALID_HANDLE;
    }

    blkref = bplib_mpool_make_dynamic_object(pool, sblk);
    if (blkref == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Failed to convert sblk to ref\n");
        self_intf_id = BP_INVALID_HANDLE;
    }
    else
    {
        self_intf_id = bplib_route_register_generic_intf(rtbl, BP_INVALID_HANDLE, blkref);
    }

    if (bp_handle_is_valid(self_intf_id))
    {
        bplib_route_register_forward_ingress_handler(rtbl, self_intf_id, bplib_route_ingress_baseintf_forwarder, NULL);
    }

    if (blkref != NULL)
    {
        bplib_mpool_release_light_reference(pool, blkref);
    }
    else if (sblk != NULL)
    {
        bplib_mpool_recycle_block(pool, sblk);
    }

    return self_intf_id;
}

int bplib_cla_egress(bplib_routetbl_t *rtbl, bp_handle_t intf_id, void *bundle, size_t *size, int timeout)
{
    bplib_mpool_refptr_t *flow_ref;
    int                   status;
    bplib_cla_stats_t    *stats;

    flow_ref = bplib_route_get_intf_controlblock(rtbl, intf_id);
    if (flow_ref == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Intf ID invalid\n");
        return BP_ERROR;
    }

    stats = bplib_mpool_cast_generic_data(bplib_mpool_get_reference_target(flow_ref), BPLIB_ROUTING_SIGNATURE_CLA_BASE);
    if (stats == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Intf ID is not a CLA\n");
        status = BP_ERROR;
    }
    else
    {
        status = bplib_generic_bundle_egress(bplib_route_get_mpool(rtbl), bplib_mpool_get_reference_target(flow_ref),
                                             bundle, size, timeout);
        if (status == BP_SUCCESS)
        {
            stats->egress_byte_count += *size;
        }
    }

    bplib_route_release_intf_controlblock(rtbl, flow_ref);

    return status;
}

int bplib_cla_ingress(bplib_routetbl_t *rtbl, bp_handle_t intf_id, const void *bundle, size_t size, int timeout)
{
    bplib_mpool_refptr_t *flow_ref;
    int                   status;
    bplib_cla_stats_t    *stats;

    flow_ref = bplib_route_get_intf_controlblock(rtbl, intf_id);
    if (flow_ref == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Intf ID invalid\n");
        return BP_ERROR;
    }

    stats = bplib_mpool_cast_generic_data(bplib_mpool_get_reference_target(flow_ref), BPLIB_ROUTING_SIGNATURE_CLA_BASE);
    if (stats == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Intf ID is not a CLA\n");
        status = BP_ERROR;
    }
    else
    {
        status = bplib_generic_bundle_ingress(bplib_route_get_mpool(rtbl), bplib_mpool_get_reference_target(flow_ref),
                                              bundle, size, timeout);

        if (status == BP_SUCCESS)
        {
            stats->ingress_byte_count += size;
        }
    }

    bplib_route_release_intf_controlblock(rtbl, flow_ref);

    return status;
}
