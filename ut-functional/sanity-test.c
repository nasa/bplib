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

/*************************************************************************
 * Includes
 *************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>

#include <osapi.h>

#include <utassert.h>
#include <utstubs.h>
#include <uttest.h>

#include "bplib.h"
#include "bplib_routing.h"

#include "v7_cache.h"
#include "v7_mpool.h"

bplib_routetbl_t *s1_rtbl;
bplib_routetbl_t *s2_rtbl;

bp_handle_t s1_intf_store;
bp_handle_t s1_intf_id1;
bp_handle_t s1_intf_id2;
bp_handle_t s1_intf_cla;

bp_handle_t s2_intf_store;
bp_handle_t s2_intf_id1;
bp_handle_t s2_intf_id2;
bp_handle_t s2_intf_cla;

uint8_t bundle_buffer[512];

osal_id_t mgt_task_1;
osal_id_t mgt_task_2;

/* The OSAL "os" submodule does not provide bplib_os_log at this time, so its here.  This should be moved  */
int bplib_os_log(const char *file, unsigned int line, uint32_t *flags, uint32_t event, const char *fmt, ...)
{
    va_list va;
    char    bpmsg[128];

    /* preferably Ut assert should provide a varargs version of this call, but it does not right now */
    va_start(va, fmt);
    vsnprintf(bpmsg, sizeof(bpmsg), fmt, va);
    va_end(va);

    UtAssert_Message(UTASSERT_CASETYPE_INFO, file, line, "BP: %s", bpmsg);
    return BP_SUCCESS;
}

void display(void *output, const uint8_t *DataPtr, size_t DisplayOffset, size_t Count)
{
    uint16_t i;
    char     display[20];
    FILE    *fp = output;

    fprintf(fp, "Data Segment Length=%lu:", (unsigned long)Count);
    memset(display, 0, sizeof(display));
    for (i = 0; i < Count; ++i)
    {
        if ((i & 0x0F) == 0)
        {
            fprintf(fp, "  %s\n  %03lx:", display, (unsigned long)DisplayOffset);
            memset(display, 0, sizeof(display));
        }
        fprintf(fp, " %02x", *DataPtr);
        if (isprint(*DataPtr))
        {
            display[i & 0x0F] = *DataPtr;
        }
        else
        {
            display[i & 0x0F] = '.';
        }
        ++DataPtr;
        ++DisplayOffset;
    }
    if (i > 0)
    {
        while (i & 0xF)
        {
            fprintf(fp, "   ");
            ++i;
        }
    }
    fprintf(fp, "  %s\n", display);
}

void *bplib_mgt_entry_point(bplib_routetbl_t *rtbl)
{
    while (true)
    {
        bplib_route_maintenance_request_wait(rtbl);

        // printf("@%lu: Maintenance running...\n", (unsigned long)bplib_os_get_dtntime_ms());

        /* do maintenance regardless of what the "request" returned, as that
         * currently only reflects actual requests, not time-based poll actions */
        bplib_route_periodic_maintenance(rtbl);
    }
}

/* These separate entry points need to exist only because OSAL does not have a mechanism
 * to pass an argument to a started task */
void bplib_mgt1_entry_point(void)
{
    bplib_mgt_entry_point(s1_rtbl);
}

void bplib_mgt2_entry_point(void)
{
    bplib_mgt_entry_point(s2_rtbl);
}

void bplib_start_mgt_task(void)
{
    UtAssert_INT32_EQ(OS_TaskCreate(&mgt_task_1, "MGT1", bplib_mgt1_entry_point, NULL, 16384, 100, 0), OS_SUCCESS);
    UtAssert_INT32_EQ(OS_TaskCreate(&mgt_task_2, "MGT2", bplib_mgt2_entry_point, NULL, 16384, 100, 0), OS_SUCCESS);
}

void bplib_end_mgt_task(void)
{
    UtAssert_INT32_EQ(OS_TaskDelete(mgt_task_1), OS_SUCCESS);
    UtAssert_INT32_EQ(OS_TaskDelete(mgt_task_2), OS_SUCCESS);
}

