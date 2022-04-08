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
#include "bplib_dataservice.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

#define BPLIB_ROUTING_SIGNATURE_SERVICE_BASE     0x6fd3b69d
#define BPLIB_ROUTING_SIGNATURE_SERVICE_ENDPOINT 0x770c4839
#define BPLIB_ROUTING_SIGNATURE_SERVICE_SOCKET   0xc21bb332

typedef struct bplib_route_serviceintf_info
{
    bp_ipn_t              node_number;
    bplib_rbt_root_t      service_index;
    bplib_mpool_refptr_t *storage_service;

} bplib_route_serviceintf_info_t;

typedef struct bplib_service_endpt bplib_service_endpt_t;

struct bplib_service_endpt
{
    bplib_rbt_link_t      rbt_link; /* for storage in RB tree, must be first */
    bplib_mpool_block_t  *self_ptr;
    bplib_mpool_refptr_t *subflow_ref;
};

typedef struct bplib_socket_info bplib_socket_info_t;

struct bplib_socket_info
{
    bplib_routetbl_t  *parent_rtbl;
    bp_handle_t        socket_intf_id;
    bplib_connection_t params;
    uintmax_t          ingress_byte_count;
    uintmax_t          egress_byte_count;
};

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

bplib_mpool_refptr_t *bplib_serviceflow_bundleize_payload(bplib_socket_info_t *sock_inf, const void *content,
                                                          size_t size)
{
    bplib_mpool_t        *pool;
    bplib_mpool_refptr_t *refptr;

    bplib_mpool_block_t *pblk;
    bplib_mpool_block_t *cblk;

    bplib_mpool_primary_block_t   *pri_block;
    bp_primary_block_t            *pri;
    bplib_mpool_canonical_block_t *ccb_pay;
    bp_canonical_block_buffer_t   *pay;

    /* Allocate Blocks */
    pool   = bplib_route_get_mpool(sock_inf->parent_rtbl);
    cblk   = NULL;
    pblk   = NULL;
    refptr = NULL;

    do
    {
        pblk      = bplib_mpool_alloc_primary_block(pool);
        pri_block = bplib_mpool_cast_primary(pblk);
        if (pri_block == NULL)
        {
            bplog(NULL, BP_FLAG_OUT_OF_MEMORY, "Failed to allocate primary block\n");
            break;
        }

        pri = bplib_mpool_get_pri_block_logical(pri_block);

        /* Initialize Primary Block */
        pri->version = 7;

        v7_set_eid(&pri->destinationEID, &sock_inf->params.remote_ipn);
        v7_set_eid(&pri->sourceEID, &sock_inf->params.local_ipn);
        v7_set_eid(&pri->reportEID, &sock_inf->params.report_ipn);

        pri->creationTimeStamp.sequence_num = 1;
        pri->creationTimeStamp.time         = v7_get_current_time();

        pri->lifetime                     = sock_inf->params.lifetime;
        pri->controlFlags.isAdminRecord   = sock_inf->params.is_admin_service;
        pri->controlFlags.mustNotFragment = !sock_inf->params.allow_fragmentation;
        pri->crctype                      = sock_inf->params.crctype;

        pri_block->delivery_data.delivery_policy     = sock_inf->params.local_delivery_policy;
        pri_block->delivery_data.local_retx_interval = sock_inf->params.local_retx_interval;

        /* Pre-Encode Primary Block */
        if (v7_block_encode_pri(pool, pri_block) < 0)
        {
            bplog(NULL, BP_FLAG_OUT_OF_MEMORY, "Failed encoding pri block\n");
            break;
        }

        /* Update Payload Block */
        cblk    = bplib_mpool_alloc_canonical_block(pool);
        ccb_pay = bplib_mpool_cast_canonical(cblk);
        if (ccb_pay == NULL)
        {
            bplog(NULL, BP_FLAG_OUT_OF_MEMORY, "Failed to allocate payload block\n");
            break;
        }

        pay = bplib_mpool_get_canonical_block_logical(ccb_pay);

        pay->canonical_block.blockNum  = 1;
        pay->canonical_block.blockType = bp_blocktype_payloadBlock;
        pay->canonical_block.crctype   = sock_inf->params.crctype;

        /* Encode Payload Block */
        if (v7_block_encode_pay(pool, ccb_pay, content, size) < 0)
        {
            bplog(NULL, BP_FLAG_OUT_OF_MEMORY, "Failed encoding pay block\n");
            break;
        }

        bplib_mpool_store_canonical_block(pri_block, cblk);
        cblk = NULL; /* do not need now that it is stored */

        /* Final step is to convert to a dynamically-managed ref for returning to caller */
        refptr = bplib_mpool_make_dynamic_object(pool, pblk);
        if (refptr == NULL)
        {
            /* not expected... */
            bplog(NULL, BP_FLAG_DIAGNOSTIC, "Cannot convert payload to ref\n");
            break;
        }

        pblk = NULL; /* do not use original anymore */
    }
    while (false);

    /* clean up, if anything did not work, recycle the blocks now */
    if (cblk != NULL)
    {
        bplib_mpool_recycle_block(pool, cblk);
    }

    if (pblk != NULL)
    {
        bplib_mpool_recycle_block(pool, pblk);
    }

    return refptr;
}

