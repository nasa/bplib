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
#include <stdio.h> // STOR - Temporary printf for temporary bplib_os_log

#include "bplib.h"
#include "bplib_dataservice.h"
#include "BPLib_STOR_CACHE_Module_base_internal.h"

// STOR - From bplib_os.h
#define bplog(flags, evt, msg_str) printf("bplog - %s:%d flags: %d, %d - %s", __FILE__, __LINE__, flags, evt, msg_str)

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

#define BPLIB_BLOCKTYPE_SERVICE_BASE     0x6fd3b69d
#define BPLIB_BLOCKTYPE_SERVICE_ENDPOINT 0x770c4839
#define BPLIB_BLOCKTYPE_SERVICE_SOCKET   0xc21bb332
#define BPLIB_BLOCKTYPE_SERVICE_BLOCK    0xbd35ac62

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

int bplib_serviceflow_bundleize_payload(bplib_socket_info_t *sock_inf, BPLib_STOR_CACHE_Block_t *pblk, const void *content,
                                        size_t size)
{
    #ifdef STOR
    BPLib_STOR_CACHE_Block_t            *cblk;
    BPLib_STOR_CACHE_BblockPrimary_t   *pri_block;
    bp_primary_block_t             *pri;
    BPLib_STOR_CACHE_BblockCanonical_t *ccb_pay;
    bp_canonical_block_buffer_t    *pay;
    int                             result;

    cblk   = NULL;
    result = BP_ERROR;

    do
    {
        pri_block = BPLib_STOR_CACHE_BblockPrimaryCast(pblk);
        if (pri_block == NULL)
        {
            bplog(NULL, BP_FLAG_OUT_OF_MEMORY, "Failed to allocate primary block\n");
            break;
        }

        pri = BPLib_STOR_CACHE_BblockPrimaryGetLogical(pri_block);

        /* Initialize Primary Block */
        pri->version = 7;

        BPLib_STOR_CACHE_DataserviceSetEid(&pri->destinationEID, &sock_inf->params.remote_ipn);
        BPLib_STOR_CACHE_DataserviceSetEid(&pri->sourceEID, &sock_inf->params.local_ipn);
        BPLib_STOR_CACHE_DataserviceSetEid(&pri->reportEID, &sock_inf->params.report_ipn);

        pri->creationTimeStamp.sequence_num = sock_inf->last_bundle_seq;
        ++sock_inf->last_bundle_seq;

        pri->creationTimeStamp.time = BPLib_STOR_CACHE_GetCurrentTime();

        pri->lifetime                     = sock_inf->params.lifetime;
        pri->controlFlags.isAdminRecord   = sock_inf->params.is_admin_service;
        pri->controlFlags.mustNotFragment = !sock_inf->params.allow_fragmentation;
        pri->crctype                      = sock_inf->params.crctype;

        pri_block->data.delivery.delivery_policy     = sock_inf->params.local_delivery_policy;
        pri_block->data.delivery.local_retx_interval = sock_inf->params.local_retx_interval;

        /* Pre-Encode Primary Block */
        if (BPLib_STOR_CACHE_BlockEncodePri(pri_block) < 0)
        {
            bplog(NULL, BP_FLAG_OUT_OF_MEMORY, "Failed encoding pri block\n");
            break;
        }

        /* Update Payload Block */
        cblk    = BPLib_STOR_CACHE_BblockCanonicalAlloc(bplib_route_get_mpool(sock_inf->parent_rtbl), 0, NULL);
        ccb_pay = BPLib_STOR_CACHE_BblockCanonicalCast(cblk);
        if (ccb_pay == NULL)
        {
            bplog(NULL, BP_FLAG_OUT_OF_MEMORY, "Failed to allocate payload block\n");
            break;
        }

        pay = BPLib_STOR_CACHE_BblockCanonicalGetLogical(ccb_pay);

        pay->canonical_block.blockNum  = 1;
        pay->canonical_block.blockType = bp_blocktype_payloadBlock;
        pay->canonical_block.crctype   = sock_inf->params.crctype;

        /* Encode Payload Block */
        if (BPLib_STOR_CACHE_BlockEncodePay(ccb_pay, content, size) < 0)
        {
            bplog(NULL, BP_FLAG_OUT_OF_MEMORY, "Failed encoding pay block\n");
            break;
        }

        BPLib_STOR_CACHE_BblockPrimaryAppend(pri_block, cblk);
        cblk   = NULL; /* do not need now that it is stored */
        result = BP_SUCCESS;
    } while (false);

    /* clean up, if anything did not work, recycle the blocks now */
    if (cblk != NULL)
    {
        BPLib_STOR_CACHE_RecycleBlock(cblk);
    }

    return result;
    #else // STOR
    return 0;
    #endif
}

