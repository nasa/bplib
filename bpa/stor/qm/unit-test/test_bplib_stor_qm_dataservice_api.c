/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/*
 * Includes
 */
#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"

#include "bplib_stor_cache_block.h"

#include "bplib_stor_qm.h"
#include "bplib_stor_qm_dataservice.h"

#include "test_BPLib_STOR_CACHE_StorQm.h"

void Test_BPLib_STOR_CACHE_DataserviceAddBaseIntf(void)
{
    /* Test function for:
     * bp_handle_t BPLib_STOR_CACHE_DataserviceAddBaseIntf(BPLib_STOR_QM_QueueTbl_t *rtbl, bp_ipn_t node_number)
     */
    BPLib_STOR_QM_QueueTbl_t    rtbl;
    bp_ipn_t            node_number = 101;
    BPLib_STOR_CACHE_Block_t sblk;

    memset(&rtbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctAlloc), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_DataserviceAddBaseIntf(&rtbl, node_number).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_DataserviceAddBaseIntf(&rtbl, node_number).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctAlloc), UT_lib_AltHandler_PointerReturn, &sblk);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_DataserviceAddBaseIntf(&rtbl, node_number).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctAlloc), UT_lib_AltHandler_PointerReturn, NULL);
}

void Test_BPLib_STOR_CACHE_DataserviceAttach(void)
{
    /* Test function for:
     * bp_handle_t BPLib_STOR_CACHE_DataserviceAttach(BPLib_STOR_QM_QueueTbl_t *tbl, const bp_ipn_addr_t *ipn,BPLib_STOR_CACHE_DataserviceType_t
     * type, BPLib_STOR_CACHE_Ref_t blkref)
     */
    BPLib_STOR_QM_QueueTbl_t            rtbl;
    bp_ipn_addr_t               ipn;
   BPLib_STOR_CACHE_DataserviceType_t    type = BPLib_STOR_CACHE_DataserviceTypeStorage;
    BPLib_STOR_CACHE_BlockContent_t blkref;
    BPLib_STOR_CACHE_Block_t         sblk;
    BPLib_STOR_CACHE_Ref_t           duct_ref;
    BPLib_STOR_CACHE_Block_t         temp_block;

    memset(&rtbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    memset(&ipn, 0, sizeof(bp_ipn_addr_t));
    memset(&blkref, 0, sizeof(BPLib_STOR_CACHE_BlockContent_t));
    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&duct_ref, 0, sizeof(BPLib_STOR_CACHE_Ref_t));
    memset(&temp_block, 0, sizeof(BPLib_STOR_CACHE_Block_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_DataserviceAttach(&rtbl, &ipn, type, &blkref).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, &sblk);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_DataserviceAttach(&rtbl, &ipn, type, &blkref).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataAlloc), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, &duct_ref);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_DataserviceAttach(&rtbl, &ipn, type, &blkref).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, &duct_ref);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataAlloc), UT_lib_AltHandler_PointerReturn, &temp_block);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefDuplicate), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctModifyFlags), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctModifyFlags), UT_lib_int8_Handler, NULL);
    UtAssert_UINT32_GT(BPLib_STOR_CACHE_DataserviceAttach(&rtbl, &ipn, type, &blkref).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataAlloc), UT_lib_AltHandler_PointerReturn, NULL);
}

void test BPLib_STOR_CACHE_DataserviceDetach(void)
{
    /* Test function for:
     * BPLib_STOR_CACHE_Ref_t BPLib_STOR_CACHE_DataserviceDetach(BPLib_STOR_QM_QueueTbl_t *tbl, const bp_ipn_addr_t *ipn)
     */
    BPLib_STOR_QM_QueueTbl_t    rtbl;
    bp_ipn_addr_t       ipn;
    BPLib_STOR_CACHE_Ref_t   duct_ref;
    BPLib_MEM_RBT_Link_t    rbt_link;
    BPLib_STOR_CACHE_Block_t temp_block;

    memset(&rtbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    ipn.node_number    = 10;
    ipn.service_number = 100;
    memset(&ipn, 0, sizeof(bp_ipn_addr_t));
    memset(&duct_ref, 0, sizeof(BPLib_STOR_CACHE_Ref_t));
    memset(&rbt_link, 0, sizeof(BPLib_MEM_RBT_Link_t));
    memset(&temp_block, 0, sizeof(BPLib_STOR_CACHE_Block_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_NULL BPLib_STOR_CACHE_DataserviceDetach(&rtbl, &ipn));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, &duct_ref);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_RBT_SearchGeneric), UT_lib_AltHandler_PointerReturn, &rbt_link);
    UtAssert_NULL BPLib_STOR_CACHE_DataserviceDetach(&rtbl, &ipn));

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, &duct_ref);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataAlloc), UT_lib_AltHandler_PointerReturn, &temp_block);
    UtAssert_NULL BPLib_STOR_CACHE_DataserviceDetach(&rtbl, &ipn));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_RBT_SearchGeneric), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataAlloc), UT_lib_AltHandler_PointerReturn, NULL);
}