int bplib_serviceflow_unbundleize_payload(bplib_socket_info_t *sock_inf, bplib_mpool_refptr_t *refptr, void *content,
                                          size_t *size)
{
    bplib_mpool_t *pool;

    bplib_mpool_primary_block_t   *pri;
    bplib_mpool_canonical_block_t *ccb_pay;
    bplib_mpool_block_t           *cblk;
    size_t                         content_size;
    size_t                         content_offset;
    size_t                         temp_size;
    int                            status;

    status = BP_ERROR;
    pool   = bplib_route_get_mpool(sock_inf->parent_rtbl);

    do
    {
        pri = bplib_mpool_cast_primary(bplib_mpool_get_reference_target(refptr));
        if (pri == NULL)
        {
            /* not a pri block, cannot be unbundled */
            bplog(NULL, BP_FLAG_INCOMPLETE, "%s(): Not a primary block\n", __func__);
            break;
        }

        /* find the payload */
        cblk = bplib_mpool_get_canonical_block_list(pri);

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
            if (bplib_mpool_get_canonical_block_logical(ccb_pay)->canonical_block.blockType ==
                bp_blocktype_payloadBlock)
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
            bplog(NULL, BP_FLAG_INCOMPLETE, "%s(): Incorrectly sized bundle\n", __func__);
            break;
        }

        temp_size = bplib_mpool_copy_block_chain(bplib_mpool_get_canonical_block_encoded_chunks(ccb_pay), content,
                                                 *size, content_offset, content_size);

        /* This should always match, some sort of bug if it does not */
        if (temp_size != content_size)
        {
            bplog(NULL, BP_FLAG_INCOMPLETE, "%s(): Failed to copy complete payload\n", __func__);
            break;
        }

        /* all done */
        *size  = content_size;
        status = BP_SUCCESS;
    }
    while (false);

    /* Always - The reference can be discarded (caller should have duped it) */
    bplib_mpool_release_light_reference(pool, refptr);

    return status;
}

