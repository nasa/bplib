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
#include <getopt.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "bplib.h"
#include "bplib_os.h"
#include "bplib_routing.h"
#include "bplib_store_ram.h"

#include "v7_rbtree.h"
#include "v7_cache.h"

#include "../cla/udp/inc/claudp.h"  // KRS
#include "../cla/bibe/inc/bibe.h"  // KRS

/* BPCAT_MAX_WAIT_MSEC controls the amount of time waiting for reading/writing to queues/files by
 * the data mover threads.  This is time limited so the "app_running" flag is checked periodically.
 * Normally it should be fairly short so that the program responds to CTRL+C in a fairly timely
 * manner.  But when debugging, it is sometimes helpful to use a very long timeout. */
//#define BPCAT_MAX_WAIT_MSEC         1800000
#define BPCAT_MAX_WAIT_MSEC 250

#define BPCAT_DATA_MESSAGE_MAX_SIZE 2552
#define BPCAT_BUNDLE_BUFFER_SIZE    (sizeof(bpcat_msg_content_t) + 512)
#define BPCAT_RECV_WINDOW_SZ        32

/*************************************************************************
 * File Data
 *************************************************************************/

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

typedef struct bpcat_msg_recv
{
    bplib_rbt_link_t    link;
    bpcat_msg_content_t msg;
} bpcat_msg_recv_t;

static bpcat_msg_recv_t recv_window[BPCAT_RECV_WINDOW_SZ];

//static volatile sig_atomic_t app_running;
volatile sig_atomic_t app_running;  // CLA moved out of this file  KRS

static const char ADDRESS_PREFIX[]           = "ipn://";
static char       local_address_string[128]  = "ipn://100.1";
//KRS static char       remote_address_string[128] = "ipn://101.1";
static char       remote_address_string[128] = "ipn://101.2";

pthread_t cla_in_task;
pthread_t cla_out_task;
pthread_t app_out_task;
pthread_t app_in_task;

/******************************************************************************
 * Local Functions
 ******************************************************************************/

/*
 * app_quick_exit - Signal handler for Control-C
 */
static void app_quick_exit(int signo)
{
    static const char message[] = "Caught CTRL+C\n";

    signal(signo, SIG_DFL);
    write(STDERR_FILENO, message, sizeof(message) - 1);
    app_running = 0;
}