int bplib_serviceflow_unbundleize_payload(bplib_socket_info_t *sock_inf, BPLib_STOR_CACHE_Ref_t refptr, void *content,
                                          size_t *size)
{
    #ifdef STOR

    BPLib_STOR_CACHE_BblockPrimary_t   *pri;
    BPLib_STOR_CACHE_BblockCanonical_t *ccb_pay;
    BPLib_STOR_CACHE_Block_t            *cblk;
    size_t                          content_size;
    size_t                          content_offset;
    size_t                          temp_size;
    int                             status;

    status = BP_ERROR;

    do
    {
        pri = BPLib_STOR_CACHE_BblockPrimaryCast(BPLib_STOR_CACHE_Dereference(refptr));
        if (pri == NULL)
        {
            /* not a pri block, cannot be unbundled */
            bplog(NULL, BP_FLAG_INCOMPLETE, "%s(): Not a primary block\n", __func__);
            break;
        }

        /* find the payload */
        cblk    = BPLib_STOR_CACHE_BblockPrimaryLocateCanonical(pri, bp_blocktype_payloadBlock);
        ccb_pay = BPLib_STOR_CACHE_BblockCanonicalCast(cblk);
        if (ccb_pay == NULL)
        {
            /* no payload, cannot be unbundled */
            bplog(NULL, BP_FLAG_INCOMPLETE, "%s(): No payload\n", __func__);
            break;
        }

        content_size   = BPLib_STOR_CACHE_BblockCanonicalGetContentLength(ccb_pay);
        content_offset = BPLib_STOR_CACHE_BblockCanonicalGetContentOffset(ccb_pay);

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

        temp_size = BPLib_STOR_CACHE_BblockCborExport(BPLib_STOR_CACHE_BblockCanonicalGetEncodedChunks(ccb_pay), content,
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
    } while (false);

    /* Always - The reference can be discarded (caller should have duped it) */
    BPLib_STOR_CACHE_RefRelease(refptr);

    return status;
    #else // STOR
    return BP_SUCCESS;
    #endif // STOR
}

int bplib_serviceflow_forward_ingress(void *arg, BPLib_STOR_CACHE_Block_t *subq_src)
{
    #ifdef STOR
    bplib_route_serviceintf_info_t *base_intf;
    BPLib_STOR_CACHE_BblockPrimary_t   *pri_block;
    BPLib_STOR_CACHE_Block_t            *qblk;
    BPLib_STOR_CACHE_Block_t            *intf_block;
    BPLib_STOR_CACHE_Flow_t             *curr_flow;
    BPLib_STOR_CACHE_Flow_t             *storage_flow;
    int                             forward_count;

    intf_block = BPLib_STOR_CACHE_GetBlockFromLink(subq_src);
    base_intf  = BPLib_STOR_CACHE_GenericDataCast(intf_block, BPLIB_BLOCKTYPE_SERVICE_BASE);
    if (base_intf == NULL)
    {
        return bplog(NULL, BP_FLAG_DIAGNOSTIC, "intf_block invalid\n");
    }

    curr_flow = BPLib_STOR_CACHE_FlowCast(intf_block);
    if (curr_flow == NULL)
    {
        return -1;
    }

    forward_count = 0;
    while (true)
    {
        qblk = BPLib_STOR_CACHE_FlowTryPull(&curr_flow->ingress, 0);
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
        pri_block = BPLib_STOR_CACHE_BblockPrimaryCast(qblk);
        if (pri_block != NULL && base_intf->storage_service != NULL &&
            !bp_handle_is_valid(pri_block->data.delivery.storage_intf_id))
        {
            storage_flow = BPLib_STOR_CACHE_FlowCast(BPLib_STOR_CACHE_Dereference(base_intf->storage_service));
            if (storage_flow != NULL && BPLib_STOR_CACHE_FlowTryPush(&storage_flow->egress, qblk, 0))
            {
                qblk = NULL;
            }
        }

        if (qblk != NULL)
        {
            /*
             * This call always puts the block somewhere -
             * if its unroutable, the block will be put into the recycle bin.
             */
            bplib_route_ingress_route_single_bundle(arg, qblk);
        }
    }

    /* This should return 0 if it did no work and no errors.
     * Should return >0 if some work was done */
    return forward_count;
    #else // STOR
    return 0;
    #endif // STOR
}

int bplib_serviceflow_forward_egress(void *arg, BPLib_STOR_CACHE_Block_t *subq_src)
{
    #ifdef STOR

    bplib_route_serviceintf_info_t *base_intf;
    BPLib_STOR_CACHE_RBT_Link_t               *tgt_subintf;
    BPLib_STOR_CACHE_Flow_t             *curr_flow;
    BPLib_STOR_CACHE_Flow_t             *next_flow;
    BPLib_STOR_CACHE_Ref_t               next_flow_ref;
    BPLib_STOR_CACHE_Block_t            *pblk;
    BPLib_STOR_CACHE_Block_t            *intf_block;
    BPLib_STOR_CACHE_BblockPrimary_t   *pri_block;
    bp_ipn_addr_t                   bundle_src;
    bp_ipn_addr_t                   bundle_dest;
    int                             forward_count;

    intf_block = BPLib_STOR_CACHE_GetBlockFromLink(subq_src);
    base_intf  = BPLib_STOR_CACHE_GenericDataCast(intf_block, BPLIB_BLOCKTYPE_SERVICE_BASE);
    if (base_intf == NULL)
    {
        return bplog(NULL, BP_FLAG_DIAGNOSTIC, "intf_block invalid\n");
    }

    curr_flow = BPLib_STOR_CACHE_FlowCast(intf_block);
    if (curr_flow == NULL)
    {
        return -1;
    }

    forward_count = 0;
    while (true)
    {
        pblk = BPLib_STOR_CACHE_FlowTryPull(&curr_flow->egress, 0);
        if (pblk == NULL)
        {
            /* no bundles available to recv */
            break;
        }

        ++forward_count;

        next_flow_ref = NULL;
        pri_block     = BPLib_STOR_CACHE_BblockPrimaryCast(pblk);
        if (pri_block != NULL)
        {
            /* Egress bundles also need to be checked if they need to go to storage */
            if (!bp_handle_is_valid(pri_block->data.delivery.storage_intf_id) && base_intf->storage_service != NULL)
            {
                /* borrows the ref */
                next_flow_ref = base_intf->storage_service;
            }

            if (next_flow_ref == NULL)
            {
                BPLib_STOR_CACHE_DataserviceGetEid(&bundle_src, &BPLib_STOR_CACHE_BblockPrimaryGetLogical(pri_block)->sourceEID);
                BPLib_STOR_CACHE_DataserviceGetEid(&bundle_dest, &BPLib_STOR_CACHE_BblockPrimaryGetLogical(pri_block)->destinationEID);

                /* Find a dataservice that matches this src/dest combo */
                tgt_subintf = BPLib_STOR_CACHE_RBT_SearchUnique(bundle_dest.service_number, &base_intf->service_index);
                if (tgt_subintf != NULL)
                {
                    /* borrows the ref */
                    /* because rbt_link is first element */
                    next_flow_ref = ((bplib_service_endpt_t *)tgt_subintf)->subflow_ref;
                }
            }

            if (next_flow_ref != NULL)
            {
                next_flow = BPLib_STOR_CACHE_FlowCast(BPLib_STOR_CACHE_Dereference(next_flow_ref));
                if (next_flow != NULL && BPLib_STOR_CACHE_FlowTryPush(&next_flow->egress, pblk, 0))
                {
                    pblk = NULL;
                }
            }
        }

        if (pblk != NULL)
        {
            /* undeliverable bundle */
            BPLib_STOR_CACHE_RecycleBlock(pblk);
        }
    }

    return forward_count;
    #else // STOR
    return 0;
    #endif // STOR
}

/**
 * @brief Append a sub-flow (data service) to the base interface block
 *
 * @param base_intf_blk
 * @param svc_num
 * @param type
 * @param endpoint_intf_ref
 */
int bplib_serviceflow_add_to_base(BPLib_STOR_CACHE_Block_t *base_intf_blk, bp_val_t svc_num, bplib_dataservice_type_t type,
                                  BPLib_STOR_CACHE_Ref_t endpoint_intf_ref)
{
    #ifdef STOR
    bplib_route_serviceintf_info_t *base_intf;
    bplib_service_endpt_t          *endpoint_intf;
    int                             status;

    base_intf = BPLib_STOR_CACHE_GenericDataCast(base_intf_blk, BPLIB_BLOCKTYPE_SERVICE_BASE);
    if (base_intf != NULL)
    {
        BPLib_STOR_CACHE_Block_t *temp_block;
        temp_block = BPLib_STOR_CACHE_GenericDataAlloc(BPLib_STOR_CACHE_GetParentPoolFromLink(base_intf_blk),
                                                    BPLIB_BLOCKTYPE_SERVICE_ENDPOINT, NULL);
        if (temp_block == NULL)
        {
            bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): out of memory when binding socket\n", __func__);
            return BP_ERROR;
        }

        endpoint_intf           = BPLib_STOR_CACHE_GenericDataCast(temp_block, BPLIB_BLOCKTYPE_SERVICE_ENDPOINT);
        endpoint_intf->self_ptr = temp_block;

        /* This can fail in the event the service number is duplicated */
        status = BPLib_STOR_CACHE_RBT_InsertValueUnique(svc_num, &base_intf->service_index, &endpoint_intf->rbt_link);
        if (status == BP_SUCCESS)
        {
            /* success */
            endpoint_intf->subflow_ref = BPLib_STOR_CACHE_RefDuplicate(endpoint_intf_ref);
            if (type == bplib_dataservice_type_storage)
            {
                if (base_intf->storage_service != NULL)
                {
                    BPLib_STOR_CACHE_RefRelease(base_intf->storage_service);
                }
                base_intf->storage_service = BPLib_STOR_CACHE_RefDuplicate(endpoint_intf_ref);
            }
        }
        else
        {
            BPLib_STOR_CACHE_RecycleBlock(temp_block);
        }
    }
    else
    {
        status = BP_ERROR;
    }

    return status;
    #else // STOR
    return BP_SUCCESS;
    #endif // STOR
}