int bplib_serviceflow_forward_ingress(bplib_routetbl_t *tbl, bplib_mpool_block_t *intf_block, void *forward_arg)
{
    bplib_route_serviceintf_info_t *base_intf;
    bplib_mpool_primary_block_t    *pri_block;
    bplib_mpool_block_t            *qblk;
    bplib_mpool_flow_t             *curr_flow;
    bplib_mpool_flow_t             *storage_flow;
    int                             forward_count;

    base_intf = bplib_mpool_cast_generic_data(intf_block, BPLIB_ROUTING_SIGNATURE_SERVICE_BASE);
    if (base_intf == NULL)
    {
        return bplog(NULL, BP_FLAG_DIAGNOSTIC, "intf_block invalid\n");
    }

    curr_flow = bplib_mpool_cast_flow(intf_block);
    if (curr_flow == NULL)
    {
        return -1;
    }

    forward_count = 0;
    while (true)
    {
        qblk = bplib_mpool_shift_subq_bundle(&curr_flow->input);
        if (qblk == NULL)
        {
            /* no more bundles */
            break;
        }

        /* Increment the counter based off items shifted from the input queue -
         * even if it gets dropped after this (hopefully not) it still counts
         * as something moved/changed by this action */
        ++forward_count;

        /* Check if it needs to be delivered to local storage.  If so, then
         * this function will also put it there. Otherwise, route it normally.
         * If this dataservice is storage-capable, and the bundle has NOT gone
         * to the storage service yet, send it there now */
        pri_block = bplib_mpool_cast_primary(qblk);
        if (pri_block != NULL && base_intf->storage_service != NULL &&
            !bp_handle_is_valid(pri_block->delivery_data.storage_intf_id))
        {
            storage_flow = bplib_mpool_cast_flow(bplib_mpool_get_reference_target(base_intf->storage_service));
            if (storage_flow != NULL)
            {
                bplib_mpool_append_subq_bundle(&storage_flow->output, qblk);
                bplib_mpool_mark_flow_active(bplib_route_get_mpool(tbl), storage_flow);
                qblk = NULL;
            }
        }

        if (qblk != NULL)
        {
            /*
             * This call always puts the block somewhere -
             * if its unroutable, the block will be put into the recycle bin.
             */
            bplib_route_ingress_route_single_bundle(tbl, qblk);
        }
    }

    /* This should return 0 if it did no work and no errors.
     * Should return >0 if some work was done */
    return forward_count;
}

int bplib_serviceflow_forward_egress(bplib_routetbl_t *tbl, bplib_mpool_block_t *intf_block, void *egress_arg)
{
    bplib_route_serviceintf_info_t *base_intf;
    bplib_rbt_link_t               *tgt_subintf;
    bplib_mpool_flow_t             *curr_flow;
    bplib_mpool_flow_t             *next_flow;
    bplib_mpool_refptr_t           *next_flow_ref;
    bplib_mpool_block_t            *pblk;
    bplib_mpool_t                  *pool;
    bplib_mpool_primary_block_t    *pri_block;
    bp_ipn_addr_t                   bundle_src;
    bp_ipn_addr_t                   bundle_dest;
    int                             forward_count;

    base_intf = bplib_mpool_cast_generic_data(intf_block, BPLIB_ROUTING_SIGNATURE_SERVICE_BASE);
    if (base_intf == NULL)
    {
        return bplog(NULL, BP_FLAG_DIAGNOSTIC, "intf_block invalid\n");
    }

    curr_flow = bplib_mpool_cast_flow(intf_block);
    if (curr_flow == NULL)
    {
        return -1;
    }

    pool          = bplib_route_get_mpool(tbl);
    forward_count = 0;
    while (true)
    {
        pblk = bplib_mpool_shift_subq_bundle(&curr_flow->output);
        if (pblk == NULL)
        {
            /* no bundles available to recv */
            break;
        }

        ++forward_count;

        next_flow_ref = NULL;
        pri_block     = bplib_mpool_cast_primary(pblk);
        if (pri_block != NULL)
        {
            /* Egress bundles also need to be checked if they need to go to storage */
            if (!bp_handle_is_valid(pri_block->delivery_data.storage_intf_id) && base_intf->storage_service != NULL)
            {
                /* borrows the ref */
                next_flow_ref = base_intf->storage_service;
            }

            if (next_flow_ref == NULL)
            {
                v7_get_eid(&bundle_src, &bplib_mpool_get_pri_block_logical(pri_block)->sourceEID);
                v7_get_eid(&bundle_dest, &bplib_mpool_get_pri_block_logical(pri_block)->destinationEID);

                /* Find a dataservice that matches this src/dest combo */
                tgt_subintf = bplib_rbt_search(bundle_dest.service_number, &base_intf->service_index);
                if (tgt_subintf != NULL)
                {
                    /* borrows the ref */
                    /* because rbt_link is first element */
                    next_flow_ref = ((bplib_service_endpt_t *)tgt_subintf)->subflow_ref;
                }
            }

            if (next_flow_ref != NULL)
            {
                next_flow = bplib_mpool_cast_flow(bplib_mpool_get_reference_target(next_flow_ref));
                if (next_flow != NULL)
                {
                    /* this delivers to output queue (same direction) as normal */
                    bplib_mpool_append_subq_bundle(&next_flow->output, pblk);
                    bplib_mpool_mark_flow_active(pool, next_flow);
                    pblk = NULL;
                }
            }
        }

        if (pblk != NULL)
        {
            /* undeliverable bundle */
            ++curr_flow->output.stats.drop_count;
            bplib_mpool_recycle_block(pool, pblk);
        }
    }

    return forward_count;
}

