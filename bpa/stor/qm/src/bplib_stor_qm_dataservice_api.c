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

#include <assert.h>

#include "bplib_time.h"

#include "bplib_mem.h"
#include "bplib_mem_rbtree.h"

#include "bplib_stor_cache_types.h"
#include "bplib_stor_cache_internal.h"
#include "bplib_stor_cache_block.h"
#include "bplib_stor_cache_ref.h"
#include "bplib_stor_cache_base_internal.h"

#include "bplib_stor_qm.h"
#include "bplib_stor_qm_base_internal.h"

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

int BPLib_STOR_CACHE_ServiceductBundleizePayload (
    BPLib_STOR_CACHE_SocketInfo_t *sock_inf, BPLib_STOR_CACHE_Block_t *pblk,
    const void *content, size_t size)
{
    BPLib_STOR_CACHE_Block_t            *cblk;
    BPLib_STOR_CACHE_BblockPrimary_t   *pri_block;
    BPLib_STOR_CACHE_PrimaryBlock_t             *pri;
    BPLib_STOR_CACHE_BblockCanonical_t *ccb_pay;
    bp_canonical_block_buffer_t    *pay;
    int                             result;

    cblk   = NULL;
    result = BPLIB_ERROR;

    do
    {
        pri_block = BPLib_STOR_CACHE_BblockPrimaryCast(pblk);
        if (pri_block == NULL)
        {
            // TODO remove bplog(NULL, BPLIB_FLAG_OUT_OF_MEMORY, "Failed to allocate primary block\n");
            break;
        }

        pri = BPLib_STOR_CACHE_BblockPrimaryGetLogical(pri_block);

        /* Initialize Primary Block */
        pri->version = 7;

        BPLib_STOR_QM_SetEid(&pri->destinationEID, &sock_inf->params.remote_ipn);
        BPLib_STOR_QM_SetEid(&pri->sourceEID, &sock_inf->params.local_ipn);
        BPLib_STOR_QM_SetEid(&pri->reportEID, &sock_inf->params.report_ipn);

        pri->creationTimeStamp.sequence_num = sock_inf->last_bundle_seq;
        ++sock_inf->last_bundle_seq;

        pri->creationTimeStamp.time = BPLIB_DTNTIME_UNDEFINED;

        pri->lifetime                     = sock_inf->params.lifetime;
        pri->controlFlags.isAdminRecord   = sock_inf->params.is_admin_service;
        pri->controlFlags.mustNotFragment = !sock_inf->params.allow_fragmentation;
        pri->crctype                      = sock_inf->params.crctype;

        pri_block->data.delivery.delivery_policy     = sock_inf->params.local_delivery_policy;
        pri_block->data.delivery.local_retx_interval = sock_inf->params.local_retx_interval;

        /* Pre-Encode Primary Block */
        if (v7_block_encode_pri(pri_block) < 0)
        {
            // TODO remove bplog(NULL, BPLIB_FLAG_OUT_OF_MEMORY, "Failed encoding pri block\n");
            break;
        }

        /* Update Payload Block */
        cblk    = BPLib_STOR_CACHE_BblockCanonicalAlloc(BPLib_STOR_QM_GetQtblPool(sock_inf->parent_rtbl), 0, NULL);
        ccb_pay = BPLib_STOR_CACHE_BblockCanonicalCast(cblk);
        if (ccb_pay == NULL)
        {
            // TODO remove bplog(NULL, BPLIB_FLAG_OUT_OF_MEMORY, "Failed to allocate payload block\n");
            break;
        }

        pay = BPLib_STOR_CACHE_BblockCanonicalGetLogical(ccb_pay);

        pay->canonical_block.blockNum  = 1;
        pay->canonical_block.blockType = bp_blocktype_payloadBlock;
        pay->canonical_block.crctype   = sock_inf->params.crctype;

        /* Encode Payload Block */
        if (v7_block_encode_pay(ccb_pay, content, size) < 0)
        {
            // TODO remove bplog(NULL, BPLIB_FLAG_OUT_OF_MEMORY, "Failed encoding pay block\n");
            break;
        }

        BPLib_STOR_CACHE_BblockPrimaryAppend(pri_block, cblk);
        cblk   = NULL; /* do not need now that it is stored */
        result = BPLIB_SUCCESS;
    } while (false);

    /* clean up, if anything did not work, recycle the blocks now */
    if (cblk != NULL)
    {
        BPLib_STOR_CACHE_RecycleBlock(cblk);
    }

    return result;
}

