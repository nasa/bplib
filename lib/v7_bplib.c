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

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/*
 * Define the external wrapper type (bp_desc).
 * This is an abstract structure externally, so the contents
 * are hidden from external entities.
 */
struct bp_socket
{
    bplib_mpool_block_t fblk;
};

struct bplib_storage
{
    bp_ipn_addr_t storage_ipn;
    const bplib_routetbl_t *parent_tbl;
};

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * create_bundle
 *-------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------
 * delete_bundle
 *-------------------------------------------------------------------------------------*/

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_init - initializes bp library
 *-------------------------------------------------------------------------------------*/
int bplib_init(void)
{
    /* Initialize OS Interface */
    bplib_os_init();

    /* Initialize CRC algorithms */
    bplib_crc_init();

    /* Return Success */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_deinit - deinitializes bp library
 *-------------------------------------------------------------------------------------*/
void bplib_deinit(void)
{
}

int bplib_serviceflow_bundleize_payload(bplib_mpool_block_t *flow_block, const void *content, size_t size, uint32_t timeout)
{
    bplib_mpool_dataservice_t *dataservice;
    bplib_mpool_flow_t *flow;

    bplib_mpool_block_t           *pblk;
    bplib_mpool_block_t           *cblk;
    bplib_mpool_block_t           *rblk;

    bplib_mpool_primary_block_t *cpb;
    bp_primary_block_t *pri;
    bplib_mpool_canonical_block_t *ccb_pay;
    bp_canonical_block_buffer_t   *pay;
    bplib_mpool_t *pool;

    dataservice = bplib_mpool_cast_dataservice(flow_block);
    if (dataservice == NULL)
    {
        return -1;
    }

    flow = bplib_mpool_cast_flow(flow_block);
    if (flow == NULL)
    {
        return -1;
    }


    pool = bplib_route_get_mpool(flow->parent_rtbl);

    /* Allocate Blocks */
    pblk = bplib_mpool_alloc_primary_block(pool);
    cpb = bplib_mpool_cast_primary(pblk);
    if (cpb == NULL)
    {
        return bplog(NULL, BP_FLAG_OUT_OF_MEMORY, "Failed to allocate primary block\n");
    }

    pri = bplib_mpool_get_pri_block_logical(cpb);

    /* Initialize Primary Block */
    pri->version = 7;
    pri->crctype = bp_crctype_CRC32C;

    v7_set_eid(&pri->destinationEID, &dataservice->params.remote_ipn);
    v7_set_eid(&pri->sourceEID, &dataservice->params.local_ipn);
    v7_set_eid(&pri->reportEID, &dataservice->params.report_ipn);

    pri->creationTimeStamp.sequence_num = 1;
    pri->creationTimeStamp.time = v7_get_current_time();

    pri->lifetime                     = dataservice->params.lifetime;
    pri->controlFlags.isAdminRecord   = dataservice->params.is_admin_service;
    pri->controlFlags.mustNotFragment = !dataservice->params.allow_fragmentation;
    pri->crctype = dataservice->params.crctype;

    cpb->delivery_data.ingress_intf_id = flow->self_intf_id;
    cpb->delivery_data.delivery_policy = dataservice->params.local_delivery_policy;
    cpb->delivery_data.local_retx_interval = dataservice->params.local_retx_interval;

    /* Pre-Encode Primary Block */
    if (v7_block_encode_pri(pool, cpb) < 0)
    {
        return bplog(NULL, BP_FLAG_OUT_OF_MEMORY, "Failed to encoding pri block\n");
    }


    /* Update Payload Block */
    cblk    = bplib_mpool_alloc_canonical_block(pool);
    ccb_pay = bplib_mpool_cast_canonical(cblk);
    if (ccb_pay == NULL)
    {
        return bplog(NULL, BP_FLAG_OUT_OF_MEMORY, "Failed to allocate payload block\n");
    }

    pay = bplib_mpool_get_canonical_block_logical(ccb_pay);

    pay->canonical_block.blockNum  = 1;
    pay->canonical_block.blockType = bp_blocktype_payloadBlock;
    pay->canonical_block.crctype   = dataservice->params.crctype;

    /* Encode Payload Block */
    if (v7_block_encode_pay(pool, ccb_pay, content, size) < 0)
    {
        return bplog(NULL, BP_FLAG_OUT_OF_MEMORY, "Failed to allocate encoding block\n");
    }


    bplib_mpool_store_canonical_block(cpb, cblk);

    rblk = bplib_mpool_create_block_reference(pool, pblk, NULL, NULL);
    if (rblk != NULL)
    {
        bplib_mpool_append_subq_bundle(&flow->input, rblk);
        bplib_mpool_mark_flow_active(pool, flow);

        flow->input.stats.byte_count += size;
        ++flow->input.stats.dataunit_count;
    }
    else
    {
        bplib_mpool_recycle_block(pool, pblk);
    }

    return 0;
}

int bplib_serviceflow_unbundleize_payload(bplib_mpool_block_t *flow_block, void *content, size_t *size, uint32_t timeout)
{
    bplib_mpool_dataservice_t *dataservice;
    bplib_mpool_flow_t *flow;
    bplib_mpool_t *pool;

    bplib_mpool_primary_block_t   *pri;
    bplib_mpool_canonical_block_t *ccb_pay;
    bplib_mpool_block_t           *pblk;
    bplib_mpool_block_t           *cblk;
    size_t                         content_size;
    size_t                         content_offset;
    size_t                         temp_size;

    dataservice = bplib_mpool_cast_dataservice(flow_block);
    if (dataservice == NULL)
    {
        return -1;
    }

    flow = bplib_mpool_cast_flow(flow_block);
    if (flow == NULL)
    {
        return -1;
    }

    pool = bplib_route_get_mpool(flow->parent_rtbl);

    pblk = &flow->output.block_list;
    pblk = bplib_mpool_get_next_block(pblk);
    pri  = bplib_mpool_cast_primary(pblk);
    if (pri == NULL)
    {
        /* no bundles available to recv */
        return BP_ERROR;
    }

    cblk           = bplib_mpool_get_canonical_block_list(pri);
    content_size   = 0;
    content_offset = 0;
    while (true)
    {
        cblk    = bplib_mpool_get_next_block(cblk);
        ccb_pay = bplib_mpool_cast_canonical(cblk);
        if (ccb_pay == NULL)
        {
            break;
        }
        if (bplib_mpool_get_canonical_block_logical(ccb_pay)->canonical_block.blockType == bp_blocktype_payloadBlock)
        {
            content_size   = bplib_mpool_get_canonical_block_encoded_content_length(ccb_pay);
            content_offset = bplib_mpool_get_canonical_block_encoded_content_offset(ccb_pay);
            break;
        }
    }

    /*
     * In theory size could be zero (although not clear if that is a violation of BPv7)
     * but offset could never be zero since there is a canonical block header in front of the
     * payload and that is nonzero size.
     */
    if (content_offset == 0 || content_size > *size)
    {
        /* No payload */
        ++flow->output.stats.error_count;
        return BP_ERROR;
    }

    temp_size = bplib_mpool_copy_block_chain(bplib_mpool_get_canonical_block_encoded_chunks(ccb_pay), content,
                                       *size, content_offset, content_size);

    *size   = content_size;

    /* The primary block can also be discarded now */
    bplib_mpool_recycle_block(pool, pblk);

    /* This should always match, some sort of bug if it does not */
    if (temp_size != content_size)
    {
        return bplog(NULL, BP_FLAG_INCOMPLETE, "Failed to copy complete payload\n");
    }

    flow->output.stats.byte_count += content_size;
    ++flow->output.stats.dataunit_count;

    return BP_SUCCESS;
}

bp_socket_t *bplib_create_socket(bplib_routetbl_t *rtbl)
{
    bplib_mpool_t *pool;
    bplib_mpool_block_t *fblk;
    bplib_mpool_flow_t *flow;

    pool = bplib_route_get_mpool(rtbl);

    /* Allocate Flow */
    fblk = bplib_mpool_alloc_block(pool, bplib_mpool_blocktype_dataservice);
    flow = bplib_mpool_cast_flow(fblk);
    if (flow == NULL)
    {
        bplog(NULL, BP_FLAG_OUT_OF_MEMORY, "Cannot open channel: not enough memory\n");
        return NULL;
    }

    flow->parent_rtbl = rtbl;

    return (bp_socket_t *)fblk;
}

int bplib_serviceflow_forward_egress(bplib_mpool_block_t *intf_block, void *egress_arg)
{
    bplib_mpool_baseintf_t  *intf;
    bplib_mpool_dataservice_t  *dataservice;
    bplib_mpool_flow_t *curr_flow;
    bplib_mpool_flow_t *next_flow;
    bplib_mpool_block_t *node;
    bplib_mpool_block_t *pblk;
    bplib_mpool_primary_block_t   *pri;
    bplib_mpool_t *pool;
    bp_ipn_addr_t bundle_src;
    bp_ipn_addr_t bundle_dest;
    int forward_count;

    dataservice = NULL;
    intf = bplib_mpool_cast_baseintf(intf_block);
    if (intf == NULL)
    {
        return bplog(NULL, BP_FLAG_DIAGNOSTIC, "intf_block invalid\n");
    }

    curr_flow = bplib_mpool_cast_flow(intf_block);
    if (curr_flow == NULL)
    {
        return -1;
    }

    pool = bplib_route_get_mpool(curr_flow->parent_rtbl);

    forward_count = 0;
    while(true)
    {
        pblk = bplib_mpool_shift_subq_bundle(&curr_flow->output);
        if (pblk == NULL)
        {
            /* no bundles available to recv */
            break;
        }

        ++forward_count;

        pri  = bplib_mpool_cast_primary(pblk);
        if (pri != NULL)
        {
            v7_get_eid(&bundle_src, &bplib_mpool_get_pri_block_logical(pri)->sourceEID);
            v7_get_eid(&bundle_dest, &bplib_mpool_get_pri_block_logical(pri)->destinationEID);

            /* Find a dataservice that matches this src/dest combo */
            node = &intf->subflow_list;
            while(true)
            {
                node = bplib_mpool_get_next_block(node);
                dataservice = bplib_mpool_cast_dataservice(node);
                if (dataservice != NULL)
                {
                    /* On recv side, bundle source is remote, bundle dest is local */
                    if (dataservice->params.remote_ipn.service_number == bundle_src.service_number &&
                        dataservice->params.local_ipn.service_number == bundle_dest.service_number &&
                        dataservice->params.remote_ipn.node_number == bundle_src.node_number &&
                        dataservice->params.local_ipn.node_number == bundle_dest.node_number)
                    {
                        next_flow = bplib_mpool_cast_flow(node);
                        if (next_flow != NULL)
                        {
                            bplib_mpool_append_subq_bundle(&next_flow->output, pblk);
                            bplib_mpool_mark_flow_active(pool, next_flow);
                            pblk = NULL;
                            break;
                        }
                    }
                }
            }
        }

        if (pblk == NULL)
        {
            /* JPHFIX: TBD if counter needed for success */
        }
        else
        {
            ++curr_flow->output.stats.drop_count;
            bplib_mpool_recycle_block(bplib_route_get_mpool(curr_flow->parent_rtbl), pblk);
        }

    }

    return forward_count;
}

int bplib_flow_forward_simple_ingress(bplib_mpool_block_t *flow_block, void *ingress_arg)
{
    bplib_mpool_block_t *qblk;
    bplib_mpool_flow_t *flow;
    int forward_count;

    /* This implements a simple ingress for sub-interfaces (of any kind) where the ingress
     * of the sub-interface nodes is simply funneled into the parent */
    flow = bplib_mpool_cast_flow(flow_block);
    if (flow == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Failed to cast flow block\n");
        return -1;
    }


    /*
     * JPHFIX: This does not need to loop here, because it does not actually need
     * to look into each bundle at all.  It can just move the entire queue as a group.
     *  (needs an API that allows that to occur)
     * The only concern is that moving as a group may affect stats keeping
     */
    forward_count = 0;
    while(true)
    {
        qblk = bplib_mpool_shift_subq_bundle(&flow->input);
        if (qblk == NULL)
        {
            /* no more bundles */
            break;
        }

        ++forward_count;

        /* is this routable right now? */
        if (bplib_route_push_ingress_bundle(flow->parent_rtbl, flow->parent_intf_id, qblk) == 0)
        {
            /* successfully routed */
            qblk = NULL;
        }
    }

    return forward_count;
}


int bplib_bind_socket(bp_socket_t *desc, const bp_ipn_addr_t *source_ipn)
{
    bp_handle_t self_intf_id;
    bp_handle_t parent_intf_id;
    bplib_mpool_dataservice_t *dataservice;
    bplib_mpool_flow_t *flow;

    dataservice = bplib_mpool_cast_dataservice(&desc->fblk);
    if (dataservice == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): bad descriptor\n", __func__);
        return -1;
    }

    flow = bplib_mpool_cast_flow(&desc->fblk);
    if (flow == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): bad descriptor\n", __func__);
        return -1;
    }

    if (bp_handle_is_valid(flow->self_intf_id))
    {
        /* already bound */
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): already bound\n", __func__);
        return -1;
    }

    /* Find the intf ID, even if it is not currently enabled */
    parent_intf_id = bplib_route_get_next_intf_with_flags(flow->parent_rtbl, source_ipn->node_number, 0, 0);
    if (!bp_handle_is_valid(parent_intf_id))
    {
        return -1;
    }

    self_intf_id = bplib_route_bind_sub_intf(&desc->fblk, parent_intf_id);
    if (!bp_handle_is_valid(self_intf_id))
    {
        return -1;
    }

    /* note, the egress forwarder must be registered on the parent intf, not this intf.
     * Only one such forwarder can be defined, this should return -1 if it was already set to something else */
    if (bplib_route_register_forward_egress_handler(flow->parent_rtbl, parent_intf_id, bplib_serviceflow_forward_egress, NULL) < 0)
    {
        return -1;
    }

    /* There should be no possibility of these registrations failing, as the intf was just created */
    bplib_route_register_dataunit_handlers(flow->parent_rtbl, self_intf_id, bplib_serviceflow_bundleize_payload, bplib_serviceflow_unbundleize_payload);
    bplib_route_register_forward_ingress_handler(flow->parent_rtbl, self_intf_id, bplib_flow_forward_simple_ingress, &dataservice->params);

    flow->parent_intf_id = parent_intf_id;

    dataservice->params.local_ipn = *source_ipn;

    /* following parameters are just hardcoded for now, need an API to set these items */
    dataservice->params.report_ipn = *source_ipn;
    dataservice->params.crctype = bp_crctype_CRC16;
    dataservice->params.local_delivery_policy = bplib_policy_delivery_local_ack;
    dataservice->params.local_retx_interval = 2000;
    dataservice->params.lifetime = 500000;


    return 0;
}