/**
 * @brief Append a sub-flow (data service) to the base interface block
 *
 * @param base_intf_blk
 * @param svc_num
 */
BPLib_STOR_CACHE_Ref_t bplib_serviceflow_remove_from_base(BPLib_STOR_CACHE_Block_t *base_intf_blk, bp_val_t svc_num)
{
    #ifdef STOR

    bplib_route_serviceintf_info_t *base_intf;
    BPLib_STOR_CACHE_RBT_Link_t               *rbt_link;
    bplib_service_endpt_t          *endpoint_intf;
    BPLib_STOR_CACHE_Ref_t               endpoint_intf_ref;
    int                             status;

    endpoint_intf_ref = NULL;
    base_intf         = BPLib_STOR_CACHE_GenericDataCast(base_intf_blk, BPLIB_BLOCKTYPE_SERVICE_BASE);
    if (base_intf != NULL)
    {
        /* This can fail in the event the service number is duplicated */
        rbt_link = BPLib_STOR_CACHE_RBT_SearchUnique(svc_num, &base_intf->service_index);
        if (rbt_link == NULL)
        {
            status = BP_ERROR;
        }
        else
        {
            status = BPLib_STOR_CACHE_RBT_ExtractNode(&base_intf->service_index, rbt_link);
        }
        if (status == BP_SUCCESS)
        {
            endpoint_intf     = (bplib_service_endpt_t *)rbt_link; /* because its the first item */
            endpoint_intf_ref = endpoint_intf->subflow_ref;

            if (endpoint_intf_ref == base_intf->storage_service)
            {
                BPLib_STOR_CACHE_RefRelease(base_intf->storage_service);
                base_intf->storage_service = NULL;
            }

            BPLib_STOR_CACHE_RecycleBlock(endpoint_intf->self_ptr);
        }
    }
    return endpoint_intf_ref;
    #else // STOR
    return (BPLib_STOR_CACHE_Ref_t)NULL;
    #endif // STOR
}