/**
 * @brief Append a sub-flow (data service) to the base interface block
 *
 * @param svc_intf
 * @param fblk
 */
int bplib_serviceflow_add_to_base(bplib_mpool_t *pool, bplib_mpool_block_t *base_intf_blk, bp_val_t svc_num,
                                  bplib_dataservice_type_t type, bplib_mpool_refptr_t *endpoint_intf_ref)
{
    bplib_route_serviceintf_info_t *base_intf;
    bplib_service_endpt_t          *endpoint_intf;
    int                             status;

    base_intf = bplib_mpool_cast_generic_data(base_intf_blk, BPLIB_ROUTING_SIGNATURE_SERVICE_BASE);
    if (base_intf != NULL)
    {
        bplib_mpool_block_t *temp_block;
        temp_block = bplib_mpool_alloc_generic_block(pool, BPLIB_ROUTING_SIGNATURE_SERVICE_ENDPOINT,
                                                     sizeof(bplib_service_endpt_t));
        if (temp_block == NULL)
        {
            bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): out of memory when binding socket\n", __func__);
            return BP_ERROR;
        }

        endpoint_intf           = bplib_mpool_cast_generic_data(temp_block, BPLIB_ROUTING_SIGNATURE_SERVICE_ENDPOINT);
        endpoint_intf->self_ptr = temp_block;

        /* This can fail in the event the service number is duplicated */
        status = bplib_rbt_insert_value(svc_num, &base_intf->service_index, &endpoint_intf->rbt_link);
        if (status == BP_SUCCESS)
        {
            /* success */
            endpoint_intf->subflow_ref = bplib_mpool_duplicate_light_reference(endpoint_intf_ref);
            if (type == bplib_dataservice_type_storage)
            {
                if (base_intf->storage_service != NULL)
                {
                    bplib_mpool_release_light_reference(pool, base_intf->storage_service);
                }
                base_intf->storage_service = bplib_mpool_duplicate_light_reference(endpoint_intf_ref);
            }
        }
        else
        {
            bplib_mpool_recycle_block(pool, temp_block);
        }
    }
    else
    {
        status = BP_ERROR;
    }

    return status;
}

/**
 * @brief Append a sub-flow (data service) to the base interface block
 *
 * @param svc_intf
 * @param fblk
 */
bplib_mpool_refptr_t *bplib_serviceflow_remove_from_base(bplib_mpool_t *pool, bplib_mpool_block_t *base_intf_blk,
                                                         bp_val_t svc_num)
{
    bplib_route_serviceintf_info_t *base_intf;
    bplib_rbt_link_t               *rbt_link;
    bplib_service_endpt_t          *endpoint_intf;
    bplib_mpool_refptr_t           *endpoint_intf_ref;
    int                             status;

    endpoint_intf_ref = NULL;
    base_intf         = bplib_mpool_cast_generic_data(base_intf_blk, BPLIB_ROUTING_SIGNATURE_SERVICE_BASE);
    if (base_intf != NULL)
    {
        /* This can fail in the event the service number is duplicated */
        status = bplib_rbt_extract_value(svc_num, &base_intf->service_index, &rbt_link);
        if (status == BP_SUCCESS)
        {
            endpoint_intf     = (bplib_service_endpt_t *)rbt_link; /* because its the first item */
            endpoint_intf_ref = endpoint_intf->subflow_ref;

            if (endpoint_intf_ref == base_intf->storage_service)
            {
                bplib_mpool_release_light_reference(pool, base_intf->storage_service);
                base_intf->storage_service = NULL;
            }

            bplib_mpool_recycle_block(pool, endpoint_intf->self_ptr);
        }
    }
    else
    {
        status = BP_ERROR;
    }

    return endpoint_intf_ref;
}