int bplib_connect_socket(bp_socket_t *desc, const bp_ipn_addr_t *destination_ipn)
{
    bp_handle_t intf_id;
    bplib_mpool_dataservice_t *dataservice;
    bplib_mpool_flow_t *flow;

    dataservice = bplib_mpool_cast_dataservice(&desc->fblk);
    if (dataservice == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): bad descriptor\n", __func__);
        return -1;
    }

    flow = bplib_mpool_cast_flow(&desc->fblk);
    if (flow == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): bad descriptor\n", __func__);
        return -1;
    }

    if (!bp_handle_is_valid(flow->parent_intf_id))
    {
        /* not yet bound */
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): dataservice not bound\n", __func__);
        return -1;
    }

    if (dataservice->params.remote_ipn.node_number != 0)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): dataservice already connected\n", __func__);
        return -1;
    }

    if (destination_ipn->node_number == 0)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): invalid destination node number\n", __func__);
        return -1;
    }

    /* check if there is actually a known route to this destination */
    intf_id = bplib_route_get_next_intf_with_flags(flow->parent_rtbl, destination_ipn->node_number, 0, 0);
    if (!bp_handle_is_valid(intf_id))
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): no route to destination node %lu\n", __func__, (unsigned long)destination_ipn->node_number);
        return -1;
    }

    dataservice->params.remote_ipn = *destination_ipn;

    bplib_route_intf_set_flags(flow->parent_rtbl, flow->self_intf_id, BPLIB_INTF_STATE_ADMIN_UP);

    return 0;
}

