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

#include "bplib_api_types.h"

#include "bplib_time.h"

#include "bplib_mem.h"

#include "bplib_stor_cache_types.h"
#include "bplib_stor_cache_internal.h"
#include "bplib_stor_cache_base_internal.h"
#include "bplib_stor_cache_block.h"
#include "bplib_stor_cache_ref.h"

#include "bplib_stor_qm.h"

#define BPLIB_BLOCKTYPE_CLA_INTF          0x7b643c85
#define BPLIB_BLOCKTYPE_CLA_INGRESS_BLOCK 0x9580be4a

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/
int BPLib_STOR_CACHE_ClaEventImpl(void *arg, BPLib_STOR_CACHE_Block_t *intf_block)
{
    #ifdef QM
    BPLib_STOR_QM_DuctGenericEvent_t   *event;
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
        BPLib_STOR_QM_DuctEnable(&duct->ingress, BPLIB_MAX_SUBQ_DEPTH);
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
    #endif // QM
    return BPLIB_SUCCESS;
}

#ifdef QM
int BPLib_STOR_CACHE_GenericBundleIngress(BPLib_STOR_CACHE_Ref_t duct_ref, const void *content, size_t size, BPLib_TIME_MonotonicTime_t time_limit)
{
    BPLib_STOR_QM_Duct_t           *duct;
    BPLib_STOR_CACHE_Block_t          *pblk;
    BPLib_STOR_CACHE_Block_t          *rblk;
    BPLib_STOR_CACHE_Ref_t             refptr;
    BPLib_STOR_CACHE_BblockPrimary_t *pri_block;
    size_t                        imported_sz;
    int                           status;

    pblk = NULL;
    duct = BPLib_STOR_QM_DuctCast(BPLib_STOR_CACHE_Dereference(duct_ref));
    if (duct == NULL)
    {
        status = bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "intf_block invalid\n");
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
            BPLib_STOR_CACHE_BblockPrimaryAlloc(BPLib_STOR_CACHE_GetParentPoolFromLink(BPLib_STOR_CACHE_Dereference(duct_ref)),
                                             0, NULL, BPLIB_MPOOL_ALLOC_PRI_MHI, BPLIB_MONOTIME_ZERO);
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
            pri_block->data.delivery.ingress_intf_id   = BPLib_STOR_CACHE_GetExternalId(BPLib_STOR_CACHE_Dereference(duct_ref));

            pri_block->data.delivery.ingress_time  = BPLib_STOR_CACHE_GetMonotonicTime();

            if (BPLib_STOR_QM_DuctTryPush(&duct->ingress, rblk, time_limit))
            {
                status = BPLIB_SUCCESS;
            }
            else
            {
                BPLib_STOR_CACHE_RecycleBlock(rblk);
                status = BPLIB_TIMEOUT;
            }
        }
        else
        {
            // TODO remove bplog(NULL, BPLIB_FLAG_INCOMPLETE, "Bundle did not decode correctly\n");
            status = BPLIB_ERROR;
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
}

int BPLib_STOR_CACHE_GenericBundleEgress(BPLib_STOR_CACHE_Ref_t duct_ref, void *content, size_t *size, BPLib_TIME_MonotonicTime_t time_limit)
{
    BPLib_STOR_QM_Duct_t           *duct;
    BPLib_STOR_CACHE_BblockPrimary_t *cpb;
    BPLib_STOR_CACHE_Block_t          *pblk;
    size_t                        export_sz;
    int                           status;

    pblk = NULL;
    duct = BPLib_STOR_QM_DuctCast(BPLib_STOR_CACHE_Dereference(duct_ref));
    if (duct == NULL)
    {
        status = bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "intf_block invalid\n");
    }
    else
    {
        /* this removes it from the list */
        /* NOTE: after this point a valid bundle has to be put somewhere (either onto another queue or recycled) */
        pblk = BPLib_STOR_QM_DuctTryPull(&duct->egress, time_limit);
        if (pblk == NULL)
        {
            /* queue is empty */
            status = BPLIB_TIMEOUT;
        }
        else
        {
            cpb = BPLib_STOR_CACHE_BblockPrimaryCast(pblk);
            if (cpb == NULL)
            {
                /* entry wasn't a bundle? */
                status = BPLIB_ERROR;
            }
            else
            {
                export_sz = BPLib_STOR_CACHE_ComputeFullBundleSize(cpb);

                if (export_sz > *size)
                {
                    /* buffer too small */
                    status = BPLIB_ERROR;
                }
                else
                {

                    *size = BPLib_STOR_CACHE_CopyFullBundleOut(cpb, content, *size);

                    if (export_sz != *size)
                    {
                        /* something went wrong during copy */
                        status = BPLIB_ERROR;
                    }
                    else
                    {
                        /* indicate that this has been sent out the intf */
                        cpb->data.delivery.egress_intf_id =
                            BPLib_STOR_CACHE_GetExternalId(BPLib_STOR_CACHE_Dereference(duct_ref));
                        cpb->data.delivery.egress_time = BPLib_STOR_CACHE_GetMonotonicTime();

                        status = BPLIB_SUCCESS;
                    }
                }
            }

            BPLib_STOR_CACHE_RecycleBlock(pblk);
        }
    }

    return status;
}