int BPLib_STOR_CACHE_ServiceductUnbundleizePayload(
    BPLib_STOR_CACHE_SocketInfo_t *sock_inf, BPLib_STOR_CACHE_Ref_t refptr,
    void *content, size_t *size)
{
    BPLib_STOR_CACHE_BblockPrimary_t   *pri;
    BPLib_STOR_CACHE_BblockCanonical_t *ccb_pay;
    BPLib_STOR_CACHE_Block_t            *cblk;
    size_t                          content_size;
    size_t                          content_offset;
    size_t                          temp_size;
    int                             status;

    status = BPLIB_ERROR;

    do
    {
        pri = BPLib_STOR_CACHE_BblockPrimaryCast(BPLib_STOR_CACHE_Dereference(refptr));
        if (pri == NULL)
        {
            /* not a pri block, cannot be unbundled */
            // TODO remove bplog(NULL, BPLIB_FLAG_INCOMPLETE, "%s(): Not a primary block\n", __func__);
            break;
        }

        /* find the payload */
        cblk    = BPLib_STOR_CACHE_BblockPrimaryLocateCanonical(pri, bp_blocktype_payloadBlock);
        ccb_pay = BPLib_STOR_CACHE_BblockCanonicalCast(cblk);
        if (ccb_pay == NULL)
        {
            /* no payload, cannot be unbundled */
            // TODO remove bplog(NULL, BPLIB_FLAG_INCOMPLETE, "%s(): No payload\n", __func__);
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
            // TODO remove bplog(NULL, BPLIB_FLAG_INCOMPLETE, "%s(): Incorrectly sized bundle\n", __func__);
            break;
        }

        temp_size = BPLib_STOR_CACHE_BblockCborExport(BPLib_STOR_CACHE_BblockCanonicalGetEncodedChunks(ccb_pay), content,
                                                   *size, content_offset, content_size);

        /* This should always match, some sort of bug if it does not */
        if (temp_size != content_size)
        {
            // TODO remove bplog(NULL, BPLIB_FLAG_INCOMPLETE, "%s(): Failed to copy complete payload\n", __func__);
            break;
        }

        /* all done */
        *size  = content_size;
        status = BPLIB_SUCCESS;
    } while (false);

    /* Always - The reference can be discarded (caller should have duped it) */
    BPLib_STOR_CACHE_RefRelease(refptr);

    return status;
}

int BPLib_STOR_CACHE_ServiceductForwardIngress(void *arg, BPLib_STOR_CACHE_Block_t *subq_src)
{
    BPLib_STOR_QM_ServiceintfInfo_t *base_intf;
    BPLib_STOR_CACHE_BblockPrimary_t   *pri_block;
    BPLib_STOR_CACHE_Block_t            *qblk;
    BPLib_STOR_CACHE_Block_t            *intf_block;
    BPLib_STOR_QM_Duct_t             *curr_duct;
    BPLib_STOR_QM_Duct_t             *storage_duct;
    int                             forward_count;

    intf_block = BPLib_STOR_CACHE_GetBlockFromLink(subq_src);
    base_intf  = BPLib_MEM_GenericDataCast((BPLib_MEM_Block_t *)intf_block, BPLIB_BLOCKTYPE_SERVICE_BASE);
    if (base_intf == NULL)
    {
        return BPLIB_ERROR;  // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "intf_block invalid\n");
    }

    curr_duct = BPLib_STOR_QM_DuctCast(intf_block);
    if (curr_duct == NULL)
    {
        return -1;
    }

    forward_count = 0;
    while (true)
    {
        qblk = BPLib_STOR_QM_DuctTryPull(&curr_duct->ingress, BPLIB_MONOTIME_ZERO);
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
         * this function will also put it there. Otherwise, queue it normally.
         * If this dataservice is storage-capable, and the bundle has NOT gone
         * to the storage service yet, send it there now */
        pri_block = BPLib_STOR_CACHE_BblockPrimaryCast(qblk);
        if (pri_block != NULL && base_intf->storage_service != NULL &&
            !bp_handle_is_valid(pri_block->data.delivery.storage_intf_id))
        {
            storage_duct = BPLib_STOR_QM_DuctCast(BPLib_STOR_CACHE_Dereference(base_intf->storage_service));
            if (storage_duct != NULL && BPLib_STOR_QM_DuctTryPush(&storage_duct->egress, qblk, BPLIB_MONOTIME_ZERO))
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
            BPLib_STOR_QM_IngressQueueSingleBundle(arg, qblk);
        }
    }

    /* This should return 0 if it did no work and no errors.
     * Should return >0 if some work was done */
    return forward_count;
}