void bplib_close_socket(bp_socket_t *desc)
{
    bplib_mpool_dataservice_t *dataservice;
    bplib_mpool_flow_t *flow;

    /* sanity check only here - just need to confirm that the cast worked */
    dataservice = bplib_mpool_cast_dataservice(&desc->fblk);
    if (dataservice == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): bad descriptor\n", __func__);
        return;
    }

    /* this is the one that is actually relevant */
    flow = bplib_mpool_cast_flow(&desc->fblk);
    if (flow == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): bad descriptor\n", __func__);
        return;
    }

    bplib_route_intf_unset_flags(flow->parent_rtbl, flow->self_intf_id, BPLIB_INTF_STATE_ADMIN_UP);
    bplib_route_del_intf(flow->parent_rtbl, flow->self_intf_id);
}



bp_handle_t bplib_create_ram_storage(bplib_routetbl_t *rtbl, const bp_ipn_addr_t *storage_addr)
{
    bp_handle_t intf_id;

    intf_id = bplib_route_add_base_intf(rtbl, storage_addr->node_number);

    if (!bp_handle_is_valid(intf_id))
    {
        bplog(NULL, BP_FLAG_OUT_OF_MEMORY, "bplib_route_add_intf failed\n");
        return BP_INVALID_HANDLE;
    }

    /* a ram storage is simply a cache without a backing store */
    if (bplib_store_cache_attach(rtbl, intf_id) < 0)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "bplib_store_cache_attach failed\n");
        bplib_route_del_intf(rtbl, intf_id);
        return BP_INVALID_HANDLE;
    }

    /* mark this as a storage-capable intf */
    bplib_route_intf_set_flags(rtbl, intf_id, BPLIB_INTF_STATE_STORAGE);
    bplib_route_register_forward_ingress_handler(rtbl, intf_id, bplib_route_forward_baseintf_bundle, NULL);

    return intf_id;
}