void bplib_route_test(void)
{
    bplib_routetbl_t *rtbl;
    bp_ipn_addr_t     storage_addr;

    /* Test route table with 1MB of cache */
    UtAssert_NOT_NULL(rtbl = bplib_route_alloc_table(10, 1 << 20));

    s1_intf_id1 = bplib_create_node_intf(rtbl, 101);
    UtAssert_BOOL_TRUE(bp_handle_is_valid(s1_intf_id1));
    s1_intf_id2 = bplib_create_node_intf(rtbl, 102);
    UtAssert_BOOL_TRUE(bp_handle_is_valid(s1_intf_id2));

    storage_addr  = (bp_ipn_addr_t) {101, 10};
    s1_intf_store = bplib_create_ram_storage(rtbl, &storage_addr);
    UtAssert_BOOL_TRUE(bp_handle_is_valid(s1_intf_store));

    s1_intf_cla = bplib_create_cla_intf(rtbl);
    UtAssert_BOOL_TRUE(bp_handle_is_valid(s1_intf_cla));

    UtAssert_INT32_EQ(bplib_route_add(rtbl, 0, 0, s1_intf_store), BP_SUCCESS);
    UtAssert_INT32_EQ(bplib_route_add(rtbl, 201, ~(bp_ipn_t)0, s1_intf_cla), BP_SUCCESS);

    UtAssert_INT32_EQ(
        bplib_route_intf_set_flags(rtbl, s1_intf_id1, BPLIB_INTF_STATE_ADMIN_UP | BPLIB_INTF_STATE_OPER_UP),
        BP_SUCCESS);
    UtAssert_INT32_EQ(
        bplib_route_intf_set_flags(rtbl, s1_intf_store, BPLIB_INTF_STATE_ADMIN_UP | BPLIB_INTF_STATE_OPER_UP),
        BP_SUCCESS);
    UtAssert_INT32_EQ(
        bplib_route_intf_set_flags(rtbl, s1_intf_cla, BPLIB_INTF_STATE_ADMIN_UP | BPLIB_INTF_STATE_OPER_UP),
        BP_SUCCESS);

    s1_rtbl = rtbl;

    /* Test route table with 1MB of cache */
    UtAssert_NOT_NULL(rtbl = bplib_route_alloc_table(10, 1 << 20));

    s2_intf_id1 = bplib_create_node_intf(rtbl, 201);
    UtAssert_BOOL_TRUE(bp_handle_is_valid(s2_intf_id1));
    s2_intf_id2 = bplib_create_node_intf(rtbl, 202);
    UtAssert_BOOL_TRUE(bp_handle_is_valid(s2_intf_id2));

    storage_addr = (bp_ipn_addr_t) {201, 10};

    s2_intf_store = bplib_create_ram_storage(rtbl, &storage_addr);
    UtAssert_BOOL_TRUE(bp_handle_is_valid(s2_intf_store));

    s2_intf_cla = bplib_create_cla_intf(rtbl);
    UtAssert_BOOL_TRUE(bp_handle_is_valid(s2_intf_cla));

    UtAssert_INT32_EQ(bplib_route_add(rtbl, 0, 0, s2_intf_store), BP_SUCCESS);
    UtAssert_INT32_EQ(bplib_route_add(rtbl, 101, ~(bp_ipn_t)0, s2_intf_cla), BP_SUCCESS);

    UtAssert_INT32_EQ(
        bplib_route_intf_set_flags(rtbl, s2_intf_id1, BPLIB_INTF_STATE_ADMIN_UP | BPLIB_INTF_STATE_OPER_UP),
        BP_SUCCESS);
    UtAssert_INT32_EQ(
        bplib_route_intf_set_flags(rtbl, s2_intf_store, BPLIB_INTF_STATE_ADMIN_UP | BPLIB_INTF_STATE_OPER_UP),
        BP_SUCCESS);
    UtAssert_INT32_EQ(
        bplib_route_intf_set_flags(rtbl, s2_intf_cla, BPLIB_INTF_STATE_ADMIN_UP | BPLIB_INTF_STATE_OPER_UP),
        BP_SUCCESS);

    s2_rtbl = rtbl;
}