bp_handle_t bplib_dataservice_add_base_intf(bplib_routetbl_t *rtbl, bp_ipn_t node_number)
{
    bplib_mpool_block_t            *sblk;
    bplib_mpool_refptr_t           *blkref;
    bplib_route_serviceintf_info_t *base_intf;
    bp_handle_t                     self_intf_id;
    bplib_mpool_t                  *pool;

    pool = bplib_route_get_mpool(rtbl);

    /* Allocate Blocks */
    sblk = bplib_mpool_alloc_flow(pool, BPLIB_ROUTING_SIGNATURE_SERVICE_BASE, sizeof(bplib_service_endpt_t));
    if (sblk == NULL)
    {
        bplog(NULL, BP_FLAG_OUT_OF_MEMORY, "Failed to allocate intf block\n");
        return BP_INVALID_HANDLE;
    }

    base_intf = bplib_mpool_cast_generic_data(sblk, BPLIB_ROUTING_SIGNATURE_SERVICE_BASE);
    blkref    = bplib_mpool_make_dynamic_object(pool, sblk);
    if (blkref == NULL || base_intf == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Failed to convert sblk to ref\n");
        self_intf_id = BP_INVALID_HANDLE;
    }
    else
    {
        self_intf_id           = bplib_route_register_generic_intf(rtbl, BP_INVALID_HANDLE, blkref);
        base_intf->node_number = node_number;
    }

    if (bp_handle_is_valid(self_intf_id))
    {
        bplib_route_register_forward_ingress_handler(rtbl, self_intf_id, bplib_serviceflow_forward_ingress, NULL);
        bplib_route_register_forward_egress_handler(rtbl, self_intf_id, bplib_serviceflow_forward_egress, NULL);
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

bp_handle_t bplib_dataservice_attach(bplib_routetbl_t *tbl, const bp_ipn_addr_t *ipn, bplib_dataservice_type_t type,
                                     bplib_mpool_refptr_t *blkref)
{
    bp_handle_t           self_intf_id;
    bp_handle_t           parent_intf_id;
    bplib_mpool_refptr_t *parent_block_ref;
    bplib_mpool_flow_t   *flow;
    bplib_mpool_t        *pool;
    int                   status;

    pool = bplib_route_get_mpool(tbl);
    flow = bplib_mpool_cast_flow(bplib_mpool_get_reference_target(blkref));
    if (flow == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): bad descriptor\n", __func__);
        return BP_INVALID_HANDLE;
    }

    if (bp_handle_is_valid(flow->external_id))
    {
        /* already bound */
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): flow is already bound\n", __func__);
        return BP_INVALID_HANDLE;
    }

    /* Find the intf ID, even if it is not currently enabled */
    parent_intf_id   = bplib_route_get_next_intf_with_flags(tbl, ipn->node_number, 0, 0);
    parent_block_ref = bplib_route_get_intf_controlblock(tbl, parent_intf_id);
    if (parent_block_ref == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): no parent intf for node %lu\n", __func__,
              (unsigned long)ipn->node_number);
        return BP_INVALID_HANDLE;
    }

    status = bplib_serviceflow_add_to_base(pool, bplib_mpool_get_reference_target(parent_block_ref),
                                           ipn->service_number, type, blkref);
    if (status != BP_SUCCESS)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): cannot add service %lu to node %lu - duplicate?\n", __func__,
              (unsigned long)ipn->node_number, (unsigned long)ipn->node_number);
    }
    else
    {
        self_intf_id = bplib_route_register_generic_intf(tbl, parent_intf_id, blkref);
        if (!bp_handle_is_valid(self_intf_id))
        {
            bplib_serviceflow_remove_from_base(pool, bplib_mpool_get_reference_target(parent_block_ref),
                                               ipn->service_number);
            bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): could not register service %lu\n", __func__,
                  (unsigned long)ipn->node_number);
        }
        else if (type == bplib_dataservice_type_storage)
        {
            /* success; mark this as a storage-capable intf */
            bplib_route_intf_set_flags(tbl, self_intf_id, BPLIB_INTF_STATE_STORAGE);
        }
    }

    bplib_route_release_intf_controlblock(tbl, parent_block_ref);

    return self_intf_id;
}