int bplib_dataservice_event_impl(void *arg, BPLib_STOR_CACHE_Block_t *intf_block)
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
        if (flow->current_state_flags & BPLIB_MEM_FLOW_FLAGS_ENDPOINT)
        {
            BPLib_STOR_CACHE_FlowEnable(&flow->ingress, BPLIB_MEM_SHORT_SUBQ_DEPTH);
        }
        else
        {
            BPLib_STOR_CACHE_FlowEnable(&flow->ingress, BPLIB_MEM_MAX_SUBQ_DEPTH);
        }
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

int bplib_dataservice_base_construct(void *arg, BPLib_STOR_CACHE_Block_t *blk)
{
    #ifdef STOR
    bplib_route_serviceintf_info_t *base_intf;

    base_intf = BPLib_STOR_CACHE_GenericDataCast(blk, BPLIB_BLOCKTYPE_SERVICE_BASE);
    if (base_intf == NULL)
    {
        return BP_ERROR;
    }

    BPLib_STOR_CACHE_RBT_InitRoot(&base_intf->service_index);
    #endif // STOR

    return BP_SUCCESS;
}

int bplib_dataservice_block_recycle(void *arg, BPLib_STOR_CACHE_Block_t *rblk)
{
    /* this should check if the block made it to storage or not, and if the calling
     * task in bplib_send() is blocked waiting for the block to be sent, this should
     * release that task. In is a no-op for now until timeouts are implemented. */
    return BP_SUCCESS;
}

