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
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>

#include "bplib.h"
#include "bplib_store_ram.h"
#include "bplib_routing.h"
#include "crc.h"

#include "v7_codec.h"
#include "v7_mpool.h"
#include "v7_cache.h"
#include "v7_types.h"

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

bp_primary_block_t priblk;

pthread_t mgt_task_1;
pthread_t mgt_task_2;

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

int crc_test(bplib_crc_parameters_t *params)
{
    const char  input1[] = "123456789";
    bp_crcval_t test;

    test = bplib_crc_initial_value(params);
    test = bplib_crc_update(params, test, input1, sizeof(input1) - 1);
    printf("CRC Test: %s => %0*x\n", bplib_crc_get_name(params), bplib_crc_get_width(params) / 4,
           (unsigned int)bplib_crc_finalize(params, test));

    return 0;
}

void *bplib_mgt_entry_point(void *arg)
{
    bplib_routetbl_t *rtbl;

    rtbl = arg;

    while (true)
    {
        bplib_route_maintenance_request_wait(rtbl);

        // printf("@%lu: Maintenance running...\n", (unsigned long)bplib_os_get_dtntime_ms());

        /* do maintenance regardless of what the "request" returned, as that
         * currently only reflects actual requests, not time-based poll actions */
        bplib_route_periodic_maintenance(rtbl);
    }
}

int bplib_start_mgt_task(void)
{
    if (pthread_create(&mgt_task_1, NULL, bplib_mgt_entry_point, s1_rtbl) < 0)
    {
        perror("pthread_create 1");
        return -1;
    }
    if (pthread_create(&mgt_task_2, NULL, bplib_mgt_entry_point, s2_rtbl) < 0)
    {
        perror("pthread_create 2");
        return -1;
    }

    return 0;
}