bplib_mpool_refptr_t *bplib_dataservice_detach(bplib_routetbl_t *tbl, const bp_ipn_addr_t *ipn)
{
    bp_handle_t           parent_intf_id;
    bplib_mpool_refptr_t *parent_block_ref;
    bplib_mpool_refptr_t *refptr;
    bplib_mpool_flow_t   *flow;
    bplib_mpool_t        *pool;

    /* Find the intf ID, even if it is not currently enabled */
    pool             = bplib_route_get_mpool(tbl);
    parent_intf_id   = bplib_route_get_next_intf_with_flags(tbl, ipn->node_number, 0, 0);
    parent_block_ref = bplib_route_get_intf_controlblock(tbl, parent_intf_id);
    if (parent_block_ref == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): no parent intf for node %lu\n", __func__,
              (unsigned long)ipn->node_number);
        return NULL;
    }

    refptr = bplib_serviceflow_remove_from_base(pool, bplib_mpool_get_reference_target(parent_block_ref),
                                                ipn->service_number);

    if (refptr != NULL)
    {
        /* also delete its registered intf id (so this mirrors bplib_dataservice_attach) */
        /* this should always work, no recourse if it doesn't */
        flow = bplib_mpool_cast_flow(bplib_mpool_get_reference_target(refptr));
        if (flow != NULL)
        {
            bplib_route_del_intf(tbl, flow->external_id);
        }
    }

    bplib_route_release_intf_controlblock(tbl, parent_block_ref);

    return refptr;
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

bp_socket_t *bplib_create_socket(bplib_routetbl_t *rtbl)
{
    bplib_mpool_t        *pool;
    bplib_mpool_block_t  *sblk;
    bplib_socket_info_t  *sock;
    bplib_mpool_refptr_t *sock_ref;

    pool = bplib_route_get_mpool(rtbl);

    /* Allocate Flow */
    sblk = bplib_mpool_alloc_flow(pool, BPLIB_ROUTING_SIGNATURE_SERVICE_SOCKET, sizeof(bplib_socket_info_t));
    if (sblk != NULL)
    {
        sock = bplib_mpool_cast_generic_data(sblk, BPLIB_ROUTING_SIGNATURE_SERVICE_SOCKET);
        assert(sock != NULL);

        sock->parent_rtbl = rtbl;

        /* following parameters are just hardcoded for now, need an API to set these items */
        sock->params.crctype               = bp_crctype_CRC16;
        sock->params.local_delivery_policy = bplib_policy_delivery_local_ack;
        sock->params.local_retx_interval   = 2000;
        sock->params.lifetime              = 500000;

        sock_ref = bplib_mpool_make_dynamic_object(pool, sblk);
    }
    else
    {
        bplog(NULL, BP_FLAG_OUT_OF_MEMORY, "Cannot open socket: not enough memory\n");
        sock_ref = NULL;
    }

    return (bp_socket_t *)sock_ref;
}