int BPLib_STOR_CACHE_ServiceductForwardEgress(void *arg, BPLib_STOR_CACHE_Block_t *subq_src)
{
    BPLib_STOR_QM_ServiceintfInfo_t  *base_intf;
    BPLib_MEM_RBT_Link_t             *tgt_subintf;
    BPLib_STOR_QM_Duct_t          *curr_duct;
    BPLib_STOR_QM_Duct_t          *next_duct;
    BPLib_STOR_CACHE_Ref_t            next_duct_ref;
    BPLib_STOR_CACHE_Block_t         *pblk;
    BPLib_STOR_CACHE_Block_t         *intf_block;
    BPLib_STOR_CACHE_BblockPrimary_t *pri_block;
    bp_ipn_addr_t                     bundle_src;
    bp_ipn_addr_t                     bundle_dest;
    int                               forward_count;

    intf_block = BPLib_STOR_CACHE_GetBlockFromLink(subq_src);
    base_intf  = BPLib_MEM_GenericDataCast((BPLib_MEM_Block_t *)intf_block, BPLIB_BLOCKTYPE_SERVICE_BASE);
    if (base_intf == NULL)
    {
        return BPLIB_ERROR;  // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "intf_block invalid\n");
    }

    curr_duct = BPLib_STOR_QM_DuctCast(intf_block);
    if (curr_duct == NULL)
    {
        return -1;
    }

    forward_count = 0;
    while (true)
    {
        pblk = BPLib_STOR_QM_DuctTryPull(&curr_duct->egress, BPLIB_MONOTIME_ZERO);
        if (pblk == NULL)
        {
            /* no bundles available to recv */
            break;
        }

        ++forward_count;

        next_duct_ref = NULL;
        pri_block     = BPLib_STOR_CACHE_BblockPrimaryCast(pblk);
        if (pri_block != NULL)
        {
            /* Egress bundles also need to be checked if they need to go to storage */
            if (!bp_handle_is_valid(pri_block->data.delivery.storage_intf_id) && base_intf->storage_service != NULL)
            {
                /* borrows the ref */
                next_duct_ref = base_intf->storage_service;
            }

            if (next_duct_ref == NULL)
            {
                BPLib_STOR_QM_GetEid(&bundle_src, &BPLib_STOR_CACHE_BblockPrimaryGetLogical(pri_block)->sourceEID);
                BPLib_STOR_QM_GetEid(&bundle_dest, &BPLib_STOR_CACHE_BblockPrimaryGetLogical(pri_block)->destinationEID);

                /* Find a dataservice that matches this src/dest combo */
                tgt_subintf = BPLib_MEM_RBT_SearchUnique(bundle_dest.service_number, &base_intf->service_index);
                if (tgt_subintf != NULL)
                {
                    /* borrows the ref */
                    /* because rbt_link is first element */
                    next_duct_ref = (BPLib_STOR_CACHE_Ref_t)tgt_subintf->right; // TODO Right, right?
                }
            }

            if (next_duct_ref != NULL)
            {
                next_duct = BPLib_STOR_QM_DuctCast(BPLib_STOR_CACHE_Dereference(next_duct_ref));
                if (next_duct != NULL && BPLib_STOR_QM_DuctTryPush(&next_duct->egress, pblk, BPLIB_MONOTIME_ZERO))
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
}

/**
 * @brief Append a sub-duct (data service) to the base interface block
 *
 * @param base_intf_blk
 * @param svc_num
 * @param type
 * @param endpoint_intf_ref
 */
int BPLib_STOR_CACHE_ServiceductAddToBase(BPLib_STOR_CACHE_Block_t *base_intf_blk, bp_val_t svc_num,BPLib_STOR_CACHE_DataserviceType_t type,
                                  BPLib_STOR_CACHE_Ref_t endpoint_intf_ref)
{
    BPLib_STOR_QM_ServiceintfInfo_t *base_intf;
    BPLib_STOR_CACHE_ServiceEndpt_t *endpoint_intf;
    int                              status;

    base_intf = BPLib_MEM_GenericDataCast((BPLib_MEM_Block_t *)base_intf_blk, BPLIB_BLOCKTYPE_SERVICE_BASE);
    if (base_intf != NULL)
    {
        BPLib_STOR_CACHE_Block_t *temp_block;
        temp_block = BPLib_STOR_CACHE_GenericDataAlloc(BPLib_STOR_CACHE_GetParentPoolFromLink(base_intf_blk),
                                                    BPLIB_BLOCKTYPE_SERVICE_ENDPOINT, NULL);
        if (temp_block == NULL)
        {
            // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "%s(): out of memory when binding socket\n", __func__);
            return BPLIB_ERROR;
        }

        endpoint_intf           = BPLib_MEM_GenericDataCast((BPLib_MEM_Block_t *)temp_block, BPLIB_BLOCKTYPE_SERVICE_ENDPOINT);
        endpoint_intf->self_ptr = temp_block;

        /* This can fail in the event the service number is duplicated */
        status = BPLib_MEM_RBT_InsertValueUnique(svc_num, &base_intf->service_index, &endpoint_intf->rbt_link);
        if (status == BPLIB_SUCCESS)
        {
            /* success */
            #ifdef QM_SUBDUCT_REF
            endpoint_intf->subduct_ref = BPLib_STOR_CACHE_RefDuplicate(endpoint_intf_ref);
            if (type == BPLib_STOR_CACHE_DataserviceTypeStorage)
            {
                if (base_intf->storage_service != NULL)
                {
                    BPLib_STOR_CACHE_RefRelease(base_intf->storage_service);
                }
                base_intf->storage_service = BPLib_STOR_CACHE_RefDuplicate(endpoint_intf_ref);
            }
            #endif QM_SUBDUCT_REF
        }
        else
        {
            BPLib_STOR_CACHE_RecycleBlock(temp_block);
        }
    }
    else
    {
        status = BPLIB_ERROR;
    }

    return status;
}

/**
 * @brief Append a sub-duct (data service) to the base interface block
 *
 * @param base_intf_blk
 * @param svc_num
 */
BPLib_STOR_CACHE_Ref_t BPLib_STOR_CACHE_ServiceductRemoveFromBase(BPLib_STOR_CACHE_Block_t *base_intf_blk, bp_val_t svc_num)
{
    BPLib_STOR_QM_ServiceintfInfo_t *base_intf;
    BPLib_MEM_RBT_Link_t            *rbt_link;
   BPLib_STOR_CACHE_ServiceEndpt_t  *endpoint_intf;
    BPLib_STOR_CACHE_Ref_t           endpoint_intf_ref;
    int                              status;

    endpoint_intf_ref = NULL;
    base_intf         = BPLib_MEM_GenericDataCast((BPLib_MEM_Block_t *)base_intf_blk, BPLIB_BLOCKTYPE_SERVICE_BASE);
    if (base_intf != NULL)
    {
        /* This can fail in the event the service number is duplicated */
        rbt_link = BPLib_MEM_RBT_SearchUnique(svc_num, &base_intf->service_index);
        if (rbt_link == NULL)
        {
            status = BPLIB_ERROR;
        }
        else
        {
            status = BPLib_MEM_RBT_ExtractNode(&base_intf->service_index, rbt_link);
        }
        if (status == BPLIB_SUCCESS)
        {
            endpoint_intf     = (BPLib_STOR_CACHE_ServiceEndpt_t *)rbt_link; /* because its the first item */
            #ifdef QM_SUBQ
            endpoint_intf_ref = endpoint_intf->subduct_ref;
            #endif // QM_SUBQ
            if (endpoint_intf_ref == base_intf->storage_service)
            {
                BPLib_STOR_CACHE_RefRelease(base_intf->storage_service);
                base_intf->storage_service = NULL;
            }

            BPLib_STOR_CACHE_RecycleBlock(endpoint_intf->self_ptr);
        }
    }

    return endpoint_intf_ref;
}

int BPLib_STOR_CACHE_DataserviceEventImpl(void *arg, BPLib_STOR_CACHE_Block_t *intf_block)
{
    BPLib_STOR_QM_DuctGenericEvent_t *event;
    BPLib_STOR_QM_Duct_t               *duct;

    event = arg;

    /* only care about state change events for now */
    if (event->event_type != BPLib_STOR_QM_DuctEventUp && event->event_type != BPLib_STOR_QM_DuctEventDown)
    {
        return BPLIB_SUCCESS;
    }

    /* only care about state change events for the local i/f */
    duct = BPLib_STOR_QM_DuctCast(intf_block);
    if (duct == NULL || !bp_handle_equal(event->intf_state.intf_id, BPLib_STOR_CACHE_GetExternalId(intf_block)))
    {
        return BPLIB_SUCCESS;
    }

    if (event->event_type == BPLib_STOR_QM_DuctEventUp)
    {
        /* Allows bundles to be pushed to duct queues */
        if (duct->current_state_flags & BPLIB_CACHE_STATE_FLAG_ENDPOINT)
        {
            BPLib_STOR_QM_DuctEnable(&duct->ingress, BP_MPOOL_SHORT_SUBQ_DEPTH);
        }
        else
        {
            BPLib_STOR_QM_DuctEnable(&duct->ingress, BPLIB_MAX_SUBQ_DEPTH);
        }
        BPLib_STOR_QM_DuctEnable(&duct->egress, BPLIB_MAX_SUBQ_DEPTH);
    }
    else if (event->event_type == BPLib_STOR_QM_DuctEventDown)
    {
        /* drop anything already in the egress queue.  Note that
         * ingress is usually empty, as bundles really should not wait there,
         * so that probably has no effect. */
        BPLib_STOR_QM_DuctDisable(&duct->ingress);
        BPLib_STOR_QM_DuctDisable(&duct->egress);
    }

    return BPLIB_SUCCESS;
}

int BPLib_STOR_CACHE_DataserviceBaseConstruct(void *arg, BPLib_STOR_CACHE_Block_t *blk)
{
    BPLib_STOR_QM_ServiceintfInfo_t *base_intf;

    base_intf = BPLib_MEM_GenericDataCast((BPLib_MEM_Block_t *)blk, BPLIB_BLOCKTYPE_SERVICE_BASE);
    if (base_intf == NULL)
    {
        return BPLIB_ERROR;
    }

    BPLib_MEM_RBT_InitRoot(&base_intf->service_index);
    return BPLIB_SUCCESS;
}

int BPLib_STOR_CACHE_DataserviceBlockRecycle(void *arg, BPLib_STOR_CACHE_Block_t *rblk)
{
    /* this should check if the block made it to storage or not, and if the calling
     * task in BPLib_STOR_CACHE_Send() is blocked waiting for the block to be sent, this should
     * release that task. In is a no-op for now until timeouts are implemented. */
    return BPLIB_SUCCESS;
}

void BPLib_STOR_CACHE_DataserviceInit(BPLib_STOR_CACHE_Pool_t *pool)
{
    const BPLib_STOR_CACHE_BlocktypeApi_t svc_base_api = (BPLib_STOR_CACHE_BlocktypeApi_t) {
        .construct =BPLib_STOR_CACHE_DataserviceBaseConstruct,
        .destruct  = NULL,
    };

    const BPLib_STOR_CACHE_BlocktypeApi_t svc_block_api = (BPLib_STOR_CACHE_BlocktypeApi_t) {
        .construct = NULL,
        .destruct  =BPLib_STOR_CACHE_DataserviceBlockRecycle,
    };

    BPLib_STOR_CACHE_RegisterBlocktype(pool, BPLIB_BLOCKTYPE_SERVICE_BASE, &svc_base_api,
                                   sizeof(BPLib_STOR_QM_ServiceintfInfo_t));
    BPLib_STOR_CACHE_RegisterBlocktype(pool, BPLIB_BLOCKTYPE_SERVICE_ENDPOINT, NULL, sizeof (BPLib_STOR_CACHE_ServiceEndpt_t));
    BPLib_STOR_CACHE_RegisterBlocktype(pool, BPLIB_BLOCKTYPE_SERVICE_SOCKET, NULL, sizeof (BPLib_STOR_CACHE_SocketInfo_t));
    BPLib_STOR_CACHE_RegisterBlocktype(pool, BPLIB_BLOCKTYPE_SERVICE_BLOCK, &svc_block_api, 0);
}

bp_handle_t BPLib_STOR_CACHE_DataserviceAddBaseIntf(BPLib_STOR_QM_QueueTbl_t *rtbl, bp_ipn_t node_number)
{
    BPLib_STOR_CACHE_Block_t            *sblk;
    BPLib_STOR_QM_ServiceintfInfo_t *base_intf;
    bp_handle_t                     self_intf_id;
    BPLib_STOR_CACHE_Pool_t                  *pool;

    pool = BPLib_STOR_QM_GetQtblPool(rtbl);

    /* register Dataservice API module */
   BPLib_STOR_CACHE_DataserviceInit(pool);

    /* Allocate Blocks */
    sblk = BPLib_STOR_QM_DuctAlloc(pool, BPLIB_BLOCKTYPE_SERVICE_BASE, NULL);
    if (sblk == NULL)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_OUT_OF_MEMORY, "Failed to allocate intf block\n");
        return BP_INVALID_HANDLE;
    }

    base_intf = BPLib_MEM_GenericDataCast((BPLib_MEM_Block_t *)sblk, BPLIB_BLOCKTYPE_SERVICE_BASE);
    if (base_intf == NULL)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "Failed to convert sblk to ref\n");
        self_intf_id = BP_INVALID_HANDLE;
    }
    else
    {
        self_intf_id           = BPLib_STOR_QM_RegisterGenericIntf(rtbl, BP_INVALID_HANDLE, sblk);
        base_intf->node_number = node_number;
    }

    if (bp_handle_is_valid(self_intf_id))
    {
        BPLib_STOR_QM_RegisterForwardIngressHandler(rtbl, self_intf_id,BPLib_STOR_CACHE_ServiceductForwardIngress);
        BPLib_STOR_QM_RegisterForwardEgressHandler(rtbl, self_intf_id,BPLib_STOR_CACHE_ServiceductForwardEgress);
        BPLib_STOR_QM_RegisterEventHandler(rtbl, self_intf_id,BPLib_STOR_CACHE_DataserviceEventImpl);
    }
    else
    {
        BPLib_STOR_CACHE_RecycleBlock(sblk);
    }

    return self_intf_id;
}