void Test_BPLib_STOR_CACHE_ConnectSocket(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_ConnectSocket(bp_socket_t *desc, const bp_ipn_addr_t *destination_ipn)
     */
    bp_socket_t         desc;
    bp_ipn_addr_t       destination_ipn;
   BPLib_STOR_CACHE_SocketInfo_t sock;
    BPLib_STOR_QM_QueueTbl_t    rbtl;

    memset(&desc, 0, sizeof(bp_socket_t));
    memset(&destination_ipn, 0, sizeof(bp_ipn_addr_t));
    destination_ipn.node_number    = 10;
    destination_ipn.service_number = 11;
    memset(&sock, 0, sizeof(BPLib_STOR_CACHE_SocketInfo_t));
    memset(&rbtl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    sock.params.local_ipn.node_number    = 10;
    sock.params.local_ipn.service_number = 11;
    sock.parent_rtbl                     = &rbtl;

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_ConnectSocket(&desc, &destination_ipn), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, &sock);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_ConnectSocket(&desc, &destination_ipn), 0);

    memset(&sock.socket_intf_id, 0, sizeof(sock.socket_intf_id));
    sock.socket_intf_id.hdl = 20;
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_ConnectSocket(&desc, &destination_ipn), 0);

    sock.params.remote_ipn.node_number = 1;
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_ConnectSocket(&desc, &destination_ipn), 0);

    destination_ipn.node_number        = 2;
    sock.params.remote_ipn.node_number = 0;
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_ConnectSocket(&desc, &destination_ipn), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void Test_BPLib_STOR_CACHE_CreateSocket(void)
{
    /* Test function for:
     * bp_socket_t BPLib_STOR_CACHE_CreateSocket(BPLib_STOR_QM_QueueTbl_t *rtbl)
     */
    BPLib_STOR_QM_QueueTbl_t    rtbl;
    BPLib_STOR_CACHE_Block_t sblk;
   BPLib_STOR_CACHE_SocketInfo_t sock;

    memset(&rtbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&sock, 0, sizeof(BPLib_STOR_CACHE_SocketInfo_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctAlloc), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_NULL(BPLib_STOR_CACHE_CreateSocket(&rtbl));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctAlloc), UT_lib_AltHandler_PointerReturn, &sblk);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, &sock);
    UtAssert_NULL(BPLib_STOR_CACHE_CreateSocket(&rtbl));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctAlloc), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void Test_BPLib_STOR_CACHE_BindSocket(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_BindSocket(bp_socket_t *desc, const bp_ipn_addr_t *source_ipn)
     */
    bp_socket_t         desc;
    bp_ipn_addr_t       source_ipn;
   BPLib_STOR_CACHE_SocketInfo_t sock;
    BPLib_STOR_QM_QueueTbl_t    rtbl;
    BPLib_STOR_CACHE_Block_t sblk;
    BPLib_STOR_CACHE_Ref_t   duct_ref;

    memset(&desc, 0, sizeof(bp_socket_t));
    memset(&source_ipn, 0, sizeof(bp_ipn_addr_t));
    memset(&sock, 0, sizeof(BPLib_STOR_CACHE_SocketInfo_t));
    memset(&rtbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    sock.parent_rtbl = &rtbl;
    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&duct_ref, 0, sizeof(BPLib_STOR_CACHE_Ref_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_BindSocket(&desc, &source_ipn), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, &sock);
    sock.params.local_ipn.node_number = 1;
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_BindSocket(&desc, &source_ipn), 0);

    source_ipn.service_number         = 0;
    sock.params.local_ipn.node_number = 0;
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_BindSocket(&desc, &source_ipn), 0);

    sock.params.local_ipn.node_number = 0;
    source_ipn.node_number            = 1;
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_BindSocket(&desc, &source_ipn), 0);

    UT_SetDefaultReturnValue(UT_KEY(BPLib_MEM_RBT_ExtractNode), BP_SUCCESS);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, &sblk);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, &sblk);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, &duct_ref);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, &sock);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataAlloc), UT_lib_AltHandler_PointerReturn, &sblk);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_BindSocket(&desc, &source_ipn), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataAlloc), UT_lib_AltHandler_PointerReturn, NULL);
}

