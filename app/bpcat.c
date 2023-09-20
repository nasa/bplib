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
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "bplib.h"
#include "bplib_os.h"
#include "bplib_routing.h"

#include "v7_rbtree.h"
#include "v7_cache.h"

/* BPCAT_MAX_WAIT_MSEC controls the amount of time waiting for reading/writing to queues/files by
 * the data mover threads.  This is time limited so the "app_running" flag is checked periodically.
 * Normally it should be fairly short so that the program responds to CTRL+C in a fairly timely
 * manner.  But when debugging, it is sometimes helpful to use a very long timeout. */
//#define BPCAT_MAX_WAIT_MSEC         1800000
#define BPCAT_MAX_WAIT_MSEC 250

#define BPCAT_BUNDLE_BUFFER_SIZE    16384
#define BPCAT_DATA_MESSAGE_MAX_SIZE (BPCAT_BUNDLE_BUFFER_SIZE - 520)
#define BPCAT_RECV_WINDOW_SZ        32

#define BPCAT_DEFAULT_LOCAL_NODENUM  100
#define BPCAT_DEFAULT_REMOTE_NODENUM 101
#define BPCAT_DEFAULT_SERVICENUM     1
#define BPCAT_DEFAULT_UDP_PORT_BASE  36400

/*************************************************************************
 * File Data
 *************************************************************************/

