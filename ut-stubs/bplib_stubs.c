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

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in bplib header
 */

#include "bplib.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_bind_socket()
 * ----------------------------------------------------
 */
int bplib_bind_socket(bp_socket_t *desc, const bp_ipn_addr_t *source_ipn)
{
    UT_GenStub_SetupReturnBuffer(bplib_bind_socket, int);

    UT_GenStub_AddParam(bplib_bind_socket, bp_socket_t *, desc);
    UT_GenStub_AddParam(bplib_bind_socket, const bp_ipn_addr_t *, source_ipn);

    UT_GenStub_Execute(bplib_bind_socket, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_bind_socket, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_cla_egress()
 * ----------------------------------------------------
 */
int bplib_cla_egress(bplib_routetbl_t *rtbl, bp_handle_t intf_id, void *bundle, size_t *size, uint32_t timeout)
{
    UT_GenStub_SetupReturnBuffer(bplib_cla_egress, int);

    UT_GenStub_AddParam(bplib_cla_egress, bplib_routetbl_t *, rtbl);
    UT_GenStub_AddParam(bplib_cla_egress, bp_handle_t, intf_id);
    UT_GenStub_AddParam(bplib_cla_egress, void *, bundle);
    UT_GenStub_AddParam(bplib_cla_egress, size_t *, size);
    UT_GenStub_AddParam(bplib_cla_egress, uint32_t, timeout);

    UT_GenStub_Execute(bplib_cla_egress, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_cla_egress, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_cla_ingress()
 * ----------------------------------------------------
 */
int bplib_cla_ingress(bplib_routetbl_t *rtbl, bp_handle_t intf_id, const void *bundle, size_t size, uint32_t timeout)
{
    UT_GenStub_SetupReturnBuffer(bplib_cla_ingress, int);

    UT_GenStub_AddParam(bplib_cla_ingress, bplib_routetbl_t *, rtbl);
    UT_GenStub_AddParam(bplib_cla_ingress, bp_handle_t, intf_id);
    UT_GenStub_AddParam(bplib_cla_ingress, const void *, bundle);
    UT_GenStub_AddParam(bplib_cla_ingress, size_t, size);
    UT_GenStub_AddParam(bplib_cla_ingress, uint32_t, timeout);

    UT_GenStub_Execute(bplib_cla_ingress, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_cla_ingress, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_close_socket()
 * ----------------------------------------------------
 */
void bplib_close_socket(bp_socket_t *desc)
{
    UT_GenStub_AddParam(bplib_close_socket, bp_socket_t *, desc);

    UT_GenStub_Execute(bplib_close_socket, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_config_integer()
 * ----------------------------------------------------
 */
int bplib_config_integer(bplib_routetbl_t *rtbl, bp_handle_t intf_id, bplib_variable_t var_id, bp_sval_t value)
{
    UT_GenStub_SetupReturnBuffer(bplib_config_integer, int);

    UT_GenStub_AddParam(bplib_config_integer, bplib_routetbl_t *, rtbl);
    UT_GenStub_AddParam(bplib_config_integer, bp_handle_t, intf_id);
    UT_GenStub_AddParam(bplib_config_integer, bplib_variable_t, var_id);
    UT_GenStub_AddParam(bplib_config_integer, bp_sval_t, value);

    UT_GenStub_Execute(bplib_config_integer, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_config_integer, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_connect_socket()
 * ----------------------------------------------------
 */
int bplib_connect_socket(bp_socket_t *desc, const bp_ipn_addr_t *destination_ipn)
{
    UT_GenStub_SetupReturnBuffer(bplib_connect_socket, int);

    UT_GenStub_AddParam(bplib_connect_socket, bp_socket_t *, desc);
    UT_GenStub_AddParam(bplib_connect_socket, const bp_ipn_addr_t *, destination_ipn);

    UT_GenStub_Execute(bplib_connect_socket, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_connect_socket, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_create_cla_intf()
 * ----------------------------------------------------
 */
bp_handle_t bplib_create_cla_intf(bplib_routetbl_t *rtbl)
{
    UT_GenStub_SetupReturnBuffer(bplib_create_cla_intf, bp_handle_t);

    UT_GenStub_AddParam(bplib_create_cla_intf, bplib_routetbl_t *, rtbl);

    UT_GenStub_Execute(bplib_create_cla_intf, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_create_cla_intf, bp_handle_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_create_file_storage()
 * ----------------------------------------------------
 */
bp_handle_t bplib_create_file_storage(bplib_routetbl_t *rtbl, const bp_ipn_addr_t *ipn_addr)
{
    UT_GenStub_SetupReturnBuffer(bplib_create_file_storage, bp_handle_t);

    UT_GenStub_AddParam(bplib_create_file_storage, bplib_routetbl_t *, rtbl);
    UT_GenStub_AddParam(bplib_create_file_storage, const bp_ipn_addr_t *, ipn_addr);

    UT_GenStub_Execute(bplib_create_file_storage, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_create_file_storage, bp_handle_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_create_node_intf()
 * ----------------------------------------------------
 */
bp_handle_t bplib_create_node_intf(bplib_routetbl_t *rtbl, bp_ipn_t node_num)
{
    UT_GenStub_SetupReturnBuffer(bplib_create_node_intf, bp_handle_t);

    UT_GenStub_AddParam(bplib_create_node_intf, bplib_routetbl_t *, rtbl);
    UT_GenStub_AddParam(bplib_create_node_intf, bp_ipn_t, node_num);

    UT_GenStub_Execute(bplib_create_node_intf, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_create_node_intf, bp_handle_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_create_ram_storage()
 * ----------------------------------------------------
 */
bp_handle_t bplib_create_ram_storage(bplib_routetbl_t *rtbl, const bp_ipn_addr_t *ipn_addr)
{
    UT_GenStub_SetupReturnBuffer(bplib_create_ram_storage, bp_handle_t);

    UT_GenStub_AddParam(bplib_create_ram_storage, bplib_routetbl_t *, rtbl);
    UT_GenStub_AddParam(bplib_create_ram_storage, const bp_ipn_addr_t *, ipn_addr);

    UT_GenStub_Execute(bplib_create_ram_storage, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_create_ram_storage, bp_handle_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_create_socket()
 * ----------------------------------------------------
 */
bp_socket_t *bplib_create_socket(bplib_routetbl_t *rtbl)
{
    UT_GenStub_SetupReturnBuffer(bplib_create_socket, bp_socket_t *);

    UT_GenStub_AddParam(bplib_create_socket, bplib_routetbl_t *, rtbl);

    UT_GenStub_Execute(bplib_create_socket, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_create_socket, bp_socket_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_deinit()
 * ----------------------------------------------------
 */
void bplib_deinit(void)
{

    UT_GenStub_Execute(bplib_deinit, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_display()
 * ----------------------------------------------------
 */
int bplib_display(const void *bundle, size_t size, uint32_t *flags)
{
    UT_GenStub_SetupReturnBuffer(bplib_display, int);

    UT_GenStub_AddParam(bplib_display, const void *, bundle);
    UT_GenStub_AddParam(bplib_display, size_t, size);
    UT_GenStub_AddParam(bplib_display, uint32_t *, flags);

    UT_GenStub_Execute(bplib_display, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_display, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_eid2ipn()
 * ----------------------------------------------------
 */
int bplib_eid2ipn(const char *eid, size_t len, bp_ipn_t *node, bp_ipn_t *service)
{
    UT_GenStub_SetupReturnBuffer(bplib_eid2ipn, int);

    UT_GenStub_AddParam(bplib_eid2ipn, const char *, eid);
    UT_GenStub_AddParam(bplib_eid2ipn, size_t, len);
    UT_GenStub_AddParam(bplib_eid2ipn, bp_ipn_t *, node);
    UT_GenStub_AddParam(bplib_eid2ipn, bp_ipn_t *, service);

    UT_GenStub_Execute(bplib_eid2ipn, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_eid2ipn, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_init()
 * ----------------------------------------------------
 */
int bplib_init(void)
{
    UT_GenStub_SetupReturnBuffer(bplib_init, int);

    UT_GenStub_Execute(bplib_init, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_init, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_ipn2eid()
 * ----------------------------------------------------
 */
int bplib_ipn2eid(char *eid, size_t len, bp_ipn_t node, bp_ipn_t service)
{
    UT_GenStub_SetupReturnBuffer(bplib_ipn2eid, int);

    UT_GenStub_AddParam(bplib_ipn2eid, char *, eid);
    UT_GenStub_AddParam(bplib_ipn2eid, size_t, len);
    UT_GenStub_AddParam(bplib_ipn2eid, bp_ipn_t, node);
    UT_GenStub_AddParam(bplib_ipn2eid, bp_ipn_t, service);

    UT_GenStub_Execute(bplib_ipn2eid, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_ipn2eid, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_query_integer()
 * ----------------------------------------------------
 */
int bplib_query_integer(bplib_routetbl_t *rtbl, bp_handle_t intf_id, bplib_variable_t var_id, bp_sval_t *value)
{
    UT_GenStub_SetupReturnBuffer(bplib_query_integer, int);

    UT_GenStub_AddParam(bplib_query_integer, bplib_routetbl_t *, rtbl);
    UT_GenStub_AddParam(bplib_query_integer, bp_handle_t, intf_id);
    UT_GenStub_AddParam(bplib_query_integer, bplib_variable_t, var_id);
    UT_GenStub_AddParam(bplib_query_integer, bp_sval_t *, value);

    UT_GenStub_Execute(bplib_query_integer, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_query_integer, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_recv()
 * ----------------------------------------------------
 */
int bplib_recv(bp_socket_t *desc, void *payload, size_t *size, uint32_t timeout)
{
    UT_GenStub_SetupReturnBuffer(bplib_recv, int);

    UT_GenStub_AddParam(bplib_recv, bp_socket_t *, desc);
    UT_GenStub_AddParam(bplib_recv, void *, payload);
    UT_GenStub_AddParam(bplib_recv, size_t *, size);
    UT_GenStub_AddParam(bplib_recv, uint32_t, timeout);

    UT_GenStub_Execute(bplib_recv, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_recv, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_send()
 * ----------------------------------------------------
 */
int bplib_send(bp_socket_t *desc, const void *payload, size_t size, uint32_t timeout)
{
    UT_GenStub_SetupReturnBuffer(bplib_send, int);

    UT_GenStub_AddParam(bplib_send, bp_socket_t *, desc);
    UT_GenStub_AddParam(bplib_send, const void *, payload);
    UT_GenStub_AddParam(bplib_send, size_t, size);
    UT_GenStub_AddParam(bplib_send, uint32_t, timeout);

    UT_GenStub_Execute(bplib_send, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_send, int);
}
