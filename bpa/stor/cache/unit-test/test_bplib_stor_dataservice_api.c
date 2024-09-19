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
#include "bplib_dataservice.h"
#include "test_bplib_base.h"

void test_bplib_dataservice_add_base_intf(void)
{
    /* Test function for:
     * bp_handle_t bplib_dataservice_add_base_intf(bplib_routetbl_t *rtbl, bp_ipn_t node_number)
     */
    bplib_routetbl_t    rtbl;
    bp_ipn_t            node_number = 101;
    BPLib_STOR_CACHE_Block_t sblk;

    memset(&rtbl, 0, sizeof(bplib_routetbl_t));
    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowAlloc), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_EQ(bplib_dataservice_add_base_intf(&rtbl, node_number).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_EQ(bplib_dataservice_add_base_intf(&rtbl, node_number).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowAlloc), UT_lib_AltHandler_PointerReturn, &sblk);
    UtAssert_UINT32_EQ(bplib_dataservice_add_base_intf(&rtbl, node_number).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowAlloc), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_dataservice_attach(void)
{
    /* Test function for:
     * bp_handle_t bplib_dataservice_attach(bplib_routetbl_t *tbl, const bp_ipn_addr_t *ipn, bplib_dataservice_type_t
     * type, BPLib_STOR_CACHE_Ref_t blkref)
     */
    bplib_routetbl_t            rtbl;
    bp_ipn_addr_t               ipn;
    bplib_dataservice_type_t    type = bplib_dataservice_type_storage;
    BPLib_STOR_CACHE_BlockContent_t blkref;
    BPLib_STOR_CACHE_Block_t         sblk;
    BPLib_STOR_CACHE_Ref_t           flow_ref;
    BPLib_STOR_CACHE_Block_t         temp_block;

    memset(&rtbl, 0, sizeof(bplib_routetbl_t));
    memset(&ipn, 0, sizeof(bp_ipn_addr_t));
    memset(&blkref, 0, sizeof(BPLib_STOR_CACHE_BlockContent_t));
    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&flow_ref, 0, sizeof(BPLib_STOR_CACHE_Ref_t));
    memset(&temp_block, 0, sizeof(BPLib_STOR_CACHE_Block_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_EQ(bplib_dataservice_attach(&rtbl, &ipn, type, &blkref).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, &sblk);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_EQ(bplib_dataservice_attach(&rtbl, &ipn, type, &blkref).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataAlloc), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, &flow_ref);
    UtAssert_UINT32_EQ(bplib_dataservice_attach(&rtbl, &ipn, type, &blkref).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, &flow_ref);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataAlloc), UT_lib_AltHandler_PointerReturn, &temp_block);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefDuplicate), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowModifyFlags), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowModifyFlags), UT_lib_int8_Handler, NULL);
    UtAssert_UINT32_GT(bplib_dataservice_attach(&rtbl, &ipn, type, &blkref).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataAlloc), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_dataservice_detach(void)
{
    /* Test function for:
     * BPLib_STOR_CACHE_Ref_t bplib_dataservice_detach(bplib_routetbl_t *tbl, const bp_ipn_addr_t *ipn)
     */
    bplib_routetbl_t    rtbl;
    bp_ipn_addr_t       ipn;
    BPLib_STOR_CACHE_Ref_t   flow_ref;
    BPLib_STOR_CACHE_RBT_Link_t    rbt_link;
    BPLib_STOR_CACHE_Block_t temp_block;

    memset(&rtbl, 0, sizeof(bplib_routetbl_t));
    ipn.node_number    = 10;
    ipn.service_number = 100;
    memset(&ipn, 0, sizeof(bp_ipn_addr_t));
    memset(&flow_ref, 0, sizeof(BPLib_STOR_CACHE_Ref_t));
    memset(&rbt_link, 0, sizeof(BPLib_STOR_CACHE_RBT_Link_t));
    memset(&temp_block, 0, sizeof(BPLib_STOR_CACHE_Block_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_NULL(bplib_dataservice_detach(&rtbl, &ipn));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, &flow_ref);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RBT_SearchGeneric), UT_lib_AltHandler_PointerReturn, &rbt_link);
    UtAssert_NULL(bplib_dataservice_detach(&rtbl, &ipn));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, &flow_ref);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataAlloc), UT_lib_AltHandler_PointerReturn, &temp_block);
    UtAssert_NULL(bplib_dataservice_detach(&rtbl, &ipn));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RBT_SearchGeneric), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataAlloc), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_connect_socket(void)
{
    /* Test function for:
     * int bplib_connect_socket(bp_socket_t *desc, const bp_ipn_addr_t *destination_ipn)
     */
    bp_socket_t         desc;
    bp_ipn_addr_t       destination_ipn;
    bplib_socket_info_t sock;
    bplib_routetbl_t    rbtl;

    memset(&desc, 0, sizeof(bp_socket_t));
    memset(&destination_ipn, 0, sizeof(bp_ipn_addr_t));
    destination_ipn.node_number    = 10;
    destination_ipn.service_number = 11;
    memset(&sock, 0, sizeof(bplib_socket_info_t));
    memset(&rbtl, 0, sizeof(bplib_routetbl_t));
    sock.params.local_ipn.node_number    = 10;
    sock.params.local_ipn.service_number = 11;
    sock.parent_rtbl                     = &rbtl;

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_NEQ(bplib_connect_socket(&desc, &destination_ipn), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, &sock);
    UtAssert_UINT32_NEQ(bplib_connect_socket(&desc, &destination_ipn), 0);

    memset(&sock.socket_intf_id, 0, sizeof(sock.socket_intf_id));
    sock.socket_intf_id.hdl = 20;
    UtAssert_UINT32_EQ(bplib_connect_socket(&desc, &destination_ipn), 0);

    sock.params.remote_ipn.node_number = 1;
    UtAssert_UINT32_NEQ(bplib_connect_socket(&desc, &destination_ipn), 0);

    destination_ipn.node_number        = 2;
    sock.params.remote_ipn.node_number = 0;
    UtAssert_UINT32_EQ(bplib_connect_socket(&desc, &destination_ipn), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_create_socket(void)
{
    /* Test function for:
     * bp_socket_t *bplib_create_socket(bplib_routetbl_t *rtbl)
     */
    bplib_routetbl_t    rtbl;
    BPLib_STOR_CACHE_Block_t sblk;
    bplib_socket_info_t sock;

    memset(&rtbl, 0, sizeof(bplib_routetbl_t));
    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&sock, 0, sizeof(bplib_socket_info_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowAlloc), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_NULL(bplib_create_socket(&rtbl));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowAlloc), UT_lib_AltHandler_PointerReturn, &sblk);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, &sock);
    UtAssert_NULL(bplib_create_socket(&rtbl));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowAlloc), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_bind_socket(void)
{
    /* Test function for:
     * int bplib_bind_socket(bp_socket_t *desc, const bp_ipn_addr_t *source_ipn)
     */
    bp_socket_t         desc;
    bp_ipn_addr_t       source_ipn;
    bplib_socket_info_t sock;
    bplib_routetbl_t    rtbl;
    BPLib_STOR_CACHE_Block_t sblk;
    BPLib_STOR_CACHE_Ref_t   flow_ref;

    memset(&desc, 0, sizeof(bp_socket_t));
    memset(&source_ipn, 0, sizeof(bp_ipn_addr_t));
    memset(&sock, 0, sizeof(bplib_socket_info_t));
    memset(&rtbl, 0, sizeof(bplib_routetbl_t));
    sock.parent_rtbl = &rtbl;
    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&flow_ref, 0, sizeof(BPLib_STOR_CACHE_Ref_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_NEQ(bplib_bind_socket(&desc, &source_ipn), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, &sock);
    sock.params.local_ipn.node_number = 1;
    UtAssert_UINT32_NEQ(bplib_bind_socket(&desc, &source_ipn), 0);

    source_ipn.service_number         = 0;
    sock.params.local_ipn.node_number = 0;
    UtAssert_UINT32_NEQ(bplib_bind_socket(&desc, &source_ipn), 0);

    sock.params.local_ipn.node_number = 0;
    source_ipn.node_number            = 1;
    UtAssert_UINT32_NEQ(bplib_bind_socket(&desc, &source_ipn), 0);

    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_RBT_ExtractNode), BP_SUCCESS);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, &sblk);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, &sblk);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, &flow_ref);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, &sock);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataAlloc), UT_lib_AltHandler_PointerReturn, &sblk);
    UtAssert_UINT32_EQ(bplib_bind_socket(&desc, &source_ipn), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataAlloc), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_close_socket(void)
{
    /* Test function for:
     * void bplib_close_socket(bp_socket_t *desc)
     */
    bp_socket_t                    desc;
    bplib_socket_info_t            sock;
    bplib_routetbl_t               rtbl;
    bplib_route_serviceintf_info_t base_intf;
    BPLib_STOR_CACHE_Ref_t              ref;

    memset(&desc, 0, sizeof(bp_socket_t));
    memset(&sock, 0, sizeof(bplib_socket_info_t));
    sock.params.local_ipn.node_number    = 10;
    sock.params.local_ipn.service_number = 11;
    memset(&rtbl, 0, sizeof(bplib_routetbl_t));
    sock.parent_rtbl = &rtbl;
    memset(&base_intf, 0, sizeof(bplib_route_serviceintf_info_t));
    memset(&ref, 0, sizeof(BPLib_STOR_CACHE_Ref_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_VOIDCALL(bplib_close_socket(&desc));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, &sock);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_RBT_ExtractNode), BP_SUCCESS);
    UtAssert_VOIDCALL(bplib_close_socket(&desc));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_send(void)
{
    /* Test function for:
     * int bplib_send(bp_socket_t *desc, const void *payload, size_t size, uint32_t timeout)
     */
    bp_socket_t                    desc;
    void                          *payload = NULL;
    size_t                         size    = 100;
    uint32_t                       timeout = 3000;
    bplib_socket_info_t            sock;
    bplib_routetbl_t               rtbl;
    BPLib_STOR_CACHE_Flow_t             flow;
    BPLib_STOR_CACHE_Block_t            blk;
    BPLib_STOR_CACHE_Ref_t              refptr;
    BPLib_STOR_CACHE_BblockPrimary_t   pri;
    BPLib_STOR_CACHE_BblockCanonical_t ccb_pay;

    memset(&desc, 0, sizeof(bp_socket_t));
    memset(&sock, 0, sizeof(bplib_socket_info_t));
    memset(&rtbl, 0, sizeof(bplib_routetbl_t));
    sock.parent_rtbl = &rtbl;
    memset(&flow, 0, sizeof(BPLib_STOR_CACHE_Flow_t));
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&refptr, 0, sizeof(BPLib_STOR_CACHE_Ref_t));
    memset(&pri, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));
    memset(&ccb_pay, 0, sizeof(BPLib_STOR_CACHE_BblockCanonical_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_NEQ(bplib_send(&desc, payload, size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, &sock);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_NEQ(bplib_send(&desc, payload, size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, &flow);
    UtAssert_UINT32_NEQ(bplib_send(&desc, payload, size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GetCurrentTime), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalAlloc), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowTryPush), UT_lib_int8_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryAlloc), UT_lib_AltHandler_PointerReturn, &blk);
    UtAssert_UINT32_NEQ(bplib_send(&desc, payload, size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_lib_AltHandler_PointerReturn, &pri);
    UtAssert_UINT32_NEQ(bplib_send(&desc, payload, size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_lib_AltHandler_PointerReturn, &ccb_pay);
    UtAssert_UINT32_NEQ(bplib_send(&desc, payload, size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, &refptr);
    UtAssert_UINT32_NEQ(bplib_send(&desc, payload, size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefMakeBlock), UT_lib_AltHandler_PointerReturn, &blk);
    UtAssert_UINT32_NEQ(bplib_send(&desc, payload, size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowTryPush), UT_lib_bool_Handler, NULL);
    UtAssert_UINT32_EQ(bplib_send(&desc, payload, size, timeout), 0);

    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_BlockEncodePri), -1);
    UtAssert_UINT32_NEQ(bplib_send(&desc, payload, size, timeout), 0);

    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_BlockEncodePri), 1);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_BlockEncodePay), -1);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalAlloc), UT_lib_AltHandler_PointerReturn, &blk);
    UtAssert_UINT32_NEQ(bplib_send(&desc, payload, size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryAlloc), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefMakeBlock), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalAlloc), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_recv(void)
{
    /* Test function for:
     * int bplib_recv(bp_socket_t *desc, void *payload, size_t *size, uint32_t timeout)
     */
    bp_socket_t                    desc;
    void                          *payload = NULL;
    size_t                         size    = 100;
    uint32_t                       timeout = 3000;
    bplib_socket_info_t            sock;
    BPLib_STOR_CACHE_Flow_t             flow;
    bplib_routetbl_t               rtbl;
    BPLib_STOR_CACHE_Block_t            blk;
    BPLib_STOR_CACHE_Ref_t              refptr;
    BPLib_STOR_CACHE_BblockCanonical_t ccb_pay;
    BPLib_STOR_CACHE_BblockPrimary_t   pri;

    memset(&desc, 0, sizeof(bp_socket_t));
    memset(&sock, 0, sizeof(bplib_socket_info_t));
    memset(&flow, 0, sizeof(BPLib_STOR_CACHE_Flow_t));
    memset(&rtbl, 0, sizeof(bplib_routetbl_t));
    sock.parent_rtbl = &rtbl;
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&refptr, 0, sizeof(BPLib_STOR_CACHE_Ref_t));
    memset(&ccb_pay, 0, sizeof(BPLib_STOR_CACHE_BblockCanonical_t));
    memset(&pri, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_NEQ(bplib_recv(&desc, payload, &size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, &sock);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_NEQ(bplib_recv(&desc, payload, &size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefFromBlock), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, &flow);
    UtAssert_UINT32_NEQ(bplib_recv(&desc, payload, &size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowTryPull), UT_lib_AltHandler_PointerReturn, &blk);
    UtAssert_UINT32_NEQ(bplib_recv(&desc, payload, &size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryLocateCanonical), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefFromBlock), UT_lib_AltHandler_PointerReturn, &refptr);
    UtAssert_UINT32_NEQ(bplib_recv(&desc, payload, &size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryLocateCanonical), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_lib_AltHandler_PointerReturn, &pri);
    UtAssert_UINT32_NEQ(bplib_recv(&desc, payload, &size, timeout), 0);

    timeout                        = 0;
    ccb_pay.encoded_content_offset = 1;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_lib_AltHandler_PointerReturn, &ccb_pay);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCborExport), UT_lib_sizet_Handler, NULL);
    UtAssert_UINT32_EQ(bplib_recv(&desc, payload, &size, timeout), 0);

    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_BblockCborExport), 1);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCborExport), UT_lib_uint64_Handler, NULL);
    UtAssert_UINT32_NEQ(bplib_recv(&desc, payload, &size, timeout), 0);

    ccb_pay.encoded_content_offset = 0;
    UtAssert_UINT32_NEQ(bplib_recv(&desc, payload, &size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowTryPull), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefFromBlock), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_serviceflow_forward_ingress(void)
{
    /* Test function for:
     * int bplib_serviceflow_forward_ingress(void *arg, BPLib_STOR_CACHE_Block_t *subq_src)
     */
    bplib_routetbl_t    tbl;
    BPLib_STOR_CACHE_Block_t subq_src;
    BPLib_STOR_CACHE_Block_t sblk;

    memset(&tbl, 0, sizeof(bplib_routetbl_t));
    memset(&subq_src, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GetBlockFromLink), UT_lib_sizet_Handler, NULL);
    UtAssert_UINT32_EQ(bplib_serviceflow_forward_ingress(&tbl, &subq_src), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, &sblk);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_NEQ(bplib_serviceflow_forward_ingress(&tbl, &subq_src), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, &sblk);
    UtAssert_UINT32_EQ(bplib_serviceflow_forward_ingress(&tbl, &subq_src), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowTryPull), UT_lib_ingress_AltHandler_PointerReturn, &sblk);
    UtAssert_UINT32_NEQ(bplib_serviceflow_forward_ingress(&tbl, &subq_src), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowTryPull), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_serviceflow_forward_egress(void)
{
    /* Test function for:
     * int bplib_serviceflow_forward_egress(void *arg, BPLib_STOR_CACHE_Block_t *subq_src)
     */
    bplib_routetbl_t               tbl;
    BPLib_STOR_CACHE_Block_t            subq_src;
    BPLib_STOR_CACHE_Block_t            sblk;
    BPLib_STOR_CACHE_Block_t            sblk1;
    bplib_route_serviceintf_info_t base_intf;

    memset(&tbl, 0, sizeof(bplib_routetbl_t));
    memset(&subq_src, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&sblk1, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&base_intf, 0, sizeof(bplib_route_serviceintf_info_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GetBlockFromLink), UT_lib_sizet_Handler, NULL);
    UtAssert_UINT32_EQ(bplib_serviceflow_forward_egress(&tbl, &subq_src), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, &base_intf);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_NEQ(bplib_serviceflow_forward_egress(&tbl, &subq_src), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, &sblk);
    UtAssert_UINT32_EQ(bplib_serviceflow_forward_egress(&tbl, &subq_src), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowTryPull), UT_lib_egress_AltHandler_PointerReturn, &sblk);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_lib_AltHandler_PointerReturn, &sblk1);
    UtAssert_UINT32_NEQ(bplib_serviceflow_forward_egress(&tbl, &subq_src), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowTryPull), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_dataservice_event_impl(void)
{
    /* Test function for:
     * int bplib_dataservice_event_impl(void *arg, BPLib_STOR_CACHE_Block_t *intf_block)
     */
    BPLib_STOR_CACHE_FlowGenericEvent_t event;
    BPLib_STOR_CACHE_Block_t              intf_block;
    BPLib_STOR_CACHE_Flow_t               flow;

    memset(&event, 0, sizeof(BPLib_STOR_CACHE_FlowGenericEvent_t));
    memset(&intf_block, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&event.intf_state.intf_id, 0, sizeof(bp_handle_t));
    memset(&flow, 0, sizeof(BPLib_STOR_CACHE_Flow_t));

    UtAssert_UINT32_EQ(bplib_dataservice_event_impl(&event, &intf_block), 0);

    event.event_type = BPLib_STOR_CACHE_FlowEventUp;
    UtAssert_UINT32_EQ(bplib_dataservice_event_impl(&event, &intf_block), 0);

    event.intf_state.intf_id = BPLIB_HANDLE_MPOOL_BASE;
    UtAssert_UINT32_EQ(bplib_dataservice_event_impl(&event, &intf_block), 0);

    flow.current_state_flags = BPLIB_MEM_FLOW_FLAGS_ENDPOINT;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, &flow);
    UtAssert_UINT32_EQ(bplib_dataservice_event_impl(&event, &intf_block), 0);

    event.event_type = BPLib_STOR_CACHE_FlowEventDown;
    UtAssert_UINT32_EQ(bplib_dataservice_event_impl(&event, &intf_block), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_dataservice_base_construct(void)
{
    /* Test function for:
     * int bplib_dataservice_base_construct(void *arg, BPLib_STOR_CACHE_Block_t *blk)
     */
    void                          *arg = NULL;
    BPLib_STOR_CACHE_Block_t            blk;
    bplib_route_serviceintf_info_t base_intf;

    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&base_intf, 0, sizeof(bplib_route_serviceintf_info_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_GT(bplib_dataservice_base_construct(arg, &blk), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, &base_intf);
    UtAssert_UINT32_EQ(bplib_dataservice_base_construct(arg, &blk), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void TestBplibBase_DataServiceApi_Register(void)
{
    UtTest_Add(test_bplib_dataservice_add_base_intf, NULL, NULL, "Test bplib_dataservice_add_base_intf");
    UtTest_Add(test_bplib_dataservice_attach, NULL, NULL, "Test bplib_dataservice_attach");
    UtTest_Add(test_bplib_dataservice_detach, NULL, NULL, "Test bplib_dataservice_detach");
    UtTest_Add(test_bplib_connect_socket, NULL, NULL, "Test bplib_connect_socket");
    UtTest_Add(test_bplib_create_socket, NULL, NULL, "Test bplib_create_socket");
    UtTest_Add(test_bplib_bind_socket, NULL, NULL, "Test bplib_bind_socket");
    UtTest_Add(test_bplib_close_socket, NULL, NULL, "Test bplib_close_socket");
    UtTest_Add(test_bplib_send, NULL, NULL, "Test bplib_send");
    UtTest_Add(test_bplib_recv, NULL, NULL, "Test bplib_recv");
    UtTest_Add(test_bplib_serviceflow_forward_ingress, NULL, NULL, "Test bplib_serviceflow_forward_ingress");
    UtTest_Add(test_bplib_serviceflow_forward_egress, NULL, NULL, "Test bplib_serviceflow_forward_egress");
    UtTest_Add(test_bplib_dataservice_event_impl, NULL, NULL, "Test bplib_dataservice_event_impl");
    UtTest_Add(test_bplib_dataservice_base_construct, NULL, NULL, "Test bplib_dataservice_base_construct");
}