int bplib_bind_socket(bp_socket_t *desc, const bp_ipn_addr_t *source_ipn)
{
    bplib_socket_info_t  *sock;
    bplib_mpool_refptr_t *sock_ref;

    sock_ref = (bplib_mpool_refptr_t *)desc;

    sock = bplib_mpool_cast_generic_data(bplib_mpool_get_reference_target(sock_ref),
                                         BPLIB_ROUTING_SIGNATURE_SERVICE_SOCKET);
    if (sock == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): bad descriptor\n", __func__);
        return BP_ERROR;
    }

    if (sock->params.local_ipn.node_number != 0)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): socket already connected\n", __func__);
        return BP_ERROR;
    }

    if (source_ipn->node_number == 0)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): invalid source node number\n", __func__);
        return BP_ERROR;
    }

    sock->socket_intf_id =
        bplib_dataservice_attach(sock->parent_rtbl, source_ipn, bplib_dataservice_type_application, sock_ref);
    if (!bp_handle_is_valid(sock->socket_intf_id))
    {
        return BP_ERROR;
    }

    /* There should be no possibility of these registrations failing, as the intf was just created */
    bplib_route_register_forward_ingress_handler(sock->parent_rtbl, sock->socket_intf_id, bplib_route_ingress_to_parent,
                                                 NULL);

    sock->params.local_ipn  = *source_ipn;
    sock->params.report_ipn = *source_ipn;

    return BP_SUCCESS;
}

int bplib_connect_socket(bp_socket_t *desc, const bp_ipn_addr_t *destination_ipn)
{
    bplib_socket_info_t  *sock;
    bplib_mpool_refptr_t *sock_ref;

    sock_ref = (bplib_mpool_refptr_t *)desc;

    sock = bplib_mpool_cast_generic_data(bplib_mpool_get_reference_target(sock_ref),
                                         BPLIB_ROUTING_SIGNATURE_SERVICE_SOCKET);
    if (sock == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): bad descriptor\n", __func__);
        return -1;
    }

    if (!bp_handle_is_valid(sock->socket_intf_id))
    {
        /* not yet bound */
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): socket not bound\n", __func__);
        return -1;
    }

    if (sock->params.remote_ipn.node_number != 0)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): socket already connected\n", __func__);
        return -1;
    }

    if (destination_ipn->node_number == 0)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): invalid destination node number\n", __func__);
        return -1;
    }

    sock->params.remote_ipn = *destination_ipn;

    bplib_route_intf_set_flags(sock->parent_rtbl, sock->socket_intf_id, BPLIB_INTF_STATE_ADMIN_UP);

    return 0;
}

void bplib_close_socket(bp_socket_t *desc)
{
    bplib_mpool_t        *pool;
    bplib_socket_info_t  *sock;
    bplib_mpool_refptr_t *sock_ref;
    bplib_mpool_refptr_t *detached_ref;

    sock_ref = (bplib_mpool_refptr_t *)desc;

    sock = bplib_mpool_cast_generic_data(bplib_mpool_get_reference_target(sock_ref),
                                         BPLIB_ROUTING_SIGNATURE_SERVICE_SOCKET);
    if (sock == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): bad descriptor\n", __func__);
        return;
    }

    pool = bplib_route_get_mpool(sock->parent_rtbl);
    if (sock->params.local_ipn.node_number != 0)
    {
        detached_ref = bplib_dataservice_detach(sock->parent_rtbl, &sock->params.local_ipn);
        if (detached_ref != NULL)
        {
            bplib_mpool_release_light_reference(pool, detached_ref);
            detached_ref = NULL;
        }

        memset(&sock->params.local_ipn, 0, sizeof(sock->params.local_ipn));
    }

    /* recycle the original */
    sock->parent_rtbl = NULL;
    bplib_mpool_release_light_reference(pool, sock_ref);
}