typedef struct bplib_cla_intf_id
{
    bplib_routetbl_t *rtbl;
    bp_handle_t       intf_id;
    int               sys_fd;

    const struct sockaddr *remote_addr;
    socklen_t              remote_addr_len;
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

static volatile sig_atomic_t app_running;

static const char IPN_ADDRESS_PREFIX[] = "ipn://";
static const char UDP_ADDRESS_PREFIX[] = "udp://";

#define ADDR_MAX_SIZE 128

static char local_dtnaddr_string[ADDR_MAX_SIZE];
static char remote_dtnaddr_string[ADDR_MAX_SIZE];
static char local_ipaddr_string[ADDR_MAX_SIZE];
static char remote_ipaddr_string[ADDR_MAX_SIZE];

static long inter_bundle_delay = 20;

pthread_t cla_in_task;
pthread_t cla_out_task;
pthread_t app_out_task;
pthread_t app_in_task;

bp_handle_t storage_intf_id;

/******************************************************************************
 * Local Functions
 ******************************************************************************/

/*
 * app_quick_exit - Signal handler for Control-C
 */
static void app_quick_exit(int signo)
{
    static const char message[] = "Caught CTRL+C\n";
    ssize_t           sz;

    signal(signo, SIG_DFL);

    /* squench warning about unused result - no recourse on failure to write to STDERR */
    sz = write(STDERR_FILENO, message, sizeof(message) - 1);
    (void)sz;
    app_running = 0;
}

static void display_banner(const char *prog_name)
{
    /* Display Welcome Banner */
    fprintf(stderr, "Usage: %s [options]\n", prog_name);
    fprintf(stderr, "   -l/--local-addr=ipn://<node>.<service> local address to use\n");
    fprintf(stderr, "   -r/--remote-addr=ipn://<node>.<service> remote address to use\n");
    fprintf(stderr, "   -i/--input-file=<filename> read input from given file instead of stdin\n");
    fprintf(stderr, "   -o/--output-file=<filename> write output to given file instead of stdout\n");
    fprintf(stderr, "      --local-cla-uri=udp://<ip>:<port> Bind local CLA to given IP:port \n");
    fprintf(stderr, "      --remote-cla-uri=udp://<ip>:<port> Send bundles to remote CLA at given IP:port\n");
    fprintf(stderr, "   -d/--delay=<msec> forced inter bundle send delay (20ms default)\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "   Creates a local BP agent with local IPN address as specified.  All data\n");
    fprintf(stderr, "   received from standard input is forwarded over BP bundles, and all data\n");
    fprintf(stderr, "   received from bundles is forwarded to standard output.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Example:\n");
    fprintf(stderr, "   %s -l ipn://101.1 -r ipn://201.1\n\n", prog_name);

    exit(-1);
}

static void parse_ipn_address(char *buffer, size_t buf_size, bp_ipn_addr_t *parsed_addr, uint32_t dfl_nodenum,
                              uint32_t dfl_svcnum)
{
    char *start;
    char *end;

    /* Start by setting defaults into the output struct */
    parsed_addr->node_number    = dfl_nodenum;
    parsed_addr->service_number = dfl_svcnum;

    /* If the buffer is completely empty, then skip the rest - use all defaults */
    if (buffer != NULL && *buffer != 0)
    {
        if (strncmp(buffer, IPN_ADDRESS_PREFIX, sizeof(IPN_ADDRESS_PREFIX) - 1) != 0)
        {
            fprintf(stderr, "IPN address string not well formed, must start with %s\n", IPN_ADDRESS_PREFIX);
            abort();
        }

        /* get node part */
        start = &buffer[sizeof(IPN_ADDRESS_PREFIX) - 1];

        parsed_addr->node_number = strtoul(start, &end, 0);
        if (end != NULL && *end == '.')
        {
            /* get service part */
            *end  = 0;
            start = end + 1;

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
    }

    snprintf(buffer, buf_size, "%s%lu.%lu", IPN_ADDRESS_PREFIX, (unsigned long)parsed_addr->node_number,
             (unsigned long)parsed_addr->service_number);
}

static void parse_ip_address(char *buffer, size_t buf_size, struct sockaddr_in *parsed_addr, in_addr_t dfl_host,
                             in_port_t dfl_port)
{
    char *host_start;
    char *port_start;
    char *end;
    long  value;

    /* Start by setting defaults into the output struct */
    parsed_addr->sin_family      = AF_INET;
    parsed_addr->sin_port        = htons(dfl_port);
    parsed_addr->sin_addr.s_addr = htonl(dfl_host);

    /* If the buffer is completely empty, then skip the rest - use all defaults */
    if (buffer != NULL && *buffer != 0)
    {
        /* Otherwise this must be a UDP URI */
        if (strncmp(buffer, UDP_ADDRESS_PREFIX, sizeof(UDP_ADDRESS_PREFIX) - 1) != 0)
        {
            fprintf(stderr, "Error: Only UDP URIs are currently supported - must begin with %s\n", UDP_ADDRESS_PREFIX);
            abort();
        }

        /* get node part */
        host_start = &buffer[sizeof(UDP_ADDRESS_PREFIX) - 1];
        port_start = strchr(host_start, ':');
        if (port_start != NULL)
        {
            *port_start = 0;
            ++port_start;

            value = strtol(port_start, &end, 0);
            if (value > 0 && value < 65536 && *end == 0)
            {
                /* overwrite default port with provided value */
                parsed_addr->sin_port = htons(value);
            }
            else
            {
                fprintf(stderr, "Error: Invalid UDP port number: %s\n", port_start);
                abort();
            }
        }

        /* For now, only dotted-decimal is accepted.  In the future, this could do a DNS lookup. */
        if (!isdigit((unsigned char)*host_start))
        {
            fprintf(stderr, "Error: IP address must be in dotted-decimal form: %s\n", host_start);
            abort();
        }

        /* overwrite default host address with provided value */
        if (!inet_pton(parsed_addr->sin_family, host_start, &parsed_addr->sin_addr))
        {
            fprintf(stderr, "Error: %s: %s\n", strerror(errno), host_start);
            abort();
        }
    }

    strcpy(buffer, UDP_ADDRESS_PREFIX);
    if (inet_ntop(parsed_addr->sin_family, &parsed_addr->sin_addr, &buffer[sizeof(UDP_ADDRESS_PREFIX) - 1],
                  buf_size - sizeof(UDP_ADDRESS_PREFIX)) == NULL)
    {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        abort();
    }

    snprintf(&buffer[strlen(buffer)], buf_size - strlen(buffer), ":%u", (unsigned int)ntohs(parsed_addr->sin_port));
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
    static const char *opt_string = "l:r:i:o:12d:?";

    /*
     * getopts_long long form argument table
     */
    static const struct option long_opts[] = {{"local-addr", required_argument, NULL, 'l'},
                                              {"remote-addr", required_argument, NULL, 'r'},
                                              {"input-file", required_argument, NULL, 'i'},
                                              {"output-file", required_argument, NULL, 'o'},
                                              {"local-cla-uri", required_argument, NULL, 1000},
                                              {"remote-cla-uri", required_argument, NULL, 1001},
                                              {"delay", required_argument, NULL, 'd'},
                                              {"help", no_argument, NULL, '?'},
                                              {NULL, no_argument, NULL, 0}};

    int         opt;
    int         longopt_index;
    const char *env_str;

    /* Initialize from Environment */
    env_str = getenv("BP_LOCAL_ADDRESS");
    if (env_str != NULL)
    {
        strncpy(local_dtnaddr_string, env_str, sizeof(local_dtnaddr_string) - 1);
        local_dtnaddr_string[sizeof(local_dtnaddr_string) - 1] = 0;
    }

    env_str = getenv("BP_REMOTE_ADDRESS");
    if (env_str != NULL)
    {
        strncpy(remote_dtnaddr_string, env_str, sizeof(remote_dtnaddr_string) - 1);
        remote_dtnaddr_string[sizeof(remote_dtnaddr_string) - 1] = 0;
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
                strncpy(local_dtnaddr_string, optarg, sizeof(local_dtnaddr_string) - 1);
                local_dtnaddr_string[sizeof(local_dtnaddr_string) - 1] = 0;
                break;

            case 'r':
                strncpy(remote_dtnaddr_string, optarg, sizeof(remote_dtnaddr_string) - 1);
                remote_dtnaddr_string[sizeof(remote_dtnaddr_string) - 1] = 0;
                break;

            case 'i':
                redirect_file(STDIN_FILENO, optarg);
                break;

            case 'o':
                redirect_file(STDOUT_FILENO, optarg);
                break;

            case 'd':
                inter_bundle_delay = strtoul(optarg, NULL, 0);
                if (inter_bundle_delay >= 1000)
                {
                    display_banner(argv[0]);
                }
                break;

            case 1000:
                strncpy(local_ipaddr_string, optarg, sizeof(local_ipaddr_string) - 1);
                local_ipaddr_string[sizeof(local_ipaddr_string) - 1] = 0;
                break;

            case 1001:
                strncpy(remote_ipaddr_string, optarg, sizeof(remote_ipaddr_string) - 1);
                remote_ipaddr_string[sizeof(remote_ipaddr_string) - 1] = 0;
                break;

            default:
                display_banner(argv[0]);
                break;
        }
    } while (true);
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

    fprintf(stderr, "started %s\n", name);
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
    struct timespec      tm;

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
            status = sendto(cla->sys_fd, bundle_buffer, data_fill_sz, 0, cla->remote_addr, cla->remote_addr_len);
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

            tm.tv_sec  = 0;
            tm.tv_nsec = inter_bundle_delay * 1000000;
            clock_nanosleep(CLOCK_MONOTONIC, 0, &tm, NULL);
        }
    }

    return NULL;
}