void bplib_dataservice_init(BPLib_STOR_CACHE_Pool_t *pool)
{
    const BPLib_STOR_CACHE_BlocktypeApi_t svc_base_api = (BPLib_STOR_CACHE_BlocktypeApi_t) {
        .construct = bplib_dataservice_base_construct,
        .destruct  = NULL,
    };

    const BPLib_STOR_CACHE_BlocktypeApi_t svc_block_api = (BPLib_STOR_CACHE_BlocktypeApi_t) {
        .construct = NULL,
        .destruct  = bplib_dataservice_block_recycle,
    };

    BPLib_STOR_CACHE_RegisterBlocktype(pool, BPLIB_BLOCKTYPE_SERVICE_BASE, &svc_base_api,
                                 #ifdef STOR
                                   sizeof(bplib_route_serviceintf_info_t));
                                 #endif // STOR
                                   128);  // STOR - just a temporary guess.
    #ifdef STOR
    BPLib_STOR_CACHE_RegisterBlocktype(pool, BPLIB_BLOCKTYPE_SERVICE_ENDPOINT, NULL, sizeof(bplib_service_endpt_t));
    #endif // STOR
    BPLib_STOR_CACHE_RegisterBlocktype(pool, BPLIB_BLOCKTYPE_SERVICE_SOCKET, NULL, sizeof(bplib_socket_info_t));
    BPLib_STOR_CACHE_RegisterBlocktype(pool, BPLIB_BLOCKTYPE_SERVICE_BLOCK, &svc_block_api, 0);
}

bp_handle_t bplib_dataservice_add_base_intf(bplib_routetbl_t *rtbl, bp_ipn_t node_number)
{
    #ifdef STOR
    BPLib_STOR_CACHE_Block_t            *sblk;
    bplib_route_serviceintf_info_t *base_intf;
    bp_handle_t                     self_intf_id;
    BPLib_STOR_CACHE_Pool_t                  *pool;

    pool = bplib_route_get_mpool(rtbl);

    /* register Dataservice API module */
    bplib_dataservice_init(pool);

    /* Allocate Blocks */
    sblk = BPLib_STOR_CACHE_FlowAlloc(pool, BPLIB_BLOCKTYPE_SERVICE_BASE, NULL);
    if (sblk == NULL)
    {
        bplog(NULL, BP_FLAG_OUT_OF_MEMORY, "Failed to allocate intf block\n");
        return BP_INVALID_HANDLE;
    }

    base_intf = BPLib_STOR_CACHE_GenericDataCast(sblk, BPLIB_BLOCKTYPE_SERVICE_BASE);
    if (base_intf == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Failed to convert sblk to ref\n");
        self_intf_id = BP_INVALID_HANDLE;
    }
    else
    {
        self_intf_id           = bplib_route_register_generic_intf(rtbl, BP_INVALID_HANDLE, sblk);
        base_intf->node_number = node_number;
    }

    if (bp_handle_is_valid(self_intf_id))
    {
        bplib_route_register_forward_ingress_handler(rtbl, self_intf_id, bplib_serviceflow_forward_ingress);
        bplib_route_register_forward_egress_handler(rtbl, self_intf_id, bplib_serviceflow_forward_egress);
        bplib_route_register_event_handler(rtbl, self_intf_id, bplib_dataservice_event_impl);
    }
    else
    {
        BPLib_STOR_CACHE_RecycleBlock(sblk);
    }

    return self_intf_id;
    #else // STOR
    bp_handle_t null_hdl;
    null_hdl.hdl = 0;
    return null_hdl;
    #endif // STOR
}