void Test_BPLib_STOR_CACHE_CloseSocket(void)
{
    /* Test function for:
     * void_BPLib_STOR_CACHE_CloseSocket(bp_socket_t *desc)
     */
    bp_socket_t                    desc;
   BPLib_STOR_CACHE_SocketInfo_t            sock;
    BPLib_STOR_QM_QueueTbl_t               rtbl;
    BPLib_STOR_QM_ServiceintfInfo_t base_intf;
    BPLib_STOR_CACHE_Ref_t              ref;

    memset(&desc, 0, sizeof(bp_socket_t));
    memset(&sock, 0, sizeof(BPLib_STOR_CACHE_SocketInfo_t));
    sock.params.local_ipn.node_number    = 10;
    sock.params.local_ipn.service_number = 11;
    memset(&rtbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    sock.parent_rtbl = &rtbl;
    memset(&base_intf, 0, sizeof(BPLib_STOR_QM_ServiceintfInfo_t));
    memset(&ref, 0, sizeof(BPLib_STOR_CACHE_Ref_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_CloseSocket(&desc));

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, &sock);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_MEM_RBT_ExtractNode), BP_SUCCESS);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_CloseSocket(&desc));

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Send(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_Send(bp_socket_t *desc, const void *payload, size_t size, uint32_t timeout)
     */
    bp_socket_t                    desc;
    void                          *payload = NULL;
    size_t                         size    = 100;
    uint32_t                       timeout = 3000;
   BPLib_STOR_CACHE_SocketInfo_t            sock;
    BPLib_STOR_QM_QueueTbl_t               rtbl;
    BPLib_STOR_QM_Duct_t             duct;
    BPLib_STOR_CACHE_Block_t            blk;
    BPLib_STOR_CACHE_Ref_t              refptr;
    BPLib_STOR_CACHE_BblockPrimary_t   pri;
    BPLib_STOR_CACHE_BblockCanonical_t ccb_pay;

    memset(&desc, 0, sizeof(bp_socket_t));
    memset(&sock, 0, sizeof(BPLib_STOR_CACHE_SocketInfo_t));
    memset(&rtbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    sock.parent_rtbl = &rtbl;
    memset(&duct, 0, sizeof(BPLib_STOR_QM_Duct_t));
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&refptr, 0, sizeof(BPLib_STOR_CACHE_Ref_t));
    memset(&pri, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));
    memset(&ccb_pay, 0, sizeof(BPLib_STOR_CACHE_BblockCanonical_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Send(&desc, payload, size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, &sock);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Send(&desc, payload, size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, &duct);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Send(&desc, payload, size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GetCurrentTime), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalAlloc), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctTryPush), UT_lib_int8_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryAlloc), UT_lib_AltHandler_PointerReturn, &blk);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Send(&desc, payload, size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_lib_AltHandler_PointerReturn, &pri);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Send(&desc, payload, size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_lib_AltHandler_PointerReturn, &ccb_pay);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Send(&desc, payload, size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, &refptr);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Send(&desc, payload, size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefMakeBlock), UT_lib_AltHandler_PointerReturn, &blk);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Send(&desc, payload, size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctTryPush), UT_lib_bool_Handler, NULL);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Send(&desc, payload, size, timeout), 0);

    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_BlockEncodePri), -1);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Send(&desc, payload, size, timeout), 0);

    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_BlockEncodePri), 1);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_BlockEncodePay), -1);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalAlloc), UT_lib_AltHandler_PointerReturn, &blk);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Send(&desc, payload, size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryAlloc), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefMakeBlock), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalAlloc), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Recv(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_Recv(bp_socket_t *desc, void *payload, size_t *size, uint32_t timeout)
     */
    bp_socket_t                    desc;
    void                          *payload = NULL;
    size_t                         size    = 100;
    uint32_t                       timeout = 3000;
   BPLib_STOR_CACHE_SocketInfo_t            sock;
    BPLib_STOR_QM_Duct_t             duct;
    BPLib_STOR_QM_QueueTbl_t               rtbl;
    BPLib_STOR_CACHE_Block_t            blk;
    BPLib_STOR_CACHE_Ref_t              refptr;
    BPLib_STOR_CACHE_BblockCanonical_t ccb_pay;
    BPLib_STOR_CACHE_BblockPrimary_t   pri;

    memset(&desc, 0, sizeof(bp_socket_t));
    memset(&sock, 0, sizeof(BPLib_STOR_CACHE_SocketInfo_t));
    memset(&duct, 0, sizeof(BPLib_STOR_QM_Duct_t));
    memset(&rtbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    sock.parent_rtbl = &rtbl;
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&refptr, 0, sizeof(BPLib_STOR_CACHE_Ref_t));
    memset(&ccb_pay, 0, sizeof(BPLib_STOR_CACHE_BblockCanonical_t));
    memset(&pri, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Recv(&desc, payload, &size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, &sock);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Recv(&desc, payload, &size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefFromBlock), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, &duct);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Recv(&desc, payload, &size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctTryPull), UT_lib_AltHandler_PointerReturn, &blk);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Recv(&desc, payload, &size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryLocateCanonical), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefFromBlock), UT_lib_AltHandler_PointerReturn, &refptr);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Recv(&desc, payload, &size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryLocateCanonical), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_lib_AltHandler_PointerReturn, &pri);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Recv(&desc, payload, &size, timeout), 0);

    timeout                        = 0;
    ccb_pay.encoded_content_offset = 1;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_lib_AltHandler_PointerReturn, &ccb_pay);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCborExport), UT_lib_sizet_Handler, NULL);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Recv(&desc, payload, &size, timeout), 0);

    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_BblockCborExport), 1);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCborExport), UT_lib_uint64_Handler, NULL);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Recv(&desc, payload, &size, timeout), 0);

    ccb_pay.encoded_content_offset = 0;
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Recv(&desc, payload, &size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctTryPull), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefFromBlock), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void Test_BPLib_STOR_CACHE_ServiceductForwardIngress(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_ServiceductForwardIngress(void *arg, BPLib_STOR_CACHE_Block_t *subq_src)
     */
    BPLib_STOR_QM_QueueTbl_t    tbl;
    BPLib_STOR_CACHE_Block_t subq_src;
    BPLib_STOR_CACHE_Block_t sblk;

    memset(&tbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    memset(&subq_src, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GetBlockFromLink), UT_lib_sizet_Handler, NULL);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_ServiceductForwardIngress(&tbl, &subq_src), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, &sblk);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_ServiceductForwardIngress(&tbl, &subq_src), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, &sblk);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_ServiceductForwardIngress(&tbl, &subq_src), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctTryPull), UT_lib_ingress_AltHandler_PointerReturn, &sblk);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_ServiceductForwardIngress(&tbl, &subq_src), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctTryPull), UT_lib_AltHandler_PointerReturn, NULL);
}