static void display_banner(const char *prog_name)
{
    /* Display Welcome Banner */
    fprintf(stderr, "Usage: %s [options]\n", prog_name);
    fprintf(stderr, "   -l/--local-addr=ipn://<node>.<service> local address to use\n");
    fprintf(stderr, "   -r/--remote-addr=ipn://<node>.<service> remote address to use\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "   Creates a local BP agent with local IPN address as specified.  All data\n");
    fprintf(stderr, "   received from standard input is forwarded over BP bundles, and all data\n");
    fprintf(stderr, "   received from bundles is forwarded to standard output.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Example:\n");
    fprintf(stderr, "   %s -l ipn://101.1 -r ipn://201.1\n\n", prog_name);

    exit(-1);
}

static void parse_address(const char *string_addr, bp_ipn_addr_t *parsed_addr)
{
    const char *start;
    char       *end;

    if (strncmp(string_addr, ADDRESS_PREFIX, sizeof(ADDRESS_PREFIX) - 1) != 0)
    {
        fprintf(stderr, "IPN address string not well formed, must start with %s\n", ADDRESS_PREFIX);
        abort();
    }

    /* get node part */
    start                    = &string_addr[sizeof(ADDRESS_PREFIX) - 1];
    parsed_addr->node_number = strtoul(start, &end, 0);
    if (end != NULL && *end == '.')
    {
        /* get service part */
        start                       = end + 1;
        parsed_addr->service_number = strtoul(start, &end, 0);
    }
    else
    {
        parsed_addr->service_number = 0;
    }

    if (end != NULL && *end != 0)
    {
        fprintf(stderr, "IPN address string not well formed, trailing data: %s\n", end);
        abort();
    }

    fprintf(stderr, "Parsed address: %s%lu.%lu\n", ADDRESS_PREFIX, (unsigned long)parsed_addr->node_number,
            (unsigned long)parsed_addr->service_number);
}

void redirect_file(int fileno, const char *filename)
{
    int flags;
    int rfd;

    if (fileno == STDIN_FILENO)
    {
        /* when redirecting input, do read only */
        flags = O_RDONLY;
    }
    else
    {
        /* when redirecting output, do write only */
        flags = O_WRONLY | O_CREAT | O_TRUNC;
    }

    rfd = open(filename, flags, 0664);
    if (rfd < 0)
    {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    if (dup2(rfd, fileno) < 0)
    {
        perror("dup2()");
        exit(EXIT_FAILURE);
    }

    close(rfd);
}

static void parse_options(int argc, char *argv[])
{
    /*
     * getopts parameter passing options string
     */
    static const char *opt_string = "l:r:i:o:?";

    /*
     * getopts_long long form argument table
     */
    static const struct option long_opts[] = {{"local-addr", required_argument, NULL, 'l'},
                                              {"remote-addr", required_argument, NULL, 'r'},
                                              {"input-file", required_argument, NULL, 'i'},
                                              {"output-file", required_argument, NULL, 'o'},
                                              {"help", no_argument, NULL, '?'},
                                              {NULL, no_argument, NULL, 0}};

    int         opt;
    int         longopt_index;
    const char *env_str;

    /* Initialize from Environment */
    env_str = getenv("BP_LOCAL_ADDRESS");
    if (env_str != NULL)
    {
        strncpy(local_address_string, env_str, sizeof(local_address_string) - 1);
        local_address_string[sizeof(local_address_string) - 1] = 0;
    }

    env_str = getenv("BP_REMOTE_ADDRESS");
    if (env_str != NULL)
    {
        strncpy(remote_address_string, env_str, sizeof(remote_address_string) - 1);
        remote_address_string[sizeof(remote_address_string) - 1] = 0;
    }

    do
    {
        opt = getopt_long(argc, argv, opt_string, long_opts, &longopt_index);
        if (opt < 0)
        {
            /* end of options */
            break;
        }
        switch (opt)
        {
            case 'l':
                strncpy(local_address_string, optarg, sizeof(local_address_string) - 1);
                local_address_string[sizeof(local_address_string) - 1] = 0;
                break;

            case 'r':
                strncpy(remote_address_string, optarg, sizeof(remote_address_string) - 1);
                remote_address_string[sizeof(remote_address_string) - 1] = 0;
                break;

            case 'i':
                redirect_file(STDIN_FILENO, optarg);
                break;

            case 'o':
                redirect_file(STDOUT_FILENO, optarg);
                break;

            default:
                display_banner(argv[0]);
                break;
        }
    }
    while (true);
}

#define join_thread(tsk) do_join_thread(#tsk, tsk##_task)
static void do_join_thread(const char *name, pthread_t task)
{
    int status;

    status = pthread_join(task, NULL);
    if (status < 0)
    {
        fprintf(stderr, "Failed to join %s: %d\n", name, status);
    }
}

#define start_thread(tsk, obj) do_start_thread(#tsk, &tsk##_task, tsk##_entry, obj)
static void do_start_thread(const char *name, pthread_t *task, void *(*entry)(void *), void *arg)
{
    int status;

    status = pthread_create(task, NULL, entry, arg);
    if (status < 0)
    {
        fprintf(stderr, "pthread_create(%s): %d, %s\n", name, status, strerror(status));
        abort();
    }

    fprintf(stderr, "bpcat started %s\n", name);
}

#if 0
moved to real cla // KRS
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
                fprintf(stderr, "poll() reported error=%d (%s)...\n", error, strerror(error));

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
                fprintf(stderr, "poll() revent=0x%x...\n", (unsigned int)pfd.revents);
            }
        }
        else
        {
            fprintf(stderr, "Call system bplib_cla_ingress()... size=%zu\n", data_fill_sz);
            status = bplib_cla_ingress(cla->rtbl, cla->intf_id, bundle_buffer, data_fill_sz, BPCAT_MAX_WAIT_MSEC);
            if (status == BP_SUCCESS)
            {
                data_fill_sz = 0;
            }
            else if (status != BP_TIMEOUT)
            {
                fprintf(stderr, "Failed bplib_cla_ingress() code=%zd... exiting\n", status);
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
            status = bplib_cla_egress(cla->rtbl, cla->intf_id, bundle_buffer, &cla_bundle_sz, BPCAT_MAX_WAIT_MSEC);
            if (status == BP_SUCCESS)
            {
                data_fill_sz = cla_bundle_sz;
            }
            else if (status != BP_TIMEOUT)
            {
                fprintf(stderr, "Failed bplib_cla_egress() code=%zd... exiting\n", status);
                break;
            }
        }
        else
        {
            fprintf(stderr, "Call system send()... size=%zu\n", data_fill_sz);
            status = send(cla->sys_fd, bundle_buffer, data_fill_sz, MSG_DONTWAIT);
            if (status == data_fill_sz)
            {
                data_fill_sz = 0;
            }
            else if (errno == ECONNREFUSED)
            {
                fprintf(stderr, "Connection refused sending to remote (continuing)\n");
            }
            else if (errno != EWOULDBLOCK && errno != EAGAIN)
            {
                fprintf(stderr, "Failed send() errno=%d (%s)\n", errno, strerror(errno));
                break;
            }
        }
    }

    return NULL;
}
#endif