bp_handle_t BPLib_STOR_CACHE_DataserviceAttach(
    BPLib_STOR_QM_QueueTbl_t *tbl, const bp_ipn_addr_t *ipn,
    BPLib_STOR_CACHE_DataserviceType_t type,
    BPLib_STOR_CACHE_Ref_t blkref)
{
    bp_handle_t         self_intf_id = BP_INVALID_HANDLE;
    bp_handle_t         parent_intf_id;
    BPLib_STOR_CACHE_Ref_t   parent_block_ref;
    BPLib_STOR_QM_Duct_t *duct;
    int                 status;

    duct = BPLib_STOR_QM_DuctCast(BPLib_STOR_CACHE_Dereference(blkref));
    if (duct == NULL)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "%s(): bad descriptor\n", __func__);
        return BP_INVALID_HANDLE;
    }

    /* Find the intf ID, even if it is not currently enabled */
    parent_intf_id = BPLib_STOR_QM_GetNextIntfWithFlags(tbl, ipn->node_number, 0, 0);
    parent_block_ref   = BPLib_STOR_QM_GetIntfControlblock(tbl, parent_intf_id);
    if (parent_block_ref == NULL)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "%s(): no parent intf for node %lu\n", __func__,
        //      (unsigned long)ipn->node_number);
        return BP_INVALID_HANDLE;
    }

    status =
       BPLib_STOR_CACHE_ServiceductAddToBase(BPLib_STOR_CACHE_Dereference(parent_block_ref), ipn->service_number, type, blkref);
    if (status != BPLIB_SUCCESS)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "%s(): cannot add service %lu to node %lu - duplicate?\n", __func__,
        //      (unsigned long)ipn->node_number, (unsigned long)ipn->node_number);
    }
    else
    {
        self_intf_id = BPLib_STOR_QM_RegisterGenericIntf(tbl, parent_intf_id, BPLib_STOR_CACHE_Dereference(blkref));
        if (!bp_handle_is_valid(self_intf_id))
        {
           BPLib_STOR_CACHE_ServiceductRemoveFromBase(BPLib_STOR_CACHE_Dereference(parent_block_ref), ipn->service_number);
            // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "%s(): could not register service %lu\n", __func__,
            //      (unsigned long)ipn->node_number);
        }
        else if (type == BPLib_STOR_CACHE_DataserviceTypeStorage)
        {
            /* success; mark this as a storage-capable intf */
            BPLib_STOR_QM_IntfSetFlags(tbl, parent_intf_id, BPLIB_CACHE_STATE_FLAG_STORAGE);
        }
    }

    BPLib_STOR_QM_ReleaseIntfControlblock(tbl, parent_block_ref);

    return self_intf_id;
}

