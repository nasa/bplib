/************************************************************************
 * File: claudp.c
 *
 * This software was created at NASA's Goddard Space Flight Center.
 *
 * Maintainer(s):
 *  Karl Sipfle, for Code 580 NASA GSFC
 *
 * Copyright © 2020-2022 United States Government as represented by 
 * the Administrator of
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
 *************************************************************************/

/*
    app: lib_status = bplib_send(desc, my_payload, sizeof(my_payload), BP_CHECK);                   // instead of v6 store
    cla: lib_status = bplib_cla_egress(s1_rtbl, s1_intf_cla, bundle_buffer, &bundle_sz, BP_CHECK);  // instead of v6 load
    cla: lib_status = bplib_cla_ingress(s2_rtbl, s2_intf_cla, bundle_buffer, bundle_sz, BP_CHECK);  // instead of v6 process
    app: lib_status = bplib_recv(desc, recv_payload, &recv_sz, BP_CHECK);                           // instead of v6 accept
*/

static int trace = 1;  // KRS
//static int brief = 0;  // KRS
//static int dump_enable = 1;  // KRS

#define BPCAT_BUNDLE_BUFFER_SIZE (sizeof(bpcat_msg_content_t) + 512)   // HARDCODED KRS
//#define BPCAT_MAX_WAIT_MSEC         1800000                           
#define BPCAT_MAX_WAIT_MSEC 250  // HARDCODED KRS                                
#define BPCAT_DATA_MESSAGE_MAX_SIZE 2552  // HARDCODED KRS

#include <stdint.h>  //
#include <stdbool.h> //
#include <stdio.h>
#include <stdlib.h>  //
#include <signal.h>
#include <pthread.h>
// #include <unistd.h>  //
#include <errno.h>

#include "bplib.h"
//#include "bp/bplib_store_ram.h"
//#include "../bplibk/inc/bplib.h"
//#include "../bplibk/inc/bplib_store_ram.h"

//#include "bpio.h"

#include "bplib_api_types.h"
#include <sys/socket.h>
#include <poll.h>
#include <netinet/in.h>
#include "bplib_routing.h"
#include <string.h>

//#include "bibe.h"
#include "../cla/bibe/inc/bibe.h"

typedef struct bplib_cla_intf_id
{
    bplib_routetbl_t *rtbl;
    bp_handle_t       intf_id;
    int               sys_fd;
} bplib_cla_intf_id_t;

typedef struct bpcat_msg_content
{
    uint32_t stream_pos;
    uint32_t segment_len;
    uint8_t  content[BPCAT_DATA_MESSAGE_MAX_SIZE];
} bpcat_msg_content_t;

pthread_t cla_in_task;
pthread_t cla_out_task;

extern volatile sig_atomic_t app_running;  ////

#define join_thread(tsk) do_join_thread(#tsk, tsk##_task)
static void do_join_thread(const char *name, pthread_t task)
{
    int status;

    status = pthread_join(task, NULL);
    if (status < 0)
    {
        if (trace) fprintf(stderr, "Failed to join %s: %d\n", name, status);
    }
}

#define start_thread(tsk, obj) do_start_thread(#tsk, &tsk##_task, tsk##_entry, obj)
static void do_start_thread(const char *name, pthread_t *task, void *(*entry)(void *), void *arg)
{
    int status;

    status = pthread_create(task, NULL, entry, arg);
    if (status < 0)
    {
        if (trace) fprintf(stderr, "pthread_create(%s): %d, %s\n", name, status, strerror(status));
        abort();
    }

    if (trace) fprintf(stderr, "claudp started %s\n", name);
}

static void *cla_in_entry(void *arg)
{
    bplib_cla_intf_id_t *cla;
    ssize_t              status;
    size_t               data_fill_sz;
    uint8_t              bundle_buffer[BPCAT_BUNDLE_BUFFER_SIZE];
    struct pollfd        pfd;
    int                  error;
    socklen_t            errlen;

    cla          = arg;
    data_fill_sz = 0;
    error        = 0;

    while (app_running)
    {
        if (data_fill_sz == 0)
        {
            pfd.fd      = cla->sys_fd;
            pfd.events  = POLLIN;
            pfd.revents = 0;
            if (poll(&pfd, 1, BPCAT_MAX_WAIT_MSEC) < 0)
            {
                perror("poll()");
                break;
            }

            if ((pfd.revents & POLLERR) != 0)
            {
                /* some other condition is present, possibly an error code */
                errlen = sizeof(error);
                getsockopt(cla->sys_fd, SOL_SOCKET, SO_ERROR, (void *)&error, &errlen);
                if (trace) fprintf(stderr, "poll() reported error=%d (%s)...\n", error, strerror(error));

                /* connection refused generally just means the bpcat is not running on the other
                 * side and thus the datagram was rejected.  This will get resolved if/when the
                 * program is started, so just keep going for now. */
                if (error != ECONNREFUSED)
                {
                    break;
                }

                pfd.revents &= ~POLLERR;
            }

            if ((pfd.revents & POLLIN) != 0)
            {
                status = recv(cla->sys_fd, bundle_buffer, sizeof(bundle_buffer), MSG_DONTWAIT);
                if (status < 0)
                {
                    perror("recv()");
                    break;
                }

                data_fill_sz = status;
                pfd.revents &= ~POLLIN;
            }

            /* Something unexpected */
            if (pfd.revents != 0)
            {
                if (trace) fprintf(stderr, "poll() revents=0x%x...\n", (unsigned int)pfd.revents);
            }
        }
        else
        {
            if (trace) fprintf(stderr, "Call system bplib_bibe_ingress()... size=%zu\n", data_fill_sz);
            status = bplib_bibe_ingress(cla->rtbl, cla->intf_id, bundle_buffer, data_fill_sz, BPCAT_MAX_WAIT_MSEC);
            if (status == BP_SUCCESS)
            {
                data_fill_sz = 0;
            }
            else if (status != BP_TIMEOUT)
            {
                if (trace) fprintf(stderr, "Failed ingress code=%zd... exiting\n", status);
                break;
            }
        }
    }

    return NULL;
}