int bplib_generic_bundle_ingress(bplib_mpool_block_t *intf_block, const void *content, size_t size, uint32_t timeout)
{
    bplib_mpool_flow_t   *flow;
    bplib_mpool_block_t           *pblk;
    bplib_mpool_block_t           *rblk;
    bplib_mpool_primary_block_t *pri_block;
    size_t   imported_sz;
    bplib_mpool_t *pool;
    int status;

    pblk = NULL;
    flow = bplib_mpool_cast_flow(intf_block);
    if (flow == NULL)
    {
        status = bplog(NULL, BP_FLAG_DIAGNOSTIC, "intf_block invalid\n");
    }
    else
    {
        pool = bplib_route_get_mpool(flow->parent_rtbl);
        imported_sz = v7_copy_full_bundle_in(pool, &pblk, content, size);
        pri_block = bplib_mpool_cast_primary(pblk);

        /*
         * normally the size from the CLA and the size computed from CBOR decoding should agree.
         * For now considering it an error if they do not.
         */
        if (pri_block != NULL && imported_sz == size)
        {
            rblk = bplib_mpool_create_block_reference(pool, pblk, NULL, NULL);
        }
        else
        {
            rblk = NULL;
        }

        if (rblk != NULL)
        {
            pri_block->delivery_data.ingress_intf_id = flow->self_intf_id;
            pri_block->delivery_data.ingress_time = bplib_os_get_dtntime_ms();

            bplib_mpool_append_subq_bundle(&flow->input, rblk);
            bplib_mpool_mark_flow_active(pool, flow);

            status = BP_SUCCESS;

            flow->input.stats.byte_count += size;
            ++flow->input.stats.dataunit_count;
        }
        else
        {
            if (pblk)
            {
                bplib_mpool_recycle_block(pool, pblk);
            }

            status = bplog(NULL, BP_FLAG_INCOMPLETE, "Bundle did not decode correctly\n");
            ++flow->input.stats.drop_count;
        }
    }

    return status;
}