static int setup_cla(bplib_routetbl_t *rtbl, const struct sockaddr_in *local_cla_addr,
                     const struct sockaddr_in *remote_cla_addr)
{
    static bplib_cla_intf_id_t cla_intf_id; /* static because its passed to pthread_create() */

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

    if (bind(cla_intf_id.sys_fd, (const struct sockaddr *)local_cla_addr, sizeof(*local_cla_addr)) < 0)
    {
        perror("bind()");
        return -1;
    }

    cla_intf_id.remote_addr     = (const struct sockaddr *)remote_cla_addr;
    cla_intf_id.remote_addr_len = sizeof(*remote_cla_addr);

    /* Create CLA Threads, one for each direction */
    /* This is currently necessary because they pend on different things */
    start_thread(cla_in, &cla_intf_id);
    start_thread(cla_out, &cla_intf_id);

    return 0;
}

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

    storage_intf_id = intf_id;

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
    uint32_t            bundle_count;
    ssize_t             status;
    struct pollfd       pfd;
    int                 app_fd;
    bool                got_eof;
    bool                send_ready;

    got_eof       = false;
    send_ready    = false;
    desc          = arg;
    data_fill_sz  = 0;
    stream_pos    = 0;
    bundle_count  = 0;
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
            if (!send_ready)
            {
                msg_buffer.segment_len = htonl(data_fill_sz);
                msg_buffer.stream_pos  = htonl(stream_pos);
                fprintf(stderr, "Ready to send: bundle=%lu, pos=%lu size=%lu\n", (unsigned long)bundle_count,
                        (unsigned long)stream_pos, (unsigned long)data_fill_sz);

                stream_pos += data_fill_sz;
                ++bundle_count;

                send_ready = true;
            }

            status = bplib_send(desc, &msg_buffer, offsetof(bpcat_msg_content_t, content[data_fill_sz]),
                                BPCAT_MAX_WAIT_MSEC);
            if (status == BP_SUCCESS)
            {
                /* reset the buffer */
                data_fill_sz = 0;
                send_ready   = false;
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
    curr_app_buffer     = NULL;
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
            fprintf(stderr, "Call system write()... msgcount=%lu streampos=%lu segment_offset=%lu segment_remain=%lu\n",
                    (unsigned long)total_message_count - BPCAT_RECV_WINDOW_SZ, (unsigned long)curr_stream_pos,
                    (unsigned long)segment_offset, (unsigned long)segment_remain);
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

                fprintf(stderr, "bplib_recv() success: got segment pos=%u len=%u distance=%ld\n",
                        (unsigned int)curr_bp_buffer->msg.stream_pos, (unsigned int)curr_bp_buffer->msg.segment_len,
                        (long)segment_distance);

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
    bplib_routetbl_t  *rtbl;
    bp_ipn_addr_t      local_ipn_addr;
    bp_ipn_addr_t      remote_ipn_addr;
    bp_ipn_addr_t      storage_ipn_addr;
    struct sockaddr_in local_cla_addr;
    struct sockaddr_in remote_cla_addr;
    uint64_t           stats_time;
    uint64_t           curr_time;

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
    parse_ipn_address(local_dtnaddr_string, sizeof(local_dtnaddr_string), &local_ipn_addr, BPCAT_DEFAULT_LOCAL_NODENUM,
                      BPCAT_DEFAULT_SERVICENUM);
    fprintf(stderr, "Local DTN address: %s\n", local_dtnaddr_string);
    parse_ipn_address(remote_dtnaddr_string, sizeof(remote_dtnaddr_string), &remote_ipn_addr,
                      BPCAT_DEFAULT_LOCAL_NODENUM, BPCAT_DEFAULT_SERVICENUM);
    fprintf(stderr, "Remote DTN address: %s\n", remote_dtnaddr_string);
    parse_ip_address(local_ipaddr_string, sizeof(local_ipaddr_string), &local_cla_addr, INADDR_ANY,
                     local_ipn_addr.node_number + BPCAT_DEFAULT_UDP_PORT_BASE);
    fprintf(stderr, "Local CLA URI: %s\n", local_ipaddr_string);
    parse_ip_address(remote_ipaddr_string, sizeof(remote_ipaddr_string), &remote_cla_addr, INADDR_LOOPBACK,
                     remote_ipn_addr.node_number + BPCAT_DEFAULT_UDP_PORT_BASE);
    fprintf(stderr, "Remote CLA URI: %s\n", remote_ipaddr_string);

    /* Test route table with 1MB of cache */
    rtbl = bplib_route_alloc_table(10, 1 << 20);
    if (rtbl == NULL)
    {
        fprintf(stderr, "%s(): bplib_route_alloc_table failed\n", __func__);
        return EXIT_FAILURE;
    }

    /* this currently assumes service number 10 for storage, should be configurable */
    storage_ipn_addr = (bp_ipn_addr_t) {local_ipn_addr.node_number, 10};
    if (setup_storage(rtbl, &storage_ipn_addr) < 0)
    {
        fprintf(stderr, "Failed setup_storage()... exiting\n");
        return EXIT_FAILURE;
    }

    if (setup_cla(rtbl, &local_cla_addr, &remote_cla_addr) < 0)
    {
        fprintf(stderr, "Failed setup_cla()... exiting\n");
        return EXIT_FAILURE;
    }

    if (setup_connection(rtbl, &local_ipn_addr, &remote_ipn_addr) < 0)
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
            bplib_cache_debug_scan(rtbl, storage_intf_id);
            stats_time += 10000;
        }
    }

    /* Join Threads */
    join_thread(app_in);
    join_thread(app_out);
    join_thread(cla_in);
    join_thread(cla_out);

    return 0;
}