BPLib_STOR_CACHE_Ref_t BPLib_STOR_CACHE_DataserviceDetach(BPLib_STOR_QM_QueueTbl_t *tbl, const bp_ipn_addr_t *ipn)
{
    bp_handle_t         parent_intf_id;
    BPLib_STOR_CACHE_Ref_t   parent_block_ref;
    BPLib_STOR_CACHE_Ref_t   refptr;
    BPLib_STOR_QM_Duct_t *duct;

    /* Find the intf ID, even if it is not currently enabled */
    parent_intf_id = BPLib_STOR_QM_GetNextIntfWithFlags(tbl, ipn->node_number, 0, 0);
    parent_block_ref   = BPLib_STOR_QM_GetIntfControlblock(tbl, parent_intf_id);
    if (parent_block_ref == NULL)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "%s(): no parent intf for node %lu\n", __func__,
        //      (unsigned long)ipn->node_number);
        return NULL;
    }

    refptr =BPLib_STOR_CACHE_ServiceductRemoveFromBase(BPLib_STOR_CACHE_Dereference(parent_block_ref), ipn->service_number);

    if (refptr != NULL)
    {
        /* also delete its registered intf id (so this mirrors BPLib_STOR_CACHE_DataserviceAttach) */
        /* this should always work, no recourse if it doesn't */
        duct = BPLib_STOR_QM_DuctCast(BPLib_STOR_CACHE_Dereference(refptr));
        if (duct != NULL)
        {
            BPLib_STOR_QM_DelIntf(tbl, BPLib_STOR_CACHE_GetExternalId(BPLib_STOR_CACHE_Dereference(refptr)));
        }
    }

    BPLib_STOR_QM_ReleaseIntfControlblock(tbl, parent_block_ref);

    return refptr;
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