void BPLib_STOR_CACHE_ClaInit(BPLib_STOR_CACHE_Pool_t *pool)
{
    BPLib_STOR_CACHE_RegisterBlocktype(pool, BPLIB_BLOCKTYPE_CLA_INTF, NULL, sizeof (BPLib_STOR_CACHE_ClaStats_t));
    BPLib_STOR_CACHE_RegisterBlocktype(pool, BPLIB_BLOCKTYPE_CLA_INGRESS_BLOCK, NULL, 0);
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

bp_handle_t BPLib_STOR_CACHE_CreateClaIntf(BPLib_STOR_QM_QueueTbl_t *rtbl)
{
    BPLib_STOR_CACHE_Block_t *sblk;
    bp_handle_t          self_intf_id;
    BPLib_STOR_CACHE_Pool_t       *pool;

    pool = BPLib_STOR_QM_GetQtblPool(rtbl);

    /* register CLA API module */
   BPLib_STOR_CACHE_ClaInit(pool);

    /* Allocate Blocks */
    sblk = BPLib_STOR_QM_DuctAlloc(pool, BPLIB_BLOCKTYPE_CLA_INTF, NULL);
    if (sblk == NULL)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_OUT_OF_MEMORY, "Failed to allocate intf block\n");
        return BP_INVALID_HANDLE;
    }

    self_intf_id = BPLib_STOR_QM_RegisterGenericIntf(rtbl, BP_INVALID_HANDLE, sblk);
    if (bp_handle_is_valid(self_intf_id))
    {
        BPLib_STOR_QM_RegisterForwardIngressHandler(rtbl, self_intf_id, BPLib_STOR_QM_IngressBaseintfForwarder);
        BPLib_STOR_QM_RegisterEventHandler(rtbl, self_intf_id,BPLib_STOR_CACHE_ClaEventImpl);
    }
    else
    {
        BPLib_STOR_CACHE_RecycleBlock(sblk);
    }

    return self_intf_id;
}

int BPLib_STOR_CACHE_ClaEgress(BPLib_STOR_QM_QueueTbl_t *rtbl, bp_handle_t intf_id, void *bundle, size_t *size, uint32_t timeout)
{
    BPLib_STOR_CACHE_Ref_t  duct_ref;
    int                status;
   BPLib_STOR_CACHE_ClaStats_t *stats;
    BPLib_TIME_MonotonicTime_t   egress_time_limit;

    /* preemptively trigger the maintenance task to run */
    /* this may help in the event that the queue is currently empty but
     * there is data somewhere else in the pool that is headed here (assuming
     * timeout is nonzero). */
    BPLib_STOR_QM_SetMaintenanceRequest(rtbl);

    egress_time_limit = BPLib_STOR_CACHE_TimeRelativeToAbsolute(timeout);

    duct_ref = BPLib_STOR_QM_GetIntfControlblock(rtbl, intf_id);
    if (duct_ref == NULL)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "Intf ID invalid\n");
        return BPLIB_ERROR;
    }

    stats = BPLib_MEM_GenericDataCast((BPLib_MEM_Block_t *)BPLib_STOR_CACHE_Dereference(duct_ref), BPLIB_BLOCKTYPE_CLA_INTF);
    if (stats == NULL)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "Intf ID is not a CLA\n");
        status = BPLIB_ERROR;
    }
    else
    {
        status =BPLib_STOR_CACHE_GenericBundleEgress(duct_ref, bundle, size, egress_time_limit);
        if (status == BPLIB_SUCCESS)
        {
            stats->egress_byte_count += *size;
        }
    }

    BPLib_STOR_QM_ReleaseIntfControlblock(rtbl, duct_ref);

    return status;
}

int BPLib_STOR_CACHE_ClaIngress(BPLib_STOR_QM_QueueTbl_t *rtbl, bp_handle_t intf_id, const void *bundle, size_t size, uint32_t timeout)
{
    BPLib_STOR_CACHE_Ref_t  duct_ref;
    int                status;
   BPLib_STOR_CACHE_ClaStats_t *stats;
    BPLib_TIME_MonotonicTime_t   ingress_time_limit;

    duct_ref = BPLib_STOR_QM_GetIntfControlblock(rtbl, intf_id);
    if (duct_ref == NULL)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "Intf ID invalid\n");
        return BPLIB_ERROR;
    }

    stats = BPLib_MEM_GenericDataCast((BPLib_MEM_Block_t *)BPLib_STOR_CACHE_Dereference(duct_ref), BPLIB_BLOCKTYPE_CLA_INTF);
    if (stats == NULL)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "Intf ID is not a CLA\n");
        status = BPLIB_ERROR;
    }
    else
    {
        ingress_time_limit = BPLib_STOR_CACHE_TimeRelativeToAbsolute(timeout);

        status =BPLib_STOR_CACHE_GenericBundleIngress(duct_ref, bundle, size, ingress_time_limit);

        if (status == BPLIB_SUCCESS)
        {
            stats->ingress_byte_count += size;
        }
    }

    BPLib_STOR_QM_ReleaseIntfControlblock(rtbl, duct_ref);

    /* trigger the maintenance task to run */
    BPLib_STOR_QM_SetMaintenanceRequest(rtbl);

    return status;
}
#endif // QM