void bplib_bundle_test(void)
{
    bp_socket_t  *desc;
    const char    my_payload[] = "The Answer to the Ultimate Question of Life, The Universe, and Everything is 42";
    char          recv_payload[1 + sizeof(my_payload)];
    size_t        bundle_sz;
    size_t        recv_sz;
    bp_ipn_addr_t source_ipn1;
    bp_ipn_addr_t dest_ipn1;
    osal_id_t     fd;

    UtAssert_NOT_NULL(desc = bplib_create_socket(s1_rtbl));

    source_ipn1 = (bp_ipn_addr_t) {101, 1};
    UtAssert_INT32_EQ(bplib_bind_socket(desc, &source_ipn1), BP_SUCCESS);

    dest_ipn1 = (bp_ipn_addr_t) {201, 1};
    UtAssert_INT32_EQ(bplib_connect_socket(desc, &dest_ipn1), BP_SUCCESS);

    /* wait for any pending data flows to complete */
    UtAssert_VOIDCALL(bplib_route_maintenance_complete_wait(s1_rtbl));

    UtDebug("@%lu: Storing payload:\n", (unsigned long)bplib_os_get_dtntime_ms());
    display(stdout, (const uint8_t *)my_payload, 0, sizeof(my_payload));

    UtAssert_INT32_EQ(bplib_send(desc, my_payload, sizeof(my_payload), BP_CHECK), BP_SUCCESS);

    /* Setting the CLA "up" should cause the previously-blocked bundle to unblock */
    UtAssert_INT32_EQ(bplib_route_intf_set_flags(s1_rtbl, s1_intf_cla, BPLIB_INTF_STATE_OPER_UP), BP_SUCCESS);

    /* wait for any pending data flows to complete */
    UtAssert_VOIDCALL(bplib_route_maintenance_complete_wait(s1_rtbl));

    bundle_sz = sizeof(bundle_buffer);
    UtAssert_INT32_EQ(bplib_cla_egress(s1_rtbl, s1_intf_cla, bundle_buffer, &bundle_sz, BP_CHECK), BP_SUCCESS);

    UtAssert_VOIDCALL(bplib_close_socket(desc));

    /* wait for any pending data flows to complete */
    UtAssert_VOIDCALL(bplib_route_maintenance_complete_wait(s1_rtbl));

    printf("@%lu: Bundle content:\n", (unsigned long)bplib_os_get_dtntime_ms());
    display(stdout, bundle_buffer, 0, bundle_sz);

    /* dump the CBOR-encoded bundle content to a file, so it can be checked for validity */
    UtAssert_INT32_EQ(OS_OpenCreate(&fd, "/ut/test.cbor", OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE, OS_WRITE_ONLY),
                      OS_SUCCESS);
    UtAssert_INT32_EQ(OS_write(fd, bundle_buffer, bundle_sz), bundle_sz);
    UtAssert_INT32_EQ(OS_close(fd), OS_SUCCESS);

    /*
     * The first maintenance cycle will end up recycling the bundle ref from CLA egress,
     * which will invoke the recycle callback to the storage intf, which puts
     * it back on the pending list for re-evaluating (but note this happens _after_ the
     * pending list was already processed earlier in the maintenance cycle).
     */

    /*
     * The second maintenance cycle will now identify the bundle as no longer necessary,
     * in the storage intf, and it will recycle those buffers as well, which causes its
     * refcount to become 0, and the entire bundle is dropped.
     */

    UtAssert_VOIDCALL(bplib_cache_debug_scan(s1_rtbl, s1_intf_store));

    /* this does a sanity check on the pool and displays various block allocation stats */
    UtAssert_VOIDCALL(bplib_mpool_debug_scan(bplib_route_get_mpool(s1_rtbl)));

    UtAssert_NOT_NULL(desc = bplib_create_socket(s2_rtbl));

    source_ipn1 = (bp_ipn_addr_t) {201, 1};
    UtAssert_INT32_EQ(bplib_bind_socket(desc, &source_ipn1), BP_SUCCESS);

    dest_ipn1 = (bp_ipn_addr_t) {101, 1};
    UtAssert_INT32_EQ(bplib_connect_socket(desc, &dest_ipn1), BP_SUCCESS);

    UtAssert_INT32_EQ(bplib_cla_ingress(s2_rtbl, s2_intf_cla, bundle_buffer, bundle_sz, BP_CHECK), BP_SUCCESS);

    /* wait for any pending data flows to complete */
    UtAssert_VOIDCALL(bplib_route_maintenance_complete_wait(s2_rtbl));

    recv_sz = sizeof(recv_payload);
    UtAssert_INT32_EQ(bplib_recv(desc, recv_payload, &recv_sz, BP_CHECK), BP_SUCCESS);

    UtAssert_VOIDCALL(bplib_close_socket(desc));

    /* wait for any pending data flows to complete */
    UtAssert_VOIDCALL(bplib_route_maintenance_complete_wait(s2_rtbl));

    UtDebug("@%lu: Received payload:\n", (unsigned long)bplib_os_get_dtntime_ms());
    display(stdout, (const uint8_t *)recv_payload, 0, recv_sz);

    UtAssert_UINT32_EQ(recv_sz, sizeof(my_payload));

    UtAssert_MemCmp(recv_payload, my_payload, recv_sz, "Bundle Data Content");

    /* let some additional maintenance cycles run */
    UtDebug("@%lu: shutting down...\n", (unsigned long)bplib_os_get_dtntime_ms());
    OS_TaskDelay(2000);
}

void prepare(void)
{
    osal_id_t fs;

    UtAssert_INT32_EQ(OS_API_Init(), OS_SUCCESS);
    UtAssert_INT32_EQ(bplib_init(), BP_SUCCESS);

    /* map virtual "/ut" path to the cwd of this test */
    UtAssert_INT32_EQ(OS_FileSysAddFixedMap(&fs, ".", "/ut"), OS_SUCCESS);
}

/******************************************************************************
 * Main
 ******************************************************************************/
void UtTest_Setup(void)
{
    /* call required init functions */
    prepare();

    UtTest_Add(bplib_route_test, NULL, NULL, "route test");
    UtTest_Add(bplib_bundle_test, bplib_start_mgt_task, bplib_end_mgt_task, "bundle test");
}