int bplib_send(bp_socket_t *desc, const void *payload, size_t size, int timeout, uint32_t *flags)
{
    int                          status;
    bplib_mpool_block_t         *rblk;
    bplib_mpool_flow_t          *flow;
    bplib_mpool_refptr_t        *refptr;
    bplib_mpool_primary_block_t *pri_block;
    bplib_mpool_refptr_t        *sock_ref;
    bplib_socket_info_t         *sock;
    bplib_mpool_t               *pool;

    sock_ref = (bplib_mpool_refptr_t *)desc;

    sock = bplib_mpool_cast_generic_data(bplib_mpool_get_reference_target(sock_ref),
                                         BPLIB_ROUTING_SIGNATURE_SERVICE_SOCKET);
    if (sock == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): bad descriptor\n", __func__);
        return BP_ERROR;
    }

    pool = bplib_route_get_mpool(sock->parent_rtbl);
    flow = bplib_mpool_cast_flow(bplib_mpool_get_reference_target(sock_ref));
    if (flow == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): bad flow ref - is socket connected?\n", __func__);
        return BP_ERROR;
    }

    refptr = bplib_serviceflow_bundleize_payload(sock, payload, size);
    if (refptr == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): cannot bundleize payload, out of memory?\n", __func__);
        return BP_ERROR;
    }

    rblk = bplib_mpool_make_block_ref(pool, refptr, NULL, NULL);
    if (rblk != NULL)
    {
        pri_block = bplib_mpool_cast_primary(rblk);
        if (pri_block != NULL)
        {
            pri_block->delivery_data.ingress_intf_id = flow->external_id;
            pri_block->delivery_data.ingress_time    = bplib_os_get_dtntime_ms();
        }

        bplib_mpool_append_subq_bundle(&flow->input, rblk);
        bplib_mpool_mark_flow_active(pool, flow);

        sock->ingress_byte_count += size;
        status = BP_SUCCESS;
    }
    else
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): failed to create block ref, out of memory?\n", __func__);
        ++flow->input.stats.error_count;
        status = BP_ERROR;
    }

    /*
     * JPHFIX - to implement a timeout, this should wait/confirm here that the bundle either
     * reached a storage (for custody-tracked) or made it to the next hop CLA (for best-effort svc level)
     */

    bplib_mpool_release_light_reference(pool, refptr);

    return status;
}

int bplib_recv(bp_socket_t *desc, void *payload, size_t *size, int timeout, uint32_t *flags)
{
    int                          status;
    bplib_socket_info_t         *sock;
    bplib_mpool_primary_block_t *pri_block;
    bplib_mpool_block_t         *pblk;
    bplib_mpool_refptr_t        *refptr;
    bplib_mpool_refptr_t        *sock_ref;
    bplib_mpool_flow_t          *flow;

    sock_ref = (bplib_mpool_refptr_t *)desc;

    sock = bplib_mpool_cast_generic_data(bplib_mpool_get_reference_target(sock_ref),
                                         BPLIB_ROUTING_SIGNATURE_SERVICE_SOCKET);
    if (sock == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): bad descriptor\n", __func__);
        return BP_ERROR;
    }

    flow = bplib_mpool_cast_flow(bplib_mpool_get_reference_target(sock_ref));
    if (flow == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): bad flow_ref\n", __func__);
        return BP_ERROR;
    }

    if (bplib_mpool_is_link_unattached(&flow->output.block_list))
    {
        /*
         * JPHFIX - to implement a timeout, this should wait/confirm here that the output queue
         * is not empty.  For now just return timeout.
         */
        return BP_TIMEOUT;
    }

    pblk   = bplib_mpool_get_next_block(&flow->output.block_list);
    refptr = bplib_mpool_duplicate_block_reference(pblk);

    if (refptr != NULL)
    {
        /* note, the unbundleize always consumes the refptr */
        status = bplib_serviceflow_unbundleize_payload(sock, refptr, payload, size);
    }
    else
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): unable to create ref from pblk\n", __func__);
        status = BP_ERROR;
    }

    /* if a block was pulled from the queue, that needs to be recycled */
    /* this should always be the case */
    if (pblk != NULL)
    {
        pri_block = bplib_mpool_cast_primary(pblk);
        if (status == BP_SUCCESS && pri_block != NULL)
        {
            pri_block->delivery_data.egress_intf_id = flow->external_id;
            pri_block->delivery_data.egress_time    = bplib_os_get_dtntime_ms();
        }

        bplib_mpool_recycle_block(bplib_route_get_mpool(sock->parent_rtbl), pblk);
    }

    if (status == BP_SUCCESS)
    {
        sock->egress_byte_count += *size;
    }
    else
    {
        ++flow->output.stats.error_count;
    }

    return status;
}