bp_socket_t *BPLib_STOR_CACHE_CreateSocket(BPLib_STOR_QM_QueueTbl_t *rtbl)
{
    BPLib_STOR_CACHE_Pool_t       *pool;
    BPLib_STOR_CACHE_Block_t *sblk;
   BPLib_STOR_CACHE_SocketInfo_t *sock;
    BPLib_STOR_CACHE_Ref_t    sock_ref;

    pool = BPLib_STOR_QM_GetQtblPool(rtbl);

    /* Allocate Duct */
    sblk = BPLib_STOR_QM_DuctAlloc(pool, BPLIB_BLOCKTYPE_SERVICE_SOCKET, NULL);
    if (sblk != NULL)
    {
        sock = BPLib_MEM_GenericDataCast((BPLib_MEM_Block_t *)sblk, BPLIB_BLOCKTYPE_SERVICE_SOCKET);
        assert(sock != NULL);

        sock->parent_rtbl = rtbl;

        /* following parameters are just hardcoded for now, need an API to set these items */
        sock->params.crctype               = bp_crctype_CRC16;
        sock->params.local_delivery_policy =BPLib_STOR_CACHE_PolicyDeliveryCustodyTracking;
        sock->params.local_retx_interval   = 30000;
        sock->params.lifetime              = 3600000;

        sock_ref = BPLib_STOR_CACHE_RefCreate(sblk);
    }
    else
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_OUT_OF_MEMORY, "Cannot open socket: not enough memory\n");
        sock_ref = NULL;
    }

    return (bp_socket_t *)sock_ref;
}