int bplib_generic_bundle_egress(bplib_mpool_block_t *flow_block, void *content, size_t *size, uint32_t timeout)
{
    bplib_mpool_flow_t   *flow;
    bplib_mpool_primary_block_t *cpb;
    bplib_mpool_block_t           *pblk;
    size_t   export_sz;
    bplib_mpool_t *pool;
    int status;

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
        pool = bplib_route_get_mpool(flow->parent_rtbl);
        pblk = bplib_mpool_shift_subq_bundle(&flow->output);
        cpb = bplib_mpool_cast_primary(pblk);
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
                    ++flow->output.stats.dataunit_count;
                    flow->output.stats.byte_count += export_sz;

                    /* indicate that this has been sent out the intf */
                    cpb->delivery_data.egress_intf_id = flow->self_intf_id;
                    cpb->delivery_data.egress_time = bplib_os_get_dtntime_ms();

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



bp_handle_t bplib_create_cla_intf(bplib_routetbl_t *rtbl)
{
    bp_handle_t intf_id;

    intf_id = bplib_route_add_base_intf(rtbl, 0);
    if (!bp_handle_is_valid(intf_id))
    {
        bplog(NULL, BP_FLAG_OUT_OF_MEMORY, "bplib_route_add_intf failed\n");
        return BP_INVALID_HANDLE;
    }

    bplib_route_register_dataunit_handlers(rtbl, intf_id, bplib_generic_bundle_ingress, bplib_generic_bundle_egress);
    bplib_route_register_forward_ingress_handler(rtbl, intf_id, bplib_route_forward_baseintf_bundle, NULL);

    return intf_id;
}

bp_handle_t bplib_create_node_intf(bplib_routetbl_t *rtbl, bp_ipn_t node_num)
{
    bp_handle_t intf_id;

    intf_id = bplib_route_add_base_intf(rtbl, node_num);
    if (!bp_handle_is_valid(intf_id))
    {
        bplog(NULL, BP_FLAG_OUT_OF_MEMORY, "bplib_route_add_intf failed\n");
        return BP_INVALID_HANDLE;
    }

    if (bplib_route_add(rtbl, node_num, ~(bp_ipn_t)0, intf_id) < 0)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "bplib_route_add failed\n");
    }

    bplib_route_register_forward_ingress_handler(rtbl, intf_id, bplib_route_forward_baseintf_bundle, NULL);

    return intf_id;
}