int bplib_route_test(void)
{
    bplib_routetbl_t *rtbl;
    bp_ipn_addr_t     storage_addr;

    /* Test route table with 1MB of cache */
    rtbl = bplib_route_alloc_table(10, 1 << 20);
    if (rtbl == NULL)
    {
        fprintf(stderr, "%s(): bplib_route_alloc_table failed\n", __func__);
    }

    s1_intf_id1 = bplib_create_node_intf(rtbl, 101);
    if (!bp_handle_is_valid(s1_intf_id1))
    {
        fprintf(stderr, "%s(): bplib_create_node_intf 1 failed\n", __func__);
    }
    s1_intf_id2 = bplib_create_node_intf(rtbl, 102);
    if (!bp_handle_is_valid(s1_intf_id2))
    {
        fprintf(stderr, "%s(): bplib_create_node_intf 2 failed\n", __func__);
    }

    storage_addr = (bp_ipn_addr_t) {101, 10};

    s1_intf_store = bplib_create_ram_storage(rtbl, &storage_addr);
    if (!bp_handle_is_valid(s1_intf_store))
    {
        fprintf(stderr, "%s(): bplib_create_ram_storage failed\n", __func__);
    }

    s1_intf_cla = bplib_create_cla_intf(rtbl);
    if (!bp_handle_is_valid(s1_intf_cla))
    {
        fprintf(stderr, "%s(): bplib_create_cla_intf 2 failed\n", __func__);
    }

    if (bplib_route_add(rtbl, 0, 0, s1_intf_store) < 0)
    {
        fprintf(stderr, "%s(): bplib_route_add storage failed\n", __func__);
    }
    if (bplib_route_add(rtbl, 201, ~(bp_ipn_t)0, s1_intf_cla) < 0)
    {
        fprintf(stderr, "%s(): bplib_route_add cla failed\n", __func__);
    }

    if (bplib_route_intf_set_flags(rtbl, s1_intf_id1, BPLIB_INTF_STATE_ADMIN_UP | BPLIB_INTF_STATE_OPER_UP) < 0)
    {
        fprintf(stderr, "%s(): bplib_route_intf_set_flags data1 failed\n", __func__);
    }
    if (bplib_route_intf_set_flags(rtbl, s1_intf_store, BPLIB_INTF_STATE_ADMIN_UP | BPLIB_INTF_STATE_OPER_UP) < 0)
    {
        fprintf(stderr, "%s(): bplib_route_intf_set_flags storage failed\n", __func__);
    }
    if (bplib_route_intf_set_flags(rtbl, s1_intf_cla, BPLIB_INTF_STATE_ADMIN_UP | BPLIB_INTF_STATE_OPER_UP) < 0)
    {
        fprintf(stderr, "%s(): bplib_route_intf_set_flags cla failed\n", __func__);
    }

    s1_rtbl = rtbl;

    /* Test route table with 1MB of cache */
    rtbl = bplib_route_alloc_table(10, 1 << 20);
    if (rtbl == NULL)
    {
        fprintf(stderr, "%s(): bplib_route_alloc_table failed\n", __func__);
    }

    s2_intf_id1 = bplib_create_node_intf(rtbl, 201);
    if (!bp_handle_is_valid(s2_intf_id1))
    {
        fprintf(stderr, "%s(): bplib_create_node_intf 1 failed\n", __func__);
    }
    s2_intf_id2 = bplib_create_node_intf(rtbl, 202);
    if (!bp_handle_is_valid(s2_intf_id2))
    {
        fprintf(stderr, "%s(): bplib_create_node_intf 2 failed\n", __func__);
    }

    storage_addr = (bp_ipn_addr_t) {201, 10};

    s2_intf_store = bplib_create_ram_storage(rtbl, &storage_addr);
    if (!bp_handle_is_valid(s2_intf_store))
    {
        fprintf(stderr, "%s(): bplib_create_ram_storage failed\n", __func__);
    }

    s2_intf_cla = bplib_create_cla_intf(rtbl);
    if (!bp_handle_is_valid(s2_intf_cla))
    {
        fprintf(stderr, "%s(): bplib_create_cla_intf 2 failed\n", __func__);
    }

    if (bplib_route_add(rtbl, 0, 0, s2_intf_store) < 0)
    {
        fprintf(stderr, "%s(): bplib_route_add storage failed\n", __func__);
    }
    if (bplib_route_add(rtbl, 101, ~(bp_ipn_t)0, s2_intf_cla) < 0)
    {
        fprintf(stderr, "%s(): bplib_route_add cla failed\n", __func__);
    }

    if (bplib_route_intf_set_flags(rtbl, s2_intf_id1, BPLIB_INTF_STATE_ADMIN_UP | BPLIB_INTF_STATE_OPER_UP) < 0)
    {
        fprintf(stderr, "%s(): bplib_route_intf_set_flags id1 failed\n", __func__);
    }
    if (bplib_route_intf_set_flags(rtbl, s2_intf_store, BPLIB_INTF_STATE_ADMIN_UP | BPLIB_INTF_STATE_OPER_UP) < 0)
    {
        fprintf(stderr, "%s(): bplib_route_intf_set_flags storage failed\n", __func__);
    }
    if (bplib_route_intf_set_flags(rtbl, s2_intf_cla, BPLIB_INTF_STATE_ADMIN_UP | BPLIB_INTF_STATE_OPER_UP) < 0)
    {
        fprintf(stderr, "%s(): bplib_route_intf_set_flags cla failed\n", __func__);
    }

    s2_rtbl = rtbl;

    return 0;
}