void Test_BPLib_STOR_CACHE_ServiceductForwardEgress(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_ServiceductForwardEgress(void *arg, BPLib_STOR_CACHE_Block_t *subq_src)
     */
    BPLib_STOR_QM_QueueTbl_t               tbl;
    BPLib_STOR_CACHE_Block_t            subq_src;
    BPLib_STOR_CACHE_Block_t            sblk;
    BPLib_STOR_CACHE_Block_t            sblk1;
    BPLib_STOR_QM_ServiceintfInfo_t base_intf;

    memset(&tbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    memset(&subq_src, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&sblk1, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&base_intf, 0, sizeof(BPLib_STOR_QM_ServiceintfInfo_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GetBlockFromLink), UT_lib_sizet_Handler, NULL);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_ServiceductForwardEgress(&tbl, &subq_src), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, &base_intf);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_ServiceductForwardEgress(&tbl, &subq_src), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, &sblk);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_ServiceductForwardEgress(&tbl, &subq_src), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctTryPull), UT_lib_egress_AltHandler_PointerReturn, &sblk);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_lib_AltHandler_PointerReturn, &sblk1);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_ServiceductForwardEgress(&tbl, &subq_src), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctTryPull), UT_lib_AltHandler_PointerReturn, NULL);
}

void Test_BPLib_STOR_CACHE_DataserviceEventImpl(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_DataserviceEventImpl(void *arg, BPLib_STOR_CACHE_Block_t *intf_block)
     */
    BPLib_STOR_QM_DuctGenericEvent_t event;
    BPLib_STOR_CACHE_Block_t              intf_block;
    BPLib_STOR_QM_Duct_t               duct;

    memset(&event, 0, sizeof(BPLib_STOR_QM_DuctGenericEvent_t));
    memset(&intf_block, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&event.intf_state.intf_id, 0, sizeof(bp_handle_t));
    memset(&duct, 0, sizeof(BPLib_STOR_QM_Duct_t));

    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_DataserviceEventImpl(&event, &intf_block), 0);

    event.event_type = BPLib_STOR_QM_DuctEventUp;
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_DataserviceEventImpl(&event, &intf_block), 0);

    event.intf_state.intf_id = BPLIB_HANDLE_MPOOL_BASE;
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_DataserviceEventImpl(&event, &intf_block), 0);

    duct.current_state_flags = BPLIB_MEM_FLOW_FLAGS_ENDPOINT;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, &duct);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_DataserviceEventImpl(&event, &intf_block), 0);

    event.event_type = BPLib_STOR_QM_DuctEventDown;
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_DataserviceEventImpl(&event, &intf_block), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void Test_BPLib_STOR_CACHE_DataserviceBaseConstruct(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_DataserviceBaseConstruct(void *arg, BPLib_STOR_CACHE_Block_t *blk)
     */
    void                          *arg = NULL;
    BPLib_STOR_CACHE_Block_t            blk;
    BPLib_STOR_QM_ServiceintfInfo_t base_intf;

    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&base_intf, 0, sizeof(BPLib_STOR_QM_ServiceintfInfo_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_GT(BPLib_STOR_CACHE_DataserviceBaseConstruct(arg, &blk), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, &base_intf);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_DataserviceBaseConstruct(arg, &blk), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void Test_BplibCache_DataServiceApi_Register(void)
{
    UtTest_Add(testBPLib_STOR_CACHE_DataserviceAddBaseIntf, NULL, NULL, "Test_BPLib_STOR_CACHE_DataserviceAddBaseIntf");
    UtTest_Add(testBPLib_STOR_CACHE_DataserviceAttach, NULL, NULL, "Test_BPLib_STOR_CACHE_DataserviceAttach");
    UtTest_Add(test BPLib_STOR_CACHE_DataserviceDetach, NULL, NULL, "Test BPLib_STOR_CACHE_DataserviceDetach");
    UtTest_Add(testBPLib_STOR_CACHE_ConnectSocket, NULL, NULL, "Test_BPLib_STOR_CACHE_ConnectSocket");
    UtTest_Add(testBPLib_STOR_CACHE_CreateSocket, NULL, NULL, "Test_BPLib_STOR_CACHE_CreateSocket");
    UtTest_Add(testBPLib_STOR_CACHE_BindSocket, NULL, NULL, "Test_BPLib_STOR_CACHE_BindSocket");
    UtTest_Add(testBPLib_STOR_CACHE_CloseSocket, NULL, NULL, "Test_BPLib_STOR_CACHE_CloseSocket");
    UtTest_Add(test_BPLib_STOR_CACHE_Send, NULL, NULL, "Test BPLib_STOR_CACHE_Send");
    UtTest_Add(test_BPLib_STOR_CACHE_Recv, NULL, NULL, "Test BPLib_STOR_CACHE_Recv");
    UtTest_Add(testBPLib_STOR_CACHE_ServiceductForwardIngress, NULL, NULL, "Test_BPLib_STOR_CACHE_ServiceductForwardIngress");
    UtTest_Add(testBPLib_STOR_CACHE_ServiceductForwardEgress, NULL, NULL, "Test_BPLib_STOR_CACHE_ServiceductForwardEgress");
    UtTest_Add(testBPLib_STOR_CACHE_DataserviceEventImpl, NULL, NULL, "Test_BPLib_STOR_CACHE_DataserviceEventImpl");
    UtTest_Add(testBPLib_STOR_CACHE_DataserviceBaseConstruct, NULL, NULL, "Test_BPLib_STOR_CACHE_DataserviceBaseConstruct");
}