static void *cla_out_entry(void *arg)
{
    bplib_cla_intf_id_t *cla;
    size_t               data_fill_sz;
    size_t               cla_bundle_sz;
    ssize_t              status;
    uint8_t              bundle_buffer[BPCAT_BUNDLE_BUFFER_SIZE];

    cla          = arg;
    data_fill_sz = 0;

    while (app_running)
    {
        if (data_fill_sz == 0)
        {
            cla_bundle_sz = sizeof(bundle_buffer);
            status = bplib_bibe_egress(cla->rtbl, cla->intf_id, bundle_buffer, &cla_bundle_sz, BPCAT_MAX_WAIT_MSEC);
            if (status == BP_SUCCESS)
            {
                data_fill_sz = cla_bundle_sz;
            }
            else if (status != BP_TIMEOUT)
            {
                if (trace) fprintf(stderr, "Failed egress code=%zd... exiting\n", status);
                break;
            }
        }
        else
        {
            if (trace) fprintf(stderr, "Call system send()... size=%zu\n", data_fill_sz);
            status = send(cla->sys_fd, bundle_buffer, data_fill_sz, MSG_DONTWAIT);
            if (status == data_fill_sz)
            {
                data_fill_sz = 0;
            }
            else if (errno == ECONNREFUSED)
            {
                if (trace) fprintf(stderr, "Connection refused sending to remote (continuing)\n");
            }
            else if (errno != EWOULDBLOCK && errno != EAGAIN)
            {
                if (trace) fprintf(stderr, "Failed send() errno=%d (%s)\n", errno, strerror(errno));
                break;
            }
        }
    }

    return NULL;
}

int setup_cla_udp(bplib_routetbl_t *rtbl, uint16_t local_port, char *remote_host, uint16_t remote_port)
{
    //attributes.lifetime = lifetime;
    //attributes.timeout = timeout;

    static bplib_cla_intf_id_t cla_intf_id; /* static because it's passed to pthread_create() */
    struct sockaddr_in         addr;

    /* Create bplib CLA and default route */
    cla_intf_id.rtbl    = rtbl;
    cla_intf_id.intf_id = bplib_create_cla_intf(rtbl);
    if (!bp_handle_is_valid(cla_intf_id.intf_id))
    {
        if (trace) fprintf(stderr, "%s(): bplib_create_cla_intf failed\n", __func__);
        return -1;
    }

    if (bplib_route_add(rtbl, 0, 0, cla_intf_id.intf_id) < 0)
    {
        if (trace) fprintf(stderr, "%s(): bplib_route_add cla failed\n", __func__);
        return -1;
    }

    if (bplib_route_intf_set_flags(rtbl, cla_intf_id.intf_id, BPLIB_INTF_STATE_ADMIN_UP | BPLIB_INTF_STATE_OPER_UP) < 0)
    {
        if (trace) fprintf(stderr, "%s(): bplib_route_intf_set_flags cla failed\n", __func__);
        return -1;
    }

    /* open a UDP socket on the loopback address */
    cla_intf_id.sys_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (cla_intf_id.sys_fd < 0)
    {
        perror("socket()");
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(local_port);
    addr.sin_addr.s_addr = (in_addr_t)htonl(INADDR_LOOPBACK);
    if (bind(cla_intf_id.sys_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind()");
        return -1;
    }

    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(remote_port);
    addr.sin_addr.s_addr = (in_addr_t)htonl(INADDR_LOOPBACK);
    if (connect(cla_intf_id.sys_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect()");
        return -1;
    }

    /* Create CLA Threads, one for each direction */
    /* This is currently necessary because they pend on different things */
    start_thread(cla_in, &cla_intf_id);
    start_thread(cla_out, &cla_intf_id);

    return 0;
}

int teardown_cla_udp()
{
    join_thread(cla_in);
    join_thread(cla_out);
}