// TODO Newer code in prototype. Some flags needed for error: cast increases required alignment of target type [-Werror=cast-align] ?
int BPLib_STOR_CACHE_BindSocket(bp_socket_t *desc, const bp_ipn_addr_t *source_ipn)
{
    #ifdef NOT_READY // sockets
    BPLib_STOR_CACHE_SocketInfo_t *sock;
    BPLib_STOR_CACHE_Ref_t    sock_ref;

    sock_ref = (BPLib_STOR_CACHE_Ref_t)desc;

    sock = BPLib_MEM_GenericDataCast((BPLib_MEM_Block_t *)BPLib_STOR_CACHE_Dereference(sock_ref), BPLIB_BLOCKTYPE_SERVICE_SOCKET);
    if (sock == NULL)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "%s(): bad descriptor\n", __func__);
        return BPLIB_ERROR;
    }

    if (sock->params.local_ipn.node_number != 0)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "%s(): socket already connected\n", __func__);
        return BPLIB_ERROR;
    }

    if (source_ipn->node_number == 0)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "%s(): invalid source node number\n", __func__);
        return BPLIB_ERROR;
    }

    sock->socket_intf_id =
       BPLib_STOR_CACHE_DataserviceAttach(sock->parent_rtbl, source_ipn,BPLib_STOR_CACHE_DataserviceTypeApplication, sock_ref);
    if (!bp_handle_is_valid(sock->socket_intf_id))
    {
        return BPLIB_ERROR;
    }

    /* There should be no possibility of these registrations failing, as the intf was just created */
    BPLib_STOR_QM_RegisterForwardIngressHandler(sock->parent_rtbl, sock->socket_intf_id,
                                                 BPLib_STOR_QM_QueueIngressToParent);
    BPLib_STOR_QM_RegisterEventHandler(sock->parent_rtbl, sock->socket_intf_id,BPLib_STOR_CACHE_DataserviceEventImpl);

    sock->params.local_ipn  = *source_ipn;
    sock->params.report_ipn = *source_ipn;

    #endif // NOT_READY sockets.
    return BPLIB_SUCCESS;
}

int BPLib_STOR_CACHE_ConnectSocket(bp_socket_t *desc, const bp_ipn_addr_t *destination_ipn)
{
   BPLib_STOR_CACHE_SocketInfo_t *sock;
    BPLib_STOR_CACHE_Ref_t    sock_ref;

    sock_ref = (BPLib_STOR_CACHE_Ref_t)desc;

    sock = BPLib_MEM_GenericDataCast((BPLib_MEM_Block_t *)BPLib_STOR_CACHE_Dereference(sock_ref), BPLIB_BLOCKTYPE_SERVICE_SOCKET);
    if (sock == NULL)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "%s(): bad descriptor\n", __func__);
        return -1;
    }

    if (!bp_handle_is_valid(sock->socket_intf_id))
    {
        /* not yet bound */
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "%s(): socket not bound\n", __func__);
        return -1;
    }

    if (sock->params.remote_ipn.node_number != 0)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "%s(): socket already connected\n", __func__);
        return -1;
    }

    if (destination_ipn->node_number == 0)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "%s(): invalid destination node number\n", __func__);
        return -1;
    }

    sock->params.remote_ipn = *destination_ipn;

    BPLib_STOR_QM_IntfSetFlags(sock->parent_rtbl, sock->socket_intf_id,
                               BPLIB_CACHE_STATE_FLAG_ENDPOINT | BPLIB_INTF_STATE_ADMIN_UP | BPLIB_INTF_STATE_OPER_UP);

    return 0;
}

void BPLib_STOR_CACHE_CloseSocket(bp_socket_t *desc)
{
   BPLib_STOR_CACHE_SocketInfo_t *sock;
    BPLib_STOR_CACHE_Ref_t    sock_ref;
    BPLib_STOR_CACHE_Ref_t    detached_ref;

    sock_ref = (BPLib_STOR_CACHE_Ref_t)desc;

    sock = BPLib_MEM_GenericDataCast((BPLib_MEM_Block_t *)BPLib_STOR_CACHE_Dereference(sock_ref), BPLIB_BLOCKTYPE_SERVICE_SOCKET);
    if (sock == NULL)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "%s(): bad descriptor\n", __func__);
        return;
    }

    if (sock->params.local_ipn.node_number != 0)
    {
        detached_ref =BPLib_STOR_CACHE_DataserviceDetach(sock->parent_rtbl, &sock->params.local_ipn);
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
}