bp_handle_t bplib_dataservice_attach(bplib_routetbl_t *tbl, const bp_ipn_addr_t *ipn, bplib_dataservice_type_t type,
                                     BPLib_STOR_CACHE_Ref_t blkref)
{
    #ifdef STOR

    bp_handle_t         self_intf_id = BP_INVALID_HANDLE;
    bp_handle_t         parent_intf_id;
    BPLib_STOR_CACHE_Ref_t   parent_block_ref;
    BPLib_STOR_CACHE_Flow_t *flow;
    int                 status;

    flow = BPLib_STOR_CACHE_FlowCast(BPLib_STOR_CACHE_Dereference(blkref));
    if (flow == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): bad descriptor\n", __func__);
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

    status =
        bplib_serviceflow_add_to_base(BPLib_STOR_CACHE_Dereference(parent_block_ref), ipn->service_number, type, blkref);
    if (status != BP_SUCCESS)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): cannot add service %lu to node %lu - duplicate?\n", __func__,
              (unsigned long)ipn->node_number, (unsigned long)ipn->node_number);
    }
    else
    {
        self_intf_id = bplib_route_register_generic_intf(tbl, parent_intf_id, BPLib_STOR_CACHE_Dereference(blkref));
        if (!bp_handle_is_valid(self_intf_id))
        {
            bplib_serviceflow_remove_from_base(BPLib_STOR_CACHE_Dereference(parent_block_ref), ipn->service_number);
            bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): could not register service %lu\n", __func__,
                  (unsigned long)ipn->node_number);
        }
        else if (type == bplib_dataservice_type_storage)
        {
            /* success; mark this as a storage-capable intf */
            bplib_route_intf_set_flags(tbl, parent_intf_id, BPLIB_MEM_FLOW_FLAGS_STORAGE);
        }
    }

    bplib_route_release_intf_controlblock(tbl, parent_block_ref);

    return self_intf_id;
    #else // STOR
    bp_handle_t null_hdl;
    null_hdl.hdl = 0;
    return null_hdl;
    #endif // STOR
}

BPLib_STOR_CACHE_Ref_t bplib_dataservice_detach(bplib_routetbl_t *tbl, const bp_ipn_addr_t *ipn)
{
    #ifdef STOR
    bp_handle_t         parent_intf_id;
    BPLib_STOR_CACHE_Ref_t   parent_block_ref;
    BPLib_STOR_CACHE_Ref_t   refptr;
    BPLib_STOR_CACHE_Flow_t *flow;

    /* Find the intf ID, even if it is not currently enabled */
    parent_intf_id   = bplib_route_get_next_intf_with_flags(tbl, ipn->node_number, 0, 0);
    parent_block_ref = bplib_route_get_intf_controlblock(tbl, parent_intf_id);
    if (parent_block_ref == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): no parent intf for node %lu\n", __func__,
              (unsigned long)ipn->node_number);
        return NULL;
    }

    refptr = bplib_serviceflow_remove_from_base(BPLib_STOR_CACHE_Dereference(parent_block_ref), ipn->service_number);

    if (refptr != NULL)
    {
        /* also delete its registered intf id (so this mirrors bplib_dataservice_attach) */
        /* this should always work, no recourse if it doesn't */
        flow = BPLib_STOR_CACHE_FlowCast(BPLib_STOR_CACHE_Dereference(refptr));
        if (flow != NULL)
        {
            bplib_route_del_intf(tbl, BPLib_STOR_CACHE_GetExternalId(BPLib_STOR_CACHE_Dereference(refptr)));
        }
    }

    bplib_route_release_intf_controlblock(tbl, parent_block_ref);

    return refptr;
    #else // STOR
    return (BPLib_STOR_CACHE_Ref_t)NULL;
    #endif // STOR
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

bp_socket_t *bplib_create_socket(bplib_routetbl_t *rtbl)
{
    #ifdef STOR

    BPLib_STOR_CACHE_Pool_t       *pool;
    BPLib_STOR_CACHE_Block_t *sblk;
    bplib_socket_info_t *sock;
    BPLib_STOR_CACHE_Ref_t    sock_ref;

    pool = bplib_route_get_mpool(rtbl);

    /* Allocate Flow */
    sblk = BPLib_STOR_CACHE_FlowAlloc(pool, BPLIB_BLOCKTYPE_SERVICE_SOCKET, NULL);
    if (sblk != NULL)
    {
        sock = BPLib_STOR_CACHE_GenericDataCast(sblk, BPLIB_BLOCKTYPE_SERVICE_SOCKET);
        assert(sock != NULL);

        sock->parent_rtbl = rtbl;

        /* following parameters are just hardcoded for now, need an API to set these items */
        sock->params.crctype               = bp_crctype_CRC16;
        sock->params.local_delivery_policy = bplib_policy_delivery_custody_tracking;
        sock->params.local_retx_interval   = 30000;
        sock->params.lifetime              = 3600000;

        sock_ref = BPLib_STOR_CACHE_RefCreate(sblk);
    }
    else
    {
        bplog(NULL, BP_FLAG_OUT_OF_MEMORY, "Cannot open socket: not enough memory\n");
        sock_ref = NULL;
    }

    #else // STOR
    BPLib_STOR_CACHE_Ref_t sock_ref = NULL;
    #endif // STOR

    return (bp_socket_t *)sock_ref;

}