int bplib_send(bp_socket_t *desc, const void *payload, size_t size, int timeout, uint32_t *flags)
{
    bplib_mpool_flow_t *flow;

    flow = bplib_mpool_cast_flow(&desc->fblk);
    if (flow == NULL)
    {
        bplog(flags, BP_FLAG_DIAGNOSTIC, "%s(): bad descriptor\n", __func__);
        return -1;
    }

    return bplib_route_ingress_dataunit(flow->parent_rtbl, flow->self_intf_id, payload, size, timeout);
}

int bplib_recv(bp_socket_t *desc, void *payload, size_t *size, int timeout, uint32_t *flags)
{
    bplib_mpool_flow_t *flow;

    flow = bplib_mpool_cast_flow(&desc->fblk);
    if (flow == NULL)
    {
        bplog(flags, BP_FLAG_DIAGNOSTIC, "%s(): bad descriptor\n", __func__);
        return -1;
    }

    return bplib_route_egress_dataunit(flow->parent_rtbl, flow->self_intf_id, payload, size, timeout);
}


int bplib_cla_egress(const bplib_routetbl_t *rtbl, bp_handle_t intf_id, void *bundle, size_t *size, int timeout)
{
    return bplib_route_egress_dataunit(rtbl, intf_id, bundle, size, timeout);
}

int bplib_cla_ingress(const bplib_routetbl_t *rtbl, bp_handle_t intf_id, const void *bundle, size_t size, int timeout)
{
    return bplib_route_ingress_dataunit(rtbl, intf_id, bundle, size, timeout);
}

/*--------------------------------------------------------------------------------------
 * bplib_display -
 *
 *  bundle -                pointer to a bundle (byte array) [INPUT]
 *  size -                  size of bundle being pointed to [INPUT]
 *  flags -                 processing flags [OUTPUT]
 *  Returns:                BP_SUCCESS or error code
 *-------------------------------------------------------------------------------------*/