int bplib2_bundle_test(void)
{
    bp_socket_t *desc;
    char         my_payload[] = "The Answer to the Ultimate Question of Life, The Universe, and Everything is 42";
    char         recv_payload[1 + sizeof(my_payload)];
    int          lib_status;
    size_t       bundle_sz;
    size_t       recv_sz;

    bp_ipn_addr_t source_ipn1;
    bp_ipn_addr_t dest_ipn1;

    desc = bplib_create_socket(s1_rtbl);
    if (desc == NULL)
    {
        fprintf(stderr, "Failed bplib_open()... exiting\n");
        return -1;
    }

    source_ipn1 = (bp_ipn_addr_t) {101, 1};
    if (bplib_bind_socket(desc, &source_ipn1) < 0)
    {
        fprintf(stderr, "Failed bplib_bind_socket()... exiting\n");
        bplib_close_socket(desc);
        return -1;
    }

    dest_ipn1 = (bp_ipn_addr_t) {201, 1};
    if (bplib_connect_socket(desc, &dest_ipn1) < 0)
    {
        fprintf(stderr, "Failed bplib_bind_socket()... exiting\n");
        bplib_close_socket(desc);
        return -1;
    }

    /* wait for any pending data flows to complete */
    bplib_route_maintenance_complete_wait(s1_rtbl);

    printf("@%lu: Storing payload:\n", (unsigned long)bplib_os_get_dtntime_ms());
    display(stdout, (const uint8_t *)my_payload, 0, sizeof(my_payload));

    lib_status = bplib_send(desc, my_payload, sizeof(my_payload), BP_CHECK);
    if (lib_status != 0)
    {
        fprintf(stderr, "Failed bplib_send() code=%d... exiting\n", lib_status);
        bplib_close_socket(desc);
        return lib_status;
    }

    /* Setting the CLA "up" should cause the previously-blocked bundle to unblock */
    if (bplib_route_intf_set_flags(s1_rtbl, s1_intf_cla, BPLIB_INTF_STATE_OPER_UP) < 0)
    {
        fprintf(stderr, "%s(): bplib_route_intf_set_flags cla failed", __func__);
    }

    /* wait for any pending data flows to complete */
    bplib_route_maintenance_complete_wait(s1_rtbl);

    bundle_sz  = sizeof(bundle_buffer);
    lib_status = bplib_cla_egress(s1_rtbl, s1_intf_cla, bundle_buffer, &bundle_sz, BP_CHECK);
    if (lib_status != 0)
    {
        fprintf(stderr, "Failed bplib_cla_egress() code=%d... exiting\n", lib_status);
        bplib_close_socket(desc);
        return lib_status;
    }

    bplib_close_socket(desc);

    /* wait for any pending data flows to complete */
    bplib_route_maintenance_complete_wait(s1_rtbl);

    printf("@%lu: Bundle content:\n", (unsigned long)bplib_os_get_dtntime_ms());
    display(stdout, bundle_buffer, 0, bundle_sz);

    int fd;
    fd = open("test.cbor", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
    {
        perror("open()");
        return -1;
    }

    if (write(fd, bundle_buffer, bundle_sz) != bundle_sz)
    {
        perror("write");
    }
    close(fd);

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

    bplib_store_cache_debug_scan(s1_rtbl, s1_intf_store);

    /* this does a sanity check on the pool and displays various block allocation stats */
    bplib_mpool_debug_scan(bplib_route_get_mpool(s1_rtbl));

    desc = bplib_create_socket(s2_rtbl);
    if (desc == NULL)
    {
        fprintf(stderr, "Failed bplib_open()... exiting\n");
        return -1;
    }

    source_ipn1 = (bp_ipn_addr_t) {201, 1};
    if (bplib_bind_socket(desc, &source_ipn1) < 0)
    {
        fprintf(stderr, "Failed bplib_bind_socket()... exiting\n");
        bplib_close_socket(desc);
        return -1;
    }

    dest_ipn1 = (bp_ipn_addr_t) {101, 1};
    if (bplib_connect_socket(desc, &dest_ipn1) < 0)
    {
        fprintf(stderr, "Failed bplib_bind_socket()... exiting\n");
        bplib_close_socket(desc);
        return -1;
    }

    lib_status = bplib_cla_ingress(s2_rtbl, s2_intf_cla, bundle_buffer, bundle_sz, BP_CHECK);
    if (lib_status != 0)
    {
        fprintf(stderr, "Failed bplib_cla_ingress() code=%d... exiting\n", lib_status);
        bplib_close_socket(desc);
        return lib_status;
    }

    /* wait for any pending data flows to complete */
    bplib_route_maintenance_complete_wait(s2_rtbl);

    recv_sz    = sizeof(recv_payload);
    lib_status = bplib_recv(desc, recv_payload, &recv_sz, BP_CHECK);
    if (lib_status != 0)
    {
        fprintf(stderr, "Failed bplib_recv() code=%d... exiting\n", lib_status);
        bplib_close_socket(desc);
        return lib_status;
    }

    bplib_close_socket(desc);

    /* wait for any pending data flows to complete */
    bplib_route_maintenance_complete_wait(s2_rtbl);

    printf("@%lu: Received payload:\n", (unsigned long)bplib_os_get_dtntime_ms());
    display(stdout, (const uint8_t *)recv_payload, 0, recv_sz);

    if (recv_sz != sizeof(my_payload))
    {
        fprintf(stderr, "Failed with different size ... %zu vs %zu exiting\n", recv_sz, sizeof(my_payload));
        return lib_status;
    }

    if (memcmp(recv_payload, my_payload, recv_sz) != 0)
    {
        fprintf(stderr, "Failed with different data\n");
        return lib_status;
    }

    /* now wait for the DACS signal, which comes out after a delay */
    printf("@%lu: waiting for ACK Bundle...\n", (unsigned long)bplib_os_get_dtntime_ms());

    bundle_sz  = sizeof(bundle_buffer);
    lib_status = bplib_cla_egress(s2_rtbl, s2_intf_cla, bundle_buffer, &bundle_sz, 5000000);
    if (lib_status != 0)
    {
        fprintf(stderr, "Failed bplib_cla_egress() for DACS  code=%d... exiting\n", lib_status);
        return lib_status;
    }

    /* wait for any pending data flows to complete */
    bplib_route_maintenance_complete_wait(s2_rtbl);

    printf("@%lu: ACK Bundle content:\n", (unsigned long)bplib_os_get_dtntime_ms());
    display(stdout, bundle_buffer, 0, bundle_sz);

    fd = open("ack.cbor", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
    {
        perror("open()");
        return -1;
    }

    if (write(fd, bundle_buffer, bundle_sz) != bundle_sz)
    {
        perror("write");
    }
    close(fd);

    printf("@%lu: sending ACK to originator...\n", (unsigned long)bplib_os_get_dtntime_ms());

    lib_status = bplib_cla_ingress(s1_rtbl, s1_intf_cla, bundle_buffer, bundle_sz, BP_CHECK);
    if (lib_status != 0)
    {
        fprintf(stderr, "Failed bplib_cla_ingress() code=%d... exiting\n", lib_status);
        return lib_status;
    }

    bplib_route_maintenance_complete_wait(s1_rtbl);

    /* let some additional maintenance cycles run */
    printf("@%lu: shutting down...\n", (unsigned long)bplib_os_get_dtntime_ms());
    sleep(2);

    return 0;
}

/******************************************************************************
 * Main
 ******************************************************************************/
int main(int argc, char *argv[])
{
    /* Initialize bplib */
    if (bplib_init() != 0)
    {
        fprintf(stderr, "Failed bplib_init()... exiting\n");
        return EXIT_FAILURE;
    }

    // bplib_crc_init();
    //
    // crc_test(&BPLIB_CRC16_X25);
    // crc_test(&BPLIB_CRC32_CASTAGNOLI);

    if (bplib_route_test() != 0)
    {
        return EXIT_FAILURE;
    }

    if (bplib_start_mgt_task() != 0)
    {
        return EXIT_FAILURE;
    }

    if (bplib2_bundle_test() != 0)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