int bplib_bind_socket(bp_socket_t *desc, const bp_ipn_addr_t *source_ipn)
{
    #ifdef STOR

    bplib_socket_info_t *sock;
    BPLib_STOR_CACHE_Ref_t    sock_ref;

    sock_ref = (BPLib_STOR_CACHE_Ref_t)desc;

    sock = BPLib_STOR_CACHE_GenericDataCast(BPLib_STOR_CACHE_Dereference(sock_ref), BPLIB_BLOCKTYPE_SERVICE_SOCKET);
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
    bplib_route_register_forward_ingress_handler(sock->parent_rtbl, sock->socket_intf_id,
                                                 bplib_route_ingress_to_parent);
    bplib_route_register_event_handler(sock->parent_rtbl, sock->socket_intf_id, bplib_dataservice_event_impl);

    sock->params.local_ipn  = *source_ipn;
    sock->params.report_ipn = *source_ipn;
    #endif // STOR

    return BP_SUCCESS;
}

int bplib_connect_socket(bp_socket_t *desc, const bp_ipn_addr_t *destination_ipn)
{
    #ifdef STOR

    bplib_socket_info_t *sock;
    BPLib_STOR_CACHE_Ref_t    sock_ref;

    sock_ref = (BPLib_STOR_CACHE_Ref_t)desc;

    sock = BPLib_STOR_CACHE_GenericDataCast(BPLib_STOR_CACHE_Dereference(sock_ref), BPLIB_BLOCKTYPE_SERVICE_SOCKET);
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

    bplib_route_intf_set_flags(sock->parent_rtbl, sock->socket_intf_id,
                               BPLIB_MEM_FLOW_FLAGS_ENDPOINT | BPLIB_INTF_STATE_ADMIN_UP | BPLIB_INTF_STATE_OPER_UP);

    return 0;

    #else // STOR
    return -1;
    #endif // STOR
}

void bplib_close_socket(bp_socket_t *desc)
{
    #ifdef STOR

    bplib_socket_info_t *sock;
    BPLib_STOR_CACHE_Ref_t    sock_ref;
    BPLib_STOR_CACHE_Ref_t    detached_ref;

    sock_ref = (BPLib_STOR_CACHE_Ref_t)desc;

    sock = BPLib_STOR_CACHE_GenericDataCast(BPLib_STOR_CACHE_Dereference(sock_ref), BPLIB_BLOCKTYPE_SERVICE_SOCKET);
    if (sock == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): bad descriptor\n", __func__);
        return;
    }

    if (sock->params.local_ipn.node_number != 0)
    {
        detached_ref = bplib_dataservice_detach(sock->parent_rtbl, &sock->params.local_ipn);
        if (detached_ref != NULL)
        {
            BPLib_STOR_CACHE_RefRelease(detached_ref);
            detached_ref = NULL;
        }

        memset(&sock->params.local_ipn, 0, sizeof(sock->params.local_ipn));
    }

    /* recycle the original */
    sock->parent_rtbl = NULL;
    BPLib_STOR_CACHE_RefRelease(sock_ref);

    #endif // STOR

}