#if 0  // KRS
functionality moved to real cla
static int setup_cla(bplib_routetbl_t *rtbl, uint16_t local_port, uint16_t remote_port)
{
    static bplib_cla_intf_id_t cla_intf_id; /* static because its passed to pthread_create() */
    struct sockaddr_in         addr;

    /* Create bplib CLA and default route */
    cla_intf_id.rtbl    = rtbl;
    cla_intf_id.intf_id = bplib_create_cla_intf(rtbl);
    if (!bp_handle_is_valid(cla_intf_id.intf_id))
    {
        fprintf(stderr, "%s(): bplib_create_cla_intf failed\n", __func__);
        return -1;
    }

    if (bplib_route_add(rtbl, 0, 0, cla_intf_id.intf_id) < 0)
    {
        fprintf(stderr, "%s(): bplib_route_add cla failed\n", __func__);
        return -1;
    }

    if (bplib_route_intf_set_flags(rtbl, cla_intf_id.intf_id, BPLIB_INTF_STATE_ADMIN_UP | BPLIB_INTF_STATE_OPER_UP) < 0)
    {
        fprintf(stderr, "%s(): bplib_route_intf_set_flags cla failed\n", __func__);
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
#endif

static int setup_storage(bplib_routetbl_t *rtbl, const bp_ipn_addr_t *storage_addr)
{
    bp_handle_t intf_id;

    intf_id = bplib_create_node_intf(rtbl, storage_addr->node_number);
    if (!bp_handle_is_valid(intf_id))
    {
        fprintf(stderr, "%s(): bplib_create_node_intf failed\n", __func__);
        return -1;
    }
    if (bplib_route_intf_set_flags(rtbl, intf_id, BPLIB_INTF_STATE_ADMIN_UP | BPLIB_INTF_STATE_OPER_UP) < 0)
    {
        fprintf(stderr, "%s(): bplib_route_intf_set_flags data1 failed\n", __func__);
        return -1;
    }

    intf_id = bplib_create_file_storage(rtbl, storage_addr);
    if (!bp_handle_is_valid(intf_id))
    {
        fprintf(stderr, "%s(): bplib_create_file_storage failed\n", __func__);
        return -1;
    }
    if (bplib_route_intf_set_flags(rtbl, intf_id, BPLIB_INTF_STATE_ADMIN_UP | BPLIB_INTF_STATE_OPER_UP) < 0)
    {
        fprintf(stderr, "%s(): bplib_route_intf_set_flags storage failed\n", __func__);
        return -1;
    }

    return 0;
}

static void *app_in_entry(void *arg)
{
    bp_socket_t        *desc;
    uint64_t            send_deadline;
    uint64_t            current_time;
    uint64_t            timeout;
    bpcat_msg_content_t msg_buffer;
    uint32_t            data_fill_sz;
    uint32_t            stream_pos;
    ssize_t             status;
    struct pollfd       pfd;
    int                 app_fd;
    bool                got_eof;

    got_eof       = false;
    desc          = arg;
    data_fill_sz  = 0;
    stream_pos    = 0;
    send_deadline = BP_DTNTIME_INFINITE;
    app_fd        = STDIN_FILENO;

    while (app_running)
    {
        if (data_fill_sz > 0)
        {
            current_time = bplib_os_get_dtntime_ms();
        }
        else
        {
            current_time = 0;
        }

        if (send_deadline > current_time && data_fill_sz < sizeof(msg_buffer.content))
        {
            pfd.fd      = app_fd;
            pfd.events  = POLLIN;
            pfd.revents = 0;
            timeout     = send_deadline - current_time;
            if (timeout > BPCAT_MAX_WAIT_MSEC)
            {
                timeout = BPCAT_MAX_WAIT_MSEC;
            }
            if (poll(&pfd, 1, timeout) < 0)
            {
                perror("poll()");
                break;
            }

            if ((pfd.revents & POLLERR) != 0)
            {
                /* not expected on stdin... */
                assert(0);
            }
            if ((pfd.revents & POLLIN) != 0)
            {
                status = read(app_fd, &msg_buffer.content[data_fill_sz], sizeof(msg_buffer.content) - data_fill_sz);
                fprintf(stderr, "read()... size=%ld\n", (long)status);
                if (status < 0)
                {
                    perror("read()");
                    break;
                }

                if (status == 0)
                {
                    /* this typically means EOF */
                    got_eof       = true;
                    send_deadline = 0;
                }
                else
                {
                    data_fill_sz += status;
                    send_deadline = bplib_os_get_dtntime_ms() + 250;
                }
            }
            else
            {
                /* probably a timeout */
                status = 0;
            }
        }
        else if (data_fill_sz > 0)
        {
            msg_buffer.segment_len = htonl(data_fill_sz);
            msg_buffer.stream_pos  = htonl(stream_pos);
            stream_pos += data_fill_sz;
            fprintf(stderr, "Call bplib_send()... size=%lu\n", (unsigned long)data_fill_sz);
            status = bplib_send(desc, &msg_buffer, &msg_buffer.content[data_fill_sz] - (uint8_t *)&msg_buffer,
                                BPCAT_MAX_WAIT_MSEC);
            if (status == BP_SUCCESS)
            {
                /* reset the buffer */
                data_fill_sz = 0;
                if (!got_eof)
                {
                    send_deadline = BP_DTNTIME_INFINITE;
                }
            }
            else if (status != BP_TIMEOUT)
            {
                fprintf(stderr, "Failed bplib_send() code=%zd... exiting\n", status);
                break;
            }
        }
        else if (got_eof)
        {
            fprintf(stderr, "Terminating app_in thread at EOF...\n");
            break;
        }
    }

    return NULL;
}

static void *app_out_entry(void *arg)
{
    bp_socket_t      *desc;
    size_t            recv_sz;
    ssize_t           status;
    bpcat_msg_recv_t *curr_bp_buffer;
    bpcat_msg_recv_t *curr_app_buffer;

    bp_val_t         curr_stream_pos;
    bp_val_t         total_message_count;
    bplib_rbt_root_t free_root;
    bplib_rbt_root_t recv_root;
    bplib_rbt_iter_t iter;

    uint32_t segment_remain;
    uint32_t segment_offset;
    int32_t  segment_distance;

    desc                = arg;
    curr_bp_buffer      = NULL;
    curr_stream_pos     = 0;
    total_message_count = 0;
    segment_remain      = 0;
    segment_offset      = 0;

    bplib_rbt_init_root(&free_root);
    bplib_rbt_init_root(&recv_root);

    for (total_message_count = 0; total_message_count < BPCAT_RECV_WINDOW_SZ; ++total_message_count)
    {
        bplib_rbt_insert_value_unique(total_message_count, &free_root, &recv_window[total_message_count].link);
    }

    while (app_running)
    {
        if (curr_app_buffer == NULL)
        {
            if (bplib_rbt_iter_goto_min(0, &recv_root, &iter) == BP_SUCCESS)
            {
                if (bplib_rbt_get_key_value(iter.position) <= curr_stream_pos)
                {
                    curr_app_buffer = (bpcat_msg_recv_t *)iter.position;

                    segment_remain  = curr_app_buffer->msg.segment_len;
                    segment_offset  = 0;
                    curr_stream_pos = bplib_rbt_get_key_value(iter.position) + curr_app_buffer->msg.segment_len;

                    /* note this must be done last because it clobbers the value */
                    bplib_rbt_extract_node(&recv_root, &curr_app_buffer->link);
                }
            }
        }

        if (curr_bp_buffer == NULL)
        {
            if (bplib_rbt_iter_goto_min(0, &free_root, &iter) == BP_SUCCESS)
            {
                curr_bp_buffer = (bpcat_msg_recv_t *)iter.position;
                bplib_rbt_extract_node(&free_root, &curr_bp_buffer->link);
            }
        }

        if (curr_app_buffer != NULL)
        {
            fprintf(stderr, "Call system write()... offset=%lu size=%lu\n", (unsigned long)segment_offset,
                    (unsigned long)segment_remain);
            status = write(STDOUT_FILENO, &curr_app_buffer->msg.content[segment_offset], segment_remain);
            if (status < 0)
            {
                perror("write()");
                break;
            }

            segment_offset += status;
            segment_remain -= status;

            if (segment_remain == 0)
            {
                bplib_rbt_insert_value_unique(total_message_count, &free_root, &curr_app_buffer->link);
                ++total_message_count;
                curr_app_buffer = NULL;
            }
        }
        else if (curr_bp_buffer != NULL)
        {
            recv_sz = sizeof(bpcat_msg_content_t);
            status  = bplib_recv(desc, &curr_bp_buffer->msg, &recv_sz, BPCAT_MAX_WAIT_MSEC);
            if (status == BP_SUCCESS && recv_sz >= offsetof(bpcat_msg_content_t, content))
            {
                curr_bp_buffer->msg.stream_pos  = ntohl(curr_bp_buffer->msg.stream_pos);
                curr_bp_buffer->msg.segment_len = ntohl(curr_bp_buffer->msg.segment_len);

                segment_distance = curr_bp_buffer->msg.stream_pos - ((uint32_t)curr_stream_pos & 0xFFFFFFFF);

                bplib_rbt_insert_value_unique(curr_stream_pos + segment_distance, &recv_root, &curr_bp_buffer->link);
                curr_bp_buffer = NULL;
            }
            else if (status != BP_TIMEOUT)
            {
                fprintf(stderr, "Failed bplib_recv() code=%zd... exiting\n", status);
                break;
            }
        }
        else
        {
            /* This means the free tree was empty, so theoretically everything must be in the recv tree */
            if (bplib_rbt_iter_goto_min(0, &recv_root, &iter) == BP_SUCCESS)
            {
                fprintf(stderr, "Gap in sequence, skipping offset %lu -> %lu\n", (unsigned long)curr_stream_pos,
                        (unsigned long)bplib_rbt_get_key_value(iter.position));

                curr_stream_pos = bplib_rbt_get_key_value(iter.position);
            }
            else
            {
                fprintf(stderr, "free_root and recv_root both empty, bug?... exiting\n");
                break;
            }
        }
    }

    return NULL;
}

static int setup_connection(bplib_routetbl_t *rtbl, const bp_ipn_addr_t *local_addr, const bp_ipn_addr_t *remote_addr)
{
    bp_socket_t *desc;

    /* Create bplib application data socket */
    desc = bplib_create_socket(rtbl);
    if (desc == NULL)
    {
        fprintf(stderr, "Failed bplib_open()... exiting\n");
        return -1;
    }

    if (bplib_bind_socket(desc, local_addr) < 0)
    {
        fprintf(stderr, "Failed bplib_bind_socket()... exiting\n");
        bplib_close_socket(desc);
        return -1;
    }

    if (bplib_connect_socket(desc, remote_addr) < 0)
    {
        fprintf(stderr, "Failed bplib_bind_socket()... exiting\n");
        bplib_close_socket(desc);
        return -1;
    }

    /* Create APP Thread */
    start_thread(app_in, desc);
    start_thread(app_out, desc);

    return 0;
}

/******************************************************************************
 * Main
 ******************************************************************************/
int main(int argc, char *argv[])
{
    bplib_routetbl_t *rtbl;
    bp_ipn_addr_t     local_addr;
    bp_ipn_addr_t     remote_addr;
    bp_ipn_addr_t     storage_addr;
    uint64_t          stats_time;
    uint64_t          curr_time;

    app_running = 1;
    signal(SIGINT, app_quick_exit);

    /* Initialize bplib */
    if (bplib_init() != 0)
    {
        fprintf(stderr, "Failed bplib_init()... exiting\n");
        return EXIT_FAILURE;
    }

    bplib_os_enable_log_flags(0xFFFFFFFF);

    /* Process Command Line */
    parse_options(argc, argv);
    parse_address(local_address_string, &local_addr);
    parse_address(remote_address_string, &remote_addr);

    /* Test route table with 1MB of cache */
    rtbl = bplib_route_alloc_table(10, 1 << 20);
    if (rtbl == NULL)
    {
        fprintf(stderr, "%s(): bplib_route_alloc_table failed\n", __func__);
        return EXIT_FAILURE;
    }

    /* this currently assumes service number 10 for storage, should be configurable */
    storage_addr = (bp_ipn_addr_t) {local_addr.node_number, 10};
    if (setup_storage(rtbl, &storage_addr) < 0)
    {
        fprintf(stderr, "Failed setup_storage()... exiting\n");
        return EXIT_FAILURE;
    }

    if (setup_cla_bibe(rtbl, local_addr.node_number, 36400 + local_addr.service_number < 0,
                       remote_addr.node_number, remote_addr.service_number)  // KRS
    {
        fprintf(stderr, "Failed setup_cla_bibe()... exiting\n");
        return EXIT_FAILURE;
    }

#if 0  // KRS
    if (setup_cla(rtbl, 36400 + local_addr.node_number, 36400 + remote_addr.node_number) < 0)
#endif
    if (setup_cla_udp(rtbl, 36400 + local_addr.service_number, NULL, 36400 + remote_addr.service_number) < 0)
    {
        fprintf(stderr, "Failed setup_cla()... exiting\n");
        return EXIT_FAILURE;
    }

    if (setup_connection(rtbl, &local_addr, &remote_addr) < 0)
    {
        fprintf(stderr, "Failed setup_connection()... exiting\n");
        return EXIT_FAILURE;
    }

    /* Run management Loop */
    stats_time = bplib_os_get_dtntime_ms() + 10000;
    while (app_running)
    {
        bplib_route_maintenance_request_wait(rtbl);

        // fprintf(stderr, "@%lu: Maintenance running...\n", (unsigned long)bplib_os_get_dtntime_ms());

        /* do maintenance regardless of what the "request" returned, as that
         * currently only reflects actual requests, not time-based poll actions */
        bplib_route_periodic_maintenance(rtbl);

        curr_time = bplib_os_get_dtntime_ms();
        if (curr_time >= stats_time)
        {
            //KRS bplib_cache_debug_scan(rtbl, storage_intf_id);
            stats_time += 10000;
        }
    }

    /* Join Threads */
    join_thread(app_in);
    join_thread(app_out);
#if 0  // KRS
    join_thread(cla_in);
    join_thread(cla_out);
#endif
    teardown_cla_udp();
    teardown_cla_bibe();

    return 0;
}