int BPLib_STOR_CACHE_Send(bp_socket_t *desc, const void *payload, size_t size, uint32_t timeout)
{
    int                           status;
    BPLib_STOR_CACHE_Block_t          *rblk;
    BPLib_STOR_QM_Duct_t           *duct;
    BPLib_STOR_CACHE_Ref_t             refptr;
    BPLib_STOR_CACHE_Block_t          *pblk;
    BPLib_STOR_CACHE_BblockPrimary_t *pri_block;
    BPLib_STOR_CACHE_Ref_t             sock_ref;
   BPLib_STOR_CACHE_SocketInfo_t          *sock;
    BPLib_TIME_MonotonicTime_t              ingress_time;
    BPLib_TIME_MonotonicTime_t              ingress_limit;

    sock_ref      = (BPLib_STOR_CACHE_Ref_t)desc;
    ingress_time  = BPLib_STOR_CACHE_GetMonotonicTime();
    ingress_limit.Time = ingress_time.Time + timeout; // TODO Exist? BPLib_*_TimeAddMs(ingress_time, timeout);

    sock = BPLib_MEM_GenericDataCast((BPLib_MEM_Block_t *)BPLib_STOR_CACHE_Dereference(sock_ref), BPLIB_BLOCKTYPE_SERVICE_SOCKET);
    if (sock == NULL)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "%s(): bad descriptor\n", __func__);
        return BPLIB_ERROR;
    }

    duct = BPLib_STOR_QM_DuctCast(BPLib_STOR_CACHE_Dereference(sock_ref));
    if (duct == NULL)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "%s(): bad duct ref - is socket connected?\n", __func__);
        return BPLIB_ERROR;
    }

    /* If no pri block is available, this should block and wait for one (up to ingress_limit) */
    pblk = BPLib_STOR_CACHE_BblockPrimaryAlloc(BPLib_STOR_QM_GetQtblPool(sock->parent_rtbl), 0, NULL, BPLIB_MPOOL_ALLOC_PRI_LO,
                                            ingress_limit);
    if (pblk == NULL)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "%s(): unable to alloc pri block\n", __func__);
        return BPLIB_TIMEOUT;
    }

    status =BPLib_STOR_CACHE_ServiceductBundleizePayload(sock, pblk, payload, size);
    if (status != BPLIB_SUCCESS)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "%s(): cannot bundleize payload, out of memory?\n", __func__);
        return status;
    }

    /* convert to a dynamically-managed ref for passing in queues */
    refptr = BPLib_STOR_CACHE_RefCreate(pblk);
    if (refptr == NULL)
    {
        /* not expected... */
        BPLib_STOR_CACHE_RecycleBlock(pblk);
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "Cannot convert payload to ref\n");
        return BPLIB_ERROR;
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

        if (BPLib_STOR_QM_DuctTryPush(&duct->ingress, rblk, ingress_limit))
        {
            sock->ingress_byte_count += size;
            status = BPLIB_SUCCESS;
        }
        else
        {
            status = BPLIB_TIMEOUT;
        }
    }
    else
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "%s(): failed to create block ref, out of memory?\n", __func__);
        status = BPLIB_ERROR;
    }

    /*
     * JPHFIX - to implement a timeout, this should wait/confirm here that the bundle either
     * reached a storage (for custody-tracked) or made it to the next hop CLA (for best-effort svc level)
     */

    BPLib_STOR_CACHE_RefRelease(refptr);

    BPLib_STOR_QM_SetMaintenanceRequest(sock->parent_rtbl);

    return status;
}

int BPLib_STOR_CACHE_Recv(bp_socket_t *desc, void *payload, size_t *size, uint32_t timeout)
{
    int                           status;
   BPLib_STOR_CACHE_SocketInfo_t          *sock;
    BPLib_STOR_CACHE_BblockPrimary_t *pri_block;
    BPLib_STOR_CACHE_Block_t          *pblk;
    BPLib_STOR_CACHE_Ref_t             refptr;
    BPLib_STOR_CACHE_Ref_t             sock_ref;
    BPLib_STOR_QM_Duct_t           *duct;
    BPLib_TIME_MonotonicTime_t              egress_time_limit;

    sock_ref = (BPLib_STOR_CACHE_Ref_t)desc;

    sock = BPLib_MEM_GenericDataCast((BPLib_MEM_Block_t *)BPLib_STOR_CACHE_Dereference(sock_ref), BPLIB_BLOCKTYPE_SERVICE_SOCKET);
    if (sock == NULL)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "%s(): bad descriptor\n", __func__);
        return BPLIB_ERROR;
    }

    duct = BPLib_STOR_QM_DuctCast(BPLib_STOR_CACHE_Dereference(sock_ref));
    if (duct == NULL)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "%s(): bad duct_ref\n", __func__);
        return BPLIB_ERROR;
    }

    /* preemptively trigger the maintenance task to run */
    /* this may help in the event that the queue is currently empty but
     * there is data somewhere else in the pool that is headed here (assuming
     * timeout is nonzero). */
    BPLib_STOR_QM_SetMaintenanceRequest(sock->parent_rtbl);

    egress_time_limit = BPLib_STOR_CACHE_TimeRelativeToAbsolute(timeout);

    pblk = BPLib_STOR_QM_DuctTryPull(&duct->egress, egress_time_limit);

    if (pblk == NULL)
    {
        status = BPLIB_TIMEOUT;
    }
    else
    {
        refptr = BPLib_STOR_CACHE_RefFromBlock(pblk);

        if (refptr != NULL)
        {
            /* note, the unbundleize always consumes the refptr */
            status =BPLib_STOR_CACHE_ServiceductUnbundleizePayload(sock, refptr, payload, size);
        }
        else
        {
            // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "%s(): unable to create ref from pblk\n", __func__);
            status = BPLIB_ERROR;
        }

        /* if a block was pulled from the queue, that needs to be recycled */
        /* this should always be the case */
        if (status == BPLIB_SUCCESS)
        {
            sock->egress_byte_count += *size;

            pri_block = BPLib_STOR_CACHE_BblockPrimaryCast(pblk);
            if (pri_block != NULL)
            {
                pri_block->data.delivery.egress_intf_id =
                    BPLib_STOR_CACHE_GetExternalId(BPLib_STOR_CACHE_Dereference(sock_ref));
                pri_block->data.delivery.egress_time = BPLib_STOR_CACHE_GetMonotonicTime();
            }
        }

        BPLib_STOR_CACHE_RecycleBlock(pblk);
    }

    return status;
}