int bplib_send(bp_socket_t *desc, const void *payload, size_t size, uint32_t timeout)
{
    #ifdef STOR

    int                           status;
    BPLib_STOR_CACHE_Block_t          *rblk;
    BPLib_STOR_CACHE_Flow_t           *flow;
    BPLib_STOR_CACHE_Ref_t             refptr;
    BPLib_STOR_CACHE_Block_t          *pblk;
    BPLib_STOR_CACHE_BblockPrimary_t *pri_block;
    BPLib_STOR_CACHE_Ref_t             sock_ref;
    bplib_socket_info_t          *sock;
    uint64_t                      ingress_time;
    uint64_t                      ingress_limit;

    sock_ref      = (BPLib_STOR_CACHE_Ref_t)desc;
    ingress_time  = bplib_os_get_dtntime_ms();
    ingress_limit = ingress_time + timeout;

    sock = BPLib_STOR_CACHE_GenericDataCast(BPLib_STOR_CACHE_Dereference(sock_ref), BPLIB_BLOCKTYPE_SERVICE_SOCKET);
    if (sock == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): bad descriptor\n", __func__);
        return BP_ERROR;
    }

    flow = BPLib_STOR_CACHE_FlowCast(BPLib_STOR_CACHE_Dereference(sock_ref));
    if (flow == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): bad flow ref - is socket connected?\n", __func__);
        return BP_ERROR;
    }

    /* If no pri block is available, this should block and wait for one (up to ingress_limit) */
    pblk = BPLib_STOR_CACHE_BblockPrimaryAlloc(bplib_route_get_mpool(sock->parent_rtbl), 0, NULL, BPLIB_MEM_ALLOC_PRI_LO,
                                            ingress_limit);
    if (pblk == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): unable to alloc pri block\n", __func__);
        return BP_TIMEOUT;
    }

    status = bplib_serviceflow_bundleize_payload(sock, pblk, payload, size);
    if (status != BP_SUCCESS)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): cannot bundleize payload, out of memory?\n", __func__);
        return status;
    }

    /* convert to a dynamically-managed ref for passing in queues */
    refptr = BPLib_STOR_CACHE_RefCreate(pblk);
    if (refptr == NULL)
    {
        /* not expected... */
        BPLib_STOR_CACHE_RecycleBlock(pblk);
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Cannot convert payload to ref\n");
        return BP_ERROR;
    }
    pblk = NULL; /* only the ref should be used from here */

    rblk = BPLib_STOR_CACHE_RefMakeBlock(refptr, BPLIB_BLOCKTYPE_SERVICE_BLOCK, NULL);
    if (rblk != NULL)
    {
        pri_block = BPLib_STOR_CACHE_BblockPrimaryCast(rblk);
        if (pri_block != NULL)
        {
            pri_block->data.delivery.ingress_intf_id = BPLib_STOR_CACHE_GetExternalId(BPLib_STOR_CACHE_Dereference(sock_ref));
            pri_block->data.delivery.ingress_time    = ingress_time;
        }

        if (BPLib_STOR_CACHE_FlowTryPush(&flow->ingress, rblk, ingress_limit))
        {
            sock->ingress_byte_count += size;
            status = BP_SUCCESS;
        }
        else
        {
            status = BP_TIMEOUT;
        }
    }
    else
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): failed to create block ref, out of memory?\n", __func__);
        status = BP_ERROR;
    }

    /*
     * JPHFIX - to implement a timeout, this should wait/confirm here that the bundle either
     * reached a storage (for custody-tracked) or made it to the next hop CLA (for best-effort svc level)
     */

    BPLib_STOR_CACHE_RefRelease(refptr);

    bplib_route_set_maintenance_request(sock->parent_rtbl);

    return status;
    #else // STOR
    return BP_SUCCESS;
    #endif // STOR
}

int bplib_recv(bp_socket_t *desc, void *payload, size_t *size, uint32_t timeout)
{
    #ifdef STOR

    int                           status;
    bplib_socket_info_t          *sock;
    BPLib_STOR_CACHE_BblockPrimary_t *pri_block;
    BPLib_STOR_CACHE_Block_t          *pblk;
    BPLib_STOR_CACHE_Ref_t             refptr;
    BPLib_STOR_CACHE_Ref_t             sock_ref;
    BPLib_STOR_CACHE_Flow_t           *flow;
    uint64_t                      egress_time_limit;

    sock_ref = (BPLib_STOR_CACHE_Ref_t)desc;

    sock = BPLib_STOR_CACHE_GenericDataCast(BPLib_STOR_CACHE_Dereference(sock_ref), BPLIB_BLOCKTYPE_SERVICE_SOCKET);
    if (sock == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): bad descriptor\n", __func__);
        return BP_ERROR;
    }

    flow = BPLib_STOR_CACHE_FlowCast(BPLib_STOR_CACHE_Dereference(sock_ref));
    if (flow == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): bad flow_ref\n", __func__);
        return BP_ERROR;
    }

    /* preemptively trigger the maintenance task to run */
    /* this may help in the event that the queue is currently empty but
     * there is data somewhere else in the pool that is headed here (assuming
     * timeout is nonzero). */
    bplib_route_set_maintenance_request(sock->parent_rtbl);

    if (timeout == 0)
    {
        egress_time_limit = 0;
    }
    else
    {
        egress_time_limit = bplib_os_get_dtntime_ms() + timeout;
    }

    pblk = BPLib_STOR_CACHE_FlowTryPull(&flow->egress, egress_time_limit);

    if (pblk == NULL)
    {
        status = BP_TIMEOUT;
    }
    else
    {
        refptr = BPLib_STOR_CACHE_RefFromBlock(pblk);

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
        if (status == BP_SUCCESS)
        {
            sock->egress_byte_count += *size;

            pri_block = BPLib_STOR_CACHE_BblockPrimaryCast(pblk);
            if (pri_block != NULL)
            {
                pri_block->data.delivery.egress_intf_id =
                    BPLib_STOR_CACHE_GetExternalId(BPLib_STOR_CACHE_Dereference(sock_ref));
                pri_block->data.delivery.egress_time = bplib_os_get_dtntime_ms();
            }
        }

        BPLib_STOR_CACHE_RecycleBlock(pblk);
    }

    return status;
    #else // STOR
    return BP_SUCCESS;
    #endif // STOR
}