int bplib_display(const void *bundle, size_t size, uint32_t *flags)
{
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_eid2ipn -
 *
 *  eid -                   null-terminated string representation of End Point ID [INPUT]
 *  len -                   size in bytes of above string [INPUT]
 *  node -                  node number as read from eid [OUTPUT]
 *  service -               service number as read from eid [OUTPUT]
 *  Returns:                BP_SUCCESS or error code
 *-------------------------------------------------------------------------------------*/
int bplib_eid2ipn(const char *eid, size_t len, bp_ipn_t *node, bp_ipn_t *service)
{
    char          eidtmp[BP_MAX_EID_STRING];
    int           tmplen;
    char         *node_ptr;
    char         *service_ptr;
    char         *endptr;
    unsigned long node_result;
    unsigned long service_result;

    /* Sanity Check EID Pointer */
    if (eid == NULL)
    {
        return bplog(NULL, BP_FLAG_API_ERROR, "EID is null\n");
    }

    /* Sanity Check Length of EID */
    if (len < 7)
    {
        return bplog(NULL, BP_FLAG_API_ERROR, "EID must be at least 7 characters, act: %d\n", len);
    }
    else if (len > BP_MAX_EID_STRING)
    {
        return bplog(NULL, BP_FLAG_API_ERROR, "EID cannot exceed %d bytes in length, act: %d\n", BP_MAX_EID_STRING,
                     len);
    }

    /* Check IPN Scheme */
    if (eid[0] != 'i' || eid[1] != 'p' || eid[2] != 'n' || eid[3] != ':')
    {
        return bplog(NULL, BP_FLAG_API_ERROR, "EID (%s) must start with 'ipn:'\n", eid);
    }

    /* Copy EID to Temporary Buffer and Set Pointers */
    tmplen = len - 4;
    memcpy(eidtmp, &eid[4], tmplen);
    eidtmp[tmplen] = '\0';
    node_ptr       = eidtmp;
    service_ptr    = strchr(node_ptr, '.');
    if (service_ptr)
    {
        *service_ptr = '\0';
        service_ptr++;
    }
    else
    {
        return bplog(NULL, BP_FLAG_API_ERROR, "Unable to find dotted notation in EID (%s)\n", eid);
    }

    /* Parse Node Number */
    errno       = 0;
    node_result = strtoul(node_ptr, &endptr, 10); /* assume IPN node and service numbers always written in base 10 */
    if ((endptr == node_ptr) || ((node_result == ULONG_MAX || node_result == 0) && errno == ERANGE))
    {
        return bplog(NULL, BP_FLAG_API_ERROR, "Unable to parse EID (%s) node number\n", eid);
    }

    /* Parse Service Number */
    errno = 0;
    service_result =
        strtoul(service_ptr, &endptr, 10); /* assume IPN node and service numbers always written in base 10 */
    if ((endptr == service_ptr) || ((service_result == ULONG_MAX || service_result == 0) && errno == ERANGE))
    {
        return bplog(NULL, BP_FLAG_API_ERROR, "Unable to parse EID (%s) service number\n", eid);
    }

    /* Set Outputs */
    *node    = (bp_ipn_t)node_result;
    *service = (bp_ipn_t)service_result;
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_ipn2eid -
 *
 *  eid -                   buffer that will hold null-terminated string representation of End Point ID [OUTPUT]
 *  len -                   size in bytes of above buffer [INPUT]
 *  node -                  node number to be written into eid [INPUT]
 *  service -               service number to be written into eid [INPUT]
 *  Returns:                BP_SUCCESS or error code
 *-------------------------------------------------------------------------------------*/
int bplib_ipn2eid(char *eid, size_t len, bp_ipn_t node, bp_ipn_t service)
{
    /* Sanity Check EID Buffer Pointer */
    if (eid == NULL)
    {
        return bplog(NULL, BP_FLAG_API_ERROR, "EID buffer is null\n");
    }

    /* Sanity Check Length of EID Buffer */
    if (len < 7)
    {
        return bplog(NULL, BP_FLAG_API_ERROR, "EID buffer must be at least 7 characters, act: %d\n", len);
    }
    else if (len > BP_MAX_EID_STRING)
    {
        return bplog(NULL, BP_FLAG_API_ERROR, "EID buffer cannot exceed %d bytes in length, act: %d\n",
                     BP_MAX_EID_STRING, len);
    }

    /* Write EID */
    bplib_os_format(eid, len, "ipn:%lu.%lu", (unsigned long)node, (unsigned long)service);

    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_attrinit -         initializes a channel attribute struct with default values
 *
 *  attr -                  pointer to attribute structure that needs to be initialized
 *  Returns:                BP_SUCCESS or error code
 *-------------------------------------------------------------------------------------*/
int bplib_attrinit(bp_attr_t *attributes)
{
    memset(attributes, 0, sizeof(*attributes));
    return BP_SUCCESS;
}
