/************************************************************************
 * File: bibe.c
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

static int trace = 1;
static int brief = 0;        // KRS
static int dump_enable = 1;  // KRS

#define MAX_WAIT_MSEC 250

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
//#include <unistd.h>  //
#include <errno.h>

#include "bplib.h"
//#include "bp/bplib_store_ram.h"
//#include "../bplibk/inc/bplib.h"
//#include "../bplibk/inc/bplib_store_ram.h"
//#include "bpio.h"

///#include "bplib_api_types.h"
#include <sys/socket.h>
#include <poll.h>
#include <netinet/in.h>
#include "bplib_routing.h"
///#include <string.h>

#include "cbor.h"
//#include "../mpool/inc/v7_mpool.h"
#include "bplib_os.h"
//#include "v7_mpool_ref.h"
#include "../mpool/inc/v7_mpool_ref.h"

#include "pri.h"
#include "bib.h"
#include "pay.h"
#include "vers7.h"
//#include "../lib/bundle_types.h"

#include "bibe.h"

//#include "cbuf.h"     // for active table
#include "rh_hash.h"    // for active table

extern volatile sig_atomic_t app_running;

#if 0
typedef struct bplib_cla_intf_id
{
    bplib_routetbl_t *rtbl;
    bp_handle_t       intf_id;
    int               sys_fd;
} bplib_cla_intf_id_t;

typedef struct bplib_cla_stats
{
    uintmax_t ingress_byte_count;
    uintmax_t egress_byte_count;
} bplib_cla_stats_t; ///
#endif

/* Table Functions */
typedef int (*bp_table_create_t)(void **table, int size);
typedef int (*bp_table_destroy_t)(void *table);
typedef int (*bp_table_add_t)(void *table, bp_active_bundle_t bundle, bool overwrite);
typedef int (*bp_table_next_t)(void *table, bp_active_bundle_t *bundle);
typedef int (*bp_table_remove_t)(void *table, bp_val_t cid, bp_active_bundle_t *bundle);
typedef int (*bp_table_available_t)(void *table, bp_val_t cid);
typedef int (*bp_table_count_t)(void *table);

/* Active Table */
typedef struct
{
    void                *table;
    bp_table_create_t    create;
    bp_table_destroy_t   destroy;
    bp_table_add_t       add;
    bp_table_next_t      next;
    bp_table_remove_t    remove;
    bp_table_available_t available;
    bp_table_count_t     count;
} bp_active_table_t;


static int table_retransmit_order = BP_RETX_OLDEST_BUNDLE;
static bool cid_reuse = true;  /* 0: new CID when retransmitting, 1: reuse CID when retransmitting */
static int local_node = 100;
static int local_service = 1;
static int remote_node = 101;
static int remote_service = 2;

static bp_handle_t       active_table_signal = BP_INVALID_HANDLE;
static bp_active_table_t active_table;
static unsigned long current_active_cid = 0;
static int max_gaps_per_custody_signal = 0;

static rb_tree_t custody_tree;
static bp_handle_t custody_tree_lock   = BP_INVALID_HANDLE;

#if BPLIB_GLOBAL_CUSTODY_ID
static bp_handle_t bplib_custody_id_mutex  = {0};
static unsigned int bplib_global_custody_id = 0;
#endif


#define INNER_BUNDLE_SIZE_MAX 65536
#define OUTER_PAYLOAD_SIZE_MAX (INNER_BUNDLE_SIZE_MAX+100)
#define CUSTODY_SIGNAL_PAYLOAD_SIZE_MAX (MAX_MAX_GAPS_PER_CUSTODY_SIGNAL * 20 + 100)

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

void dump(uint8_t* buffer, size_t normal_length, size_t max_length, bool enable)
{
    if (!enable)
        return;

    char out[10000];
    int i=0;
    strcpy(out, "\n");
    int newlines = 1;
    for (i=0; i<max_length; ++i)
    {
        if (buffer[i] == 0)
            sprintf(out+newlines+i*3, ".. ");
        else if (i != normal_length-1)
            sprintf(out+newlines+i*3, "%02X ", buffer[i]);
        else
            sprintf(out+newlines+i*3, "%02X|", buffer[i]);
        if ((i+1) % 16 == 0)
        {
            strcat(out, "\n");
            ++newlines;
        }
    }
    if (i%16 != 0)
        strcat(out, "\n");
    //bplog(NULL, 0, out);
    fprintf(stderr, out);
}

int v7_display_primary_block(void* pprimary_block)
{
    bp_blk_pri_t* pri_blk = (bp_blk_pri_t*)pprimary_block;

    /* Display Primary Block */
    bplog(NULL, BP_FLAG_DIAGNOSTIC, "@@@@\n");
    bplog(NULL, BP_FLAG_DIAGNOSTIC, "Adminstrative Record:          %s\n", pri_blk->is_admin_rec ? "TRUE" : "FALSE");
    bplog(NULL, BP_FLAG_DIAGNOSTIC, "Fragmented:                    %s\n", pri_blk->is_frag ? "TRUE" : "FALSE");
    bplog(NULL, BP_FLAG_DIAGNOSTIC, "Fragmentation Allowed:         %s\n", pri_blk->allow_frag ? "TRUE" : "FALSE");
    bplog(NULL, BP_FLAG_DIAGNOSTIC, "Application Acknowledgement:   %s\n", pri_blk->ack_app ? "TRUE" : "FALSE");
    bplog(NULL, BP_FLAG_DIAGNOSTIC, "-\n");
    bplog(NULL, BP_FLAG_DIAGNOSTIC, "Bundle of Version %d\n", pri_blk->version);
    bplog(NULL, BP_FLAG_DIAGNOSTIC, "Processing Control Flags:      0x%X\n",    (unsigned int)pri_blk->pcf);
    bplog(NULL, BP_FLAG_DIAGNOSTIC, "CRC type:                      %ld\n",     (long)pri_blk->crctype); // new v7
    bplog(NULL, BP_FLAG_DIAGNOSTIC, "Destination EID:               %ld.%ld\n", (long)pri_blk->dstnode, (long)pri_blk->dstserv);
    bplog(NULL, BP_FLAG_DIAGNOSTIC, "Source EID:                    %ld.%ld\n", (long)pri_blk->srcnode, (long)pri_blk->srcserv);
    bplog(NULL, BP_FLAG_DIAGNOSTIC, "Report To EID:                 %ld.%ld\n", (long)pri_blk->rptnode, (long)pri_blk->rptserv);
    bplog(NULL, BP_FLAG_DIAGNOSTIC, "Creation Time:                 %ld\n",     (long)pri_blk->createmsec);
    bplog(NULL, BP_FLAG_DIAGNOSTIC, "Creation Sequence:             %ld\n",     (long)pri_blk->createseq);
    bplog(NULL, BP_FLAG_DIAGNOSTIC, "Lifetime:                      %ld\n",     (long)pri_blk->lifetime);
    if(pri_blk->is_frag)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Fragmentation Offset;          %ld\n",     (long)pri_blk->fragoffset);
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Payload Length:                %ld\n",     (long)pri_blk->paylen);
        // above item is supposed to be the total ADU length  KRS
    }
    if (pri_blk->crctype)
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "CRC:                           %ld\n",     (long)pri_blk->crc);

    /* Return Success */
    return BP_SUCCESS;
}

#if 0
dump_active_table(void* parm, bool lock)  // KRS
{
    bp_channel_t* ch = (bp_channel_t*)parm;
    bp_active_bundle_t active_bundle;
    fprintf(stderr, "active table:\n");
    if (lock) bplib_os_lock(ch->active_table_signal);
    {
        //while(ch->active_table.next(ch->active_table.table, &active_bundle) == BP_SUCCESS)
        int num = ch->active_table.count(ch->active_table.table);
        {
            int i=0;
            for (int num_found=0; num_found<num; ++i)
            {
                /* Move to Next Oldest */
                //if (ch->active_table.next(ch->active_table.table, &active_bundle) == BP_SUCCESS)
                //    if (trace) fprintf(stderr, "sid: %d   cid: %d\n", active_bundle.sid, active_bundle.cid);
                //else
                //    if (trace) fprintf(stderr, "NONSUCCESS IN DUMP\n");

                int status = ch->active_table.remove(ch->active_table.table, i, &active_bundle);
                if (status == BP_SUCCESS)
                {
                    fprintf(stderr, "sid: %u   i: %d cid: %d\n", active_bundle.sid, i, active_bundle.cid);
                    ++num_found;
                    ch->active_table.add(ch->active_table.table, active_bundle, false);  // put it back
                }
            }
            //fprintf(stderr, "i == %d\n", i);
        }
    }
    if (lock) bplib_os_unlock(ch->active_table_signal);
}
#endif

static size_t cborlen(uint8_t* buffer, size_t size)  // length of 1 top-level object
{
    CborParser parser;
    CborValue it_val;
    CborError err=0;
if (trace && dump_enable) fprintf(stderr, "into cborlen:\n");
dump(buffer, 200, 200, dump_enable);
    err = cbor_parser_init(buffer, size, 0, &parser, &it_val);
    if (trace) fprintf(stderr, "cborlen err 1 %d\n", err);
    err = cbor_value_advance(&it_val);
    if (trace) fprintf(stderr, "cborlen err 2 %d\n", err);
    return cbor_value_get_next_byte(&it_val) - buffer;    
}


static int pri_decborize(CborValue *it, int size, bp_blk_pri_t* pri, uint32_t* flags)
{
    uint32_t sdnvflags = 0;
 
    CborValue recursed, recursed2, recursed3;

    /* Check Size */
    if(size < 1) return bplog(flags, BP_FLAG_FAILED_TO_PARSE, "Invalid size of primary block: %d\n", size);

    if (trace) bplog(NULL, 0, "read block\n");
    /* Read Block */
    cbor_value_enter_container(it, &recursed);
    uint64_t version=0;
    cbor_value_get_uint64(&recursed, &version);
    pri->version = (uint8_t)version;
    cbor_value_advance_fixed(&recursed);
    cbor_value_get_uint64(&recursed, &pri->pcf);
    cbor_value_advance_fixed(&recursed);
    cbor_value_get_uint64(&recursed, &pri->crctype);
    cbor_value_advance_fixed(&recursed);
    if (trace) bplog(NULL, 0, "crctype %d\n", pri->crctype);

    if (trace) bplog(NULL, 0, "read block 2\n");
    cbor_value_enter_container(&recursed, &recursed2);
    cbor_value_get_uint64(&recursed2, &pri->dstscheme);
    cbor_value_advance_fixed(&recursed2);
    cbor_value_enter_container(&recursed2, &recursed3);
    // HARDCODED ASSUMPTION OF IPN SCHEME  KRS
    cbor_value_get_uint64(&recursed3, &pri->dstnode);
    cbor_value_advance_fixed(&recursed3);
    cbor_value_get_uint64(&recursed3, &pri->dstserv);
    cbor_value_advance_fixed(&recursed3);
    cbor_value_leave_container(&recursed2, &recursed3);
    cbor_value_leave_container(&recursed, &recursed2);

    if (trace) bplog(NULL, 0, "read block 3\n");
    cbor_value_enter_container(&recursed, &recursed2);
    cbor_value_get_uint64(&recursed2, &pri->srcscheme);
    cbor_value_advance_fixed(&recursed2);
    cbor_value_enter_container(&recursed2, &recursed3);
    // HARDCODED ASSUMPTION OF IPN SCHEME  KRS
    cbor_value_get_uint64(&recursed3, &pri->srcnode);
    cbor_value_advance_fixed(&recursed3);
    cbor_value_get_uint64(&recursed3, &pri->srcserv);
    cbor_value_advance_fixed(&recursed3);
    cbor_value_leave_container(&recursed2, &recursed3);
    cbor_value_leave_container(&recursed, &recursed2);

    if (trace) bplog(NULL, 0, "read block 4\n");
    cbor_value_enter_container(&recursed, &recursed2);
    cbor_value_get_uint64(&recursed2, &pri->rptscheme);
    cbor_value_advance_fixed(&recursed2);
    cbor_value_enter_container(&recursed2, &recursed3);
    // HARDCODED ASSUMPTION OF IPN SCHEME  KRS
    cbor_value_get_uint64(&recursed3, &pri->rptnode);
    cbor_value_advance_fixed(&recursed3);
    cbor_value_get_uint64(&recursed3, &pri->rptserv);
    cbor_value_advance_fixed(&recursed3);
    cbor_value_leave_container(&recursed2, &recursed3);
    cbor_value_leave_container(&recursed, &recursed2);

    if (trace) bplog(NULL, 0, "read block 5\n");
    cbor_value_enter_container(&recursed, &recursed2);
    if (trace) bplog(NULL, 0, "read block 51\n");
    cbor_value_get_uint64(&recursed2, &pri->createmsec);
    cbor_value_advance_fixed(&recursed2);
    if (trace) bplog(NULL, 0, "read block 52\n");
    cbor_value_get_uint64(&recursed2, &pri->createseq);
    cbor_value_advance_fixed(&recursed2);
    cbor_value_leave_container(&recursed, &recursed2);

    if (trace) bplog(NULL, 0, "read block 53\n");
    cbor_value_get_uint64(&recursed, &pri->lifetime);
    cbor_value_advance_fixed(&recursed);

    /* Handle Fragment Fields */
    if(pri->pcf & BP_PCF_FRAGMENT_MASK) /* fields are present in bundle */
    {
        if (trace) bplog(NULL, 0, "read block 54\n");
        cbor_value_get_uint64(&recursed, &pri->fragoffset);
        cbor_value_advance_fixed(&recursed);
        if (trace) bplog(NULL, 0, "read block 55\n");
        cbor_value_get_uint64(&recursed, &pri->paylen);
        cbor_value_advance_fixed(&recursed);
    }

#if 1
    /* CRC on this block if specified  KRS */
    if (pri->crctype != bp_crctype_none)
    {
        cbor_value_advance(&recursed);  // skip HARDCODED KRS
    }
#endif

    if (trace) bplog(NULL, 0, "type %d\n", cbor_value_get_type(&recursed));
    cbor_value_leave_container(it, &recursed);
    if (trace) bplog(NULL, 0, "type %d\n", cbor_value_get_type(&recursed));

    /* Set Administrative Record Status */
    if(pri->pcf & BP_PCF_ADMIN_MASK)      pri->is_admin_rec = true;
    else                                        pri->is_admin_rec = false;

    /* Set Allow Fragmentation */
    if(pri->pcf & BP_PCF_NOFRAG_MASK)     pri->allow_frag = false;
    else                                        pri->allow_frag = true;

    /* Set Is Fragment */
    if(pri->pcf & BP_PCF_FRAGMENT_MASK)   pri->is_frag = true;
    else                                        pri->is_frag = false;

    /* Set Custody Request */
    //if(pri->pcf & BP_PCF_CSTRQST_MASK)    pri->cst_rqst = true;
    //else                                        pri->cst_rqst = false;

    /* Set Acknowledgement from Application Request */
    if(pri->pcf & BP_PCF_ACKRQST_MASK)    pri->ack_app = true;
    else                                        pri->ack_app = false;

    /* Success Oriented Error Checking */
    if(sdnvflags != 0)
    {
        *flags |= sdnvflags;
        return bplog(flags, BP_FLAG_FAILED_TO_PARSE, "Flags raised during processing of primary block (%08X)\n", sdnvflags);
    }
    else if(pri->version != BP_PRI_VERSION)
    {
        return bplog(flags, BP_FLAG_FAILED_TO_PARSE, "Incorrect version of bundle reported: %d\n", pri->version);
    }
    else
    {
        return BP_SUCCESS;
    }
}

int bib_decborize (CborValue *recursed, int size, bp_blk_bib_t* bib, uint32_t* flags)
{
    if (trace) bplog(NULL, 0, "bib_decborize()\n");

    CborValue recursed2, recursed3;
    uint32_t processing_flags = 0;

    /* Check Size */
    if(size < 1) return bplog(flags, BP_FLAG_FAILED_TO_PARSE, "Invalid size of BIB block: %d\n", size);

    /* Read Block */
    uint64_t blknum = 0;
    cbor_value_get_uint64(recursed, &blknum);  // new in v7
    cbor_value_advance(recursed);

    bib->bf = -1000000;
    bib->security_target_count = -1000000;

    cbor_value_get_uint64(recursed, &bib->bf);
    cbor_value_advance(recursed);
    cbor_value_get_uint64(recursed, &bib->crctype);  // new in v7
    cbor_value_advance(recursed);

    
    // ASB 

    #define MAX_BIB_ASB_LEN 1024
    //static uint8_t asb_buffer[MAX_BIB_ASB_LEN];
    uint8_t asb_buffer[MAX_BIB_ASB_LEN];
    size_t asb_len = sizeof(asb_buffer);
    cbor_value_copy_byte_string(recursed, asb_buffer, &asb_len, NULL);
    cbor_value_advance(recursed);
    if (trace) if (dump_enable) bplog(NULL, 0, "ASB:\n");
    if (trace) dump(asb_buffer, 50, 50, dump_enable);
#if 0    
    CborValue asb;
    CborParser asb_parser;
    cbor_parser_init(asb_buffer, sizeof(asb_buffer), 0, &asb_parser, &asb);

    size_t length = 0;
    cbor_value_get_array_length(&asb, &length);  // targets array

    //if (bytes_read + 1 > size) 
    //  return bplog(flags, BP_FLAG_FAILED_TO_PARSE, "BIB block terminated prematurely: %d\n", bytes_read);

    bib->context_id = -1000000;
    bib->context_flags = -1000000;
    //bytes_read = -1000000;
    cbor_value_enter_container(&asb, &recursed2);
    if (trace) bplog(NULL, 0, "bib 1\n");
    if (trace) bplog(NULL, 0, "length %d\n", length);
    for (unsigned int i=0; i<length; ++i)
    {
        if (i == 0)  // HARDCODED KRS
            cbor_value_get_uint64(&recursed2, &bib->target);
        if (trace && !brief) bplog(NULL, 0, "target block %d\n", bib->target);
        cbor_value_advance(&recursed2);
        ///cbor_value_advance_fixed(&recursed2);
        //if (trace) bplog(NULL, 0, "at end a %d\n", cbor_value_at_end(&recursed2)); // true
        //if (trace) bplog(NULL, 0, "at end aa %d\n", cbor_value_at_end(&asb));  // false
        //if (trace) bplog(NULL, 0, "next byte a %p\n", cbor_value_get_next_byte(&recursed2));  // 52e2
        //if (trace) bplog(NULL, 0, "next byte aa %p\n", cbor_value_get_next_byte(&asb));       // 52e0
    }
    //if (trace) bplog(NULL, 0, "at end b %d\n", cbor_value_at_end(&recursed2));  // true
    //if (trace) bplog(NULL, 0, "at end bb %d\n", cbor_value_at_end(&asb));  // false
    //if (trace) bplog(NULL, 0, "next byte b %p\n", cbor_value_get_next_byte(&recursed2));   // 52e2
    //if (trace) bplog(NULL, 0, "next byte bb %p\n", cbor_value_get_next_byte(&asb));        // 52e0
    uint8_t* next = (uint8_t*)(uint8_t*)cbor_value_get_next_byte(&recursed2);
    cbor_value_leave_container(&asb, &recursed2);
    //if (trace) bplog(NULL, 0, "at end c %d\n", cbor_value_at_end(&recursed2));  // true
    //if (trace) bplog(NULL, 0, "at end cc %d\n", cbor_value_at_end(&asb));  //true
    //if (trace) bplog(NULL, 0, "next byte c %p\n", cbor_value_get_next_byte(&recursed2));   // 52e2
    //if (trace) bplog(NULL, 0, "next byte cc %p\n", cbor_value_get_next_byte(&asb));        // 52e2

    //cbor_parser_init(asb_buffer, sizeof(asb_buffer), 0, &asb_parser, &asb);
    //cbor_value_advance(&asb); // skip initial array
    next = (uint8_t*)cbor_value_get_next_byte(&asb);
    cbor_parser_init(next, sizeof(asb_buffer), 0, &asb_parser, &asb);
    //if (trace) bplog(NULL, 0, "type %d\n", cbor_value_get_type(&asb));
    next = (uint8_t*)cbor_value_get_next_byte(&asb);
    cbor_parser_init(next, sizeof(asb_buffer), 0, &asb_parser, &asb);
    //if (trace) bplog(NULL, 0, "type %d\n", cbor_value_get_type(&asb));
    cbor_value_get_uint64(&asb, &bib->context_id);  // used to be cipher_suite_id
    if (trace && !brief) bplog(NULL, 0, "context_id %d\n", bib->context_id);
    cbor_value_advance(&asb);
    next = (uint8_t*)cbor_value_get_next_byte(&asb);
    cbor_parser_init(next, sizeof(asb_buffer), 0, &asb_parser, &asb);
    if (trace && !brief) bplog(NULL, 0, "type %d\n", cbor_value_get_type(&asb));
    cbor_value_get_uint64(&asb, &bib->context_flags);  // used to be cipher_suite_flags
    if (trace  & !brief) bplog(NULL, 0, "context_flags %d\n", bib->context_flags);
    cbor_value_advance(&asb);
    next = (uint8_t*)cbor_value_get_next_byte(&asb);
    cbor_parser_init(next, sizeof(asb_buffer), 0, &asb_parser, &asb);

    unsigned int j=0;
    if (0 &&                length > 0)
    {
    // new in v7
    if (trace) bplog(NULL, 0, "bib --2\n");
    cbor_value_enter_container(&asb, &recursed2);
    if (trace) bplog(NULL, 0, "bib --3\n");
    cbor_value_get_uint64(&recursed2, &bib->srcscheme);
    cbor_value_advance(&recursed2);
    if (trace) bplog(NULL, 0, "bib --4\n");
    cbor_value_enter_container(&recursed2, &recursed3);
    if (trace) bplog(NULL, 0, "bib --5\n");
    cbor_value_get_uint64(&recursed3, &bib->srcnode);
    cbor_value_advance(&recursed3);
    cbor_value_get_uint64(&recursed3, &bib->srcserv);
    //if (trace) bplog(NULL, 0, "bib --6\n");
    cbor_value_advance(&recursed3);
    cbor_value_leave_container(&recursed2, &recursed3);
    cbor_value_leave_container(&asb, &recursed2);
    //if (trace) bplog(NULL, 0, "bib --7\n");
    next = (uint8_t*)cbor_value_get_next_byte(&asb);
    cbor_parser_init(next, sizeof(asb_buffer), 0, &asb_parser, &asb);

    /* Optional security context parameters */

    //if (bytes_read + 1 > size) 
    // return bplog(flags, BP_FLAG_FAILED_TO_PARSE, "BIB block terminated prematurely: %d\n", bytes_read);

    // security results
    uint64_t num_targets = 0;
    cbor_value_get_array_length(&asb, &num_targets);
    if (trace) bplog(NULL, 0, "bib --16\n");
    cbor_value_enter_container(&asb, &recursed2);
    if (trace) bplog(NULL, 0, "bib --17\n");
    for (unsigned int i=0; i<num_targets; ++i)  // TODO WATCH OUT FOR OVERRUN  KRS
    {   
        uint64_t num_results_this_target = 0;
        cbor_value_get_array_length(&recursed2, &num_results_this_target);
    if (trace) bplog(NULL, 0, "bib --18\n");
        cbor_value_enter_container(&recursed2, &recursed3);
    if (trace) bplog(NULL, 0, "bib --19\n");
        for (; j<num_results_this_target; j+=2)  // WATCH OUT FOR OVERRUN  KRS
        {
            cbor_value_get_uint64(&recursed3, &((uint64_t*)&bib->security_result_data)[j]);  // id
            cbor_value_advance(&recursed3);
            
            //BP_BIB_CRC16_X25 or BP_BIB_CRC32_CASTAGNOLI are only id's presently known
            //return bplog(flags, BP_FLAG_INVALID_CIPHER_SUITEID, "Invalid BIB cipher suite id: %d\n", bib->cipher_suite_id);
            cbor_value_get_uint64(&recursed3, &((uint64_t*)&bib->security_result_data)[j+1]);  // HARDCODED ASSUMPTION OF UINT RESULT  KRS
            cbor_value_advance(&recursed3);
        }
        if (trace) bplog(NULL, 0, "leave 1\n");
        cbor_value_leave_container(&recursed2, &recursed3);
    }
    //if (trace) bplog(NULL, 0, "leave 2\n");
    cbor_value_leave_container(&asb, &recursed2);
    }
    //if (trace) bplog(NULL, 0, "leave 3\n");
    next = (uint8_t*)cbor_value_get_next_byte(&asb);
    cbor_parser_init(next, sizeof(asb_buffer), 0, &asb_parser, &asb);
    bib->security_result_length = -1000000;;
    //bytes_read = -1000000;
    bib->security_result_length = j;
#endif


#if 1
    /* CRC on this block if specified  KRS */
    if (bib->crctype != bp_crctype_none)
    {
        cbor_value_advance(recursed);  // skip HARDCODED KRS
    }
#endif

    //cbor_value_leave_container(buffer, recursed);

    /* Success Oriented Error Checking */
    if(processing_flags != 0)
    {
        *flags |= processing_flags;
        return bplog(flags, BP_FLAG_FAILED_TO_PARSE, "Flags raised during processing of BIB (%08X)\n", processing_flags); 
    }
    else
    {
        return BP_SUCCESS;
    }
}

static int pay_decborize (CborValue* recursed, int size, bp_blk_pay_t* pay, uint32_t* flags, bool admin_record)
{
    uint32_t processing_flags = 0;

    /* Check Size */
    ///if(size < 1) 
    ///  return bplog(flags, BP_FLAG_FAILED_TO_PARSE, "Invalid size of payload block: %d\n", size);

    if (trace) bplog(NULL, 0, "pay_decborize:\n");

    /* Read Block */
    uint64_t blknum = 0;
    cbor_value_get_uint64(recursed, &blknum);  // new in v7
    cbor_value_advance(recursed);
    if (trace) bplog(NULL, 0, "block # %d\n", blknum);
    /* Read Block Information */
    ///pay->bf.width = 0;
    ///pay->bf.index = -1000000;

    cbor_value_get_uint64(recursed, &pay->bf);  // block flags
    cbor_value_advance(recursed);
    if (trace) bplog(NULL, 0, "block flags %x\n", pay->bf);
    cbor_value_get_uint64(recursed, &pay->crctype);  // new in v7
    cbor_value_advance(recursed);
    if (trace) bplog(NULL, 0, "crctype %d\n", pay->crctype);

    size_t length = 0;
    cbor_value_get_string_length(recursed, &length);
    if (trace) bplog(NULL, 0, "length %d\n", length);
    static uint8_t payld[INNER_BUNDLE_SIZE_MAX];
    cbor_value_copy_byte_string(recursed, payld, &length, NULL);
    cbor_value_advance(recursed);
    memset((uint8_t*)payld+length, 0, sizeof(payld)-length);
    pay->payptr = payld;  // still not decoded though
    pay->paysize = length;  // length of undecoded

    // all payloads we we ae supposed to get for parsing are in Administraive Records  KRS
    
    // parse the payload of the encapsulating bundle
    static uint8_t local_buf[OUTER_PAYLOAD_SIZE_MAX];  // MAGIC NUMBER  KRS

    CborParser parser2;
    CborValue it2, recursed20;
    uint64_t value;
    //if (trace) dump(buffer, size);
    cbor_parser_init(payld, length +100, 0, &parser2, &it2);
    if (dump_enable) fprintf(stderr, "payld:\n");
    dump(payld, 200, 200, dump_enable);
if (trace) bplog(NULL, 0, "pay_decborize 100:\n");
    cbor_value_enter_container(&it2, &recursed20);
    cbor_value_get_int(&recursed20, &pay->admin_record_type);
    if (trace) fprintf(stderr, "pay_decborize: pay->admin_record_type == %d\n", pay->admin_record_type);
    cbor_value_advance(&recursed20);

    // the cbor format of the rest of the admin record is dependent on the admin record type
    if (pay->admin_record_type == BP_ADMIN_REC_TYPE_BIBE)
    {
        CborValue recursed30;
        cbor_value_enter_container(&recursed20, &recursed30);
        cbor_value_get_uint64(&recursed30, &pay->bpdu_transmission_id);
        if (trace) fprintf(stderr, "decoded transmission id %d\n", pay->bpdu_transmission_id);
        //cbor_value_advance(&recursed30);
        cbor_value_advance_fixed(&recursed30);
        cbor_value_get_uint64(&recursed30, &pay->bpdu_retransmission_time);
        cbor_value_advance_fixed(&recursed30);
        length = sizeof(local_buf);
        //CborValue next;
        //CborError err = cbor_value_copy_byte_string(&recursed30, payld, &length, &next);  // overlapping addresses  KRS
        CborError err = cbor_value_copy_byte_string(&recursed30, local_buf, &length, NULL);
        cbor_value_advance(&recursed30);
        pay->paysize = length;
        pay->payptr = local_buf;  ////
        if (trace) fprintf(stderr, "fetched payload of length %d:\n", length);
        if (trace) dump(pay->payptr, length, 200, dump_enable);
        cbor_value_leave_container(&recursed20, &recursed30);
    }
    else if (pay->admin_record_type == BP_ADMIN_REC_TYPE_CUSTODY_SIGNAL)
    {
        CborValue recursed30, recursed40, recursed50;
        cbor_value_enter_container(&recursed20, &recursed30);
        cbor_value_get_int(&recursed30, &pay->custody_status);
        cbor_value_advance_fixed(&recursed30);
        size_t length = 0;
        cbor_value_get_array_length(&recursed30, &length);
        cbor_value_enter_container(&recursed30, &recursed40);
        if (trace) fprintf(stderr, "pay_decborize: %d ranges\n", length);
        for (int i=0; i<length; ++i)
        {
            cbor_value_enter_container(&recursed40, &recursed50);
            cbor_value_get_uint64(&recursed50, &pay->first_transmission_id[i]);
            cbor_value_advance_fixed(&recursed50);
            cbor_value_get_int(&recursed50, &pay->num_transmission_ids[i]);
            cbor_value_advance_fixed(&recursed50);
            cbor_value_leave_container(&recursed40, &recursed50);
            if (trace) fprintf(stderr, "  pay_decborize: first_transmission id %d  num_transmission_ids %d\n",
                                pay->first_transmission_id[i], pay->num_transmission_ids[i]);
        }
        for (int i=length; i<max_gaps_per_custody_signal+1; ++i)
        {
            pay->first_transmission_id[i] = 0;
            pay->num_transmission_ids[i] = 0;
        }
        cbor_value_leave_container(&recursed30, &recursed40);

        pay->paysize = 0;
        if (trace) fprintf(stderr, "fetched custody signal payload first_transmission_ids:\n");
        if (trace) dump((uint8_t*)&pay->first_transmission_id, 100, 100, dump_enable);
        if (trace) fprintf(stderr, "fetched custody signal payload nums_transmission_ids:\n");
        if (trace) dump((uint8_t*)&pay->num_transmission_ids, 100, 100, dump_enable);

        cbor_value_leave_container(&recursed20, &recursed30);
        //if (trace) bplog(NULL, 0, "type210 %02x\n", cbor_value_get_type(&recursed20));
    }
    else  // malformed bundle  KRS
    {
        if (trace) fprintf(stderr, "MALFORMED BUNDLE admin_record with pay->admin_record_type == %d\n", pay->admin_record_type);
            // eg, v6-style cust xfer  have to skip out on unknown to allow CRC, below  KRS
    }
    cbor_value_leave_container(&it2, &recursed20);


#if 1
    /* CRC on this block if specified  KRS */
    if (pay->crctype != bp_crctype_none)
    {
        cbor_value_advance(&it2);  // skip HARDCODED KRS
    }
#endif

    //cbor_value_leave_container(buffer, recursed);

    /* Success Oriented Error Checking */
    if(processing_flags != 0)
    {
        *flags |= processing_flags;
        return bplog(flags, BP_FLAG_FAILED_TO_PARSE, 
                     "Flags raised during processing of payload block (%08X)\n", processing_flags);
        //return BP_ERROR;
    }
    else
    {
        return BP_SUCCESS;
    }
}

static int bundle_decode(const uint8_t *buffer, size_t size, uint8_t** out, size_t *out_size, bp_blk_pay_t *ppay)  // KRS
{
    // at first do nothing
    //*out = (uint8_t *)in;
    //*out_size = size;
    //return 0;

    static bp_blk_pri_t pri;
    static bp_blk_bib_t bib;

    CborParser parser;
    CborValue it, recursed, recursed2;
    uint64_t blk_type;
    uint32_t flags = 0;

    //if (trace) bplog(NULL, 0, "bundle_decode  size %uld\n", size);
    cbor_parser_init(buffer, size, 0, &parser, &it);
    cbor_value_enter_container(&it, &recursed);

    memset(&pri, 0, sizeof(pri));
    int status = pri_decborize(&recursed, size, &pri, &flags);
    if (trace) v7_display_primary_block(&pri);
    if (status != BP_SUCCESS)
        return status;

    if (!pri.is_admin_rec)
    {
        static uint8_t bundle[INNER_BUNDLE_SIZE_MAX];
        memset(bundle, 0, sizeof(bundle));
        memcpy(bundle, buffer, size);
        *out_size = size;
        return BP_SUCCESS;
    }

    // open consecutive canonical blocks
    //if (trace && !brief) bplog(NULL, 0, "open consecutive canonical blocks\n");
    while (cbor_value_is_container(&recursed))
    {
        cbor_value_enter_container(&recursed, &recursed2);
        cbor_value_get_uint64(&recursed2, &blk_type);
        //cbor_value_advance(&recursed2);
        cbor_value_advance_fixed(&recursed2);
        if (trace && !brief) bplog(NULL, 0, "block type %d\n", blk_type);
if (trace) fprintf(stderr, "block type %d\n", blk_type);
        if (blk_type == BP_BIB_BLK_TYPE)
        {
            bib_decborize(&recursed2, size, &bib, &flags);  // assumes its outer array already opened
        }
        else if (blk_type == BP_PAY_BLK_TYPE)
        {
            pay_decborize(&recursed2, size, ppay, &flags, (&pri)->is_admin_rec);  // assumes its outer array already opened
        }
        else
        {
#if 1    
            /* Mark Processing as Incomplete (unrecognized extension block) */
            bplog(&flags, BP_FLAG_INCOMPLETE, "Unrecognized extension block of type %d skipped\n", blk_type);

            /* Should transmit status report that block cannot be processed */
            if(flags & BP_BLK_NOTIFYNOPROC_MASK)
            {
                bplog(&flags, BP_FLAG_NONCOMPLIANT, "Request to notify on unprocessed extension block ignored\n");
            }

            /* Delete bundle since block not recognized */
            if(flags & BP_BLK_DELETENOPROC_MASK)
            {
                status = bplog(&flags, BP_FLAG_DROPPED, "Dropping bundle with unrecognized block\n");
            }

            /* Check if Block Should be Included */
            if(flags & BP_BLK_DROPNOPROC_MASK)
            {
                /* Exclude Block */
//                exclude[ei++] = start_index;
//                exclude[ei++] = index;
            }
            else
            {
                /* Mark As Forwarded without Processed */
                flags |= BP_BLK_FORWARDNOPROC_MASK;
//                sdnv_write(&buffer[start_index], size - start_index, blk_flags, flags);
            }
#endif
            return BP_ERROR;
        }
        //if (trace) bplog(NULL, 0, "type10 %02x\n", cbor_value_get_type(&recursed2));
        cbor_value_leave_container(&recursed, &recursed2);
    }

    //if (trace) bplog(NULL, 0, "type20 %02x\n", cbor_value_get_type(&recursed));
    cbor_value_leave_container(&it, &recursed);

    *out = ppay->payptr;
    *out_size = ppay->paysize;
    //if (trace) bplog(NULL, 0, "leaving bundle_decode()\n");
    return BP_SUCCESS;
}

int pri_cborize (CborEncoder* encoder, bp_blk_pri_t* pri)
{
    /* Set Administrative Record Status */
    if (trace) fprintf(stderr, "pri_cborize: pri->is_admin_rec %d\n", pri->is_admin_rec);
    if (pri->is_admin_rec)
        pri->pcf |= BP_PCF_ADMIN_MASK;
    else                                        
        pri->pcf &= ~BP_PCF_ADMIN_MASK;

    /* Set Allow Fragmentation */
    if (!pri->allow_frag)
        pri->pcf |= BP_PCF_NOFRAG_MASK;
    else                                        
        pri->pcf &= ~BP_PCF_NOFRAG_MASK;

    /* Set Is Fragment */
    if (pri->is_frag)
        pri->pcf |= BP_PCF_FRAGMENT_MASK;
    else                                        
        pri->pcf &= ~BP_PCF_FRAGMENT_MASK;

#if 0  // v6
    /* Set Custody Request */
    if(pri->cst_rqst)
        pri->pcf |= BP_PCF_CSTRQST_MASK;
    else
        pri->pcf &= ~BP_PCF_CSTRQST_MASK;
#endif

    /* Set Acknowledgement from Application Request */
    if (pri->ack_app)
        pri->pcf |= BP_PCF_ACKRQST_MASK;
    else                                        
        pri->pcf &= ~BP_PCF_ACKRQST_MASK;


    CborEncoder arrayEncoder;

    int num_items = 8;
    if (pri->crctype != bp_crctype_none)
        num_items += 1;
    if (pri->is_frag)
        num_items += 2;
    cbor_encoder_create_array(encoder, &arrayEncoder, num_items);

    cbor_encode_uint(&arrayEncoder, 7);  // Bundle Protocol version
    if (trace) fprintf(stderr, "pri_cborize: encoding these bundle processing flags %x\n", pri->pcf);
    cbor_encode_uint(&arrayEncoder, pri->pcf);  // bundle processing flags
    cbor_encode_uint(&arrayEncoder, pri->crctype);  // CRC type

    CborEncoder eidArrayEncoder, eidArrayEncoder2;
    cbor_encoder_create_array(&arrayEncoder, &eidArrayEncoder, 2);
    cbor_encode_uint(&eidArrayEncoder, pri->dstscheme);  // dest scheme
    cbor_encoder_create_array(&eidArrayEncoder, &eidArrayEncoder2, 2);
    cbor_encode_uint(&eidArrayEncoder2, pri->dstnode);  // dest SSP   HARDCODED FOR IPN  KRS
    cbor_encode_uint(&eidArrayEncoder2, pri->dstserv);  // dest SSP
    if (trace) bplog(NULL, 0, "destination is node %d.%d\n", pri->dstnode, pri->dstserv);
    cbor_encoder_close_container(&eidArrayEncoder, &eidArrayEncoder2);
    cbor_encoder_close_container(&arrayEncoder, &eidArrayEncoder);

    cbor_encoder_create_array(&arrayEncoder, &eidArrayEncoder, 2);
    cbor_encode_uint(&eidArrayEncoder, pri->srcscheme);
    cbor_encoder_create_array(&eidArrayEncoder, &eidArrayEncoder2, 2);
    cbor_encode_uint(&eidArrayEncoder2, pri->srcnode);
    cbor_encode_uint(&eidArrayEncoder2, pri->srcserv);
    cbor_encoder_close_container(&eidArrayEncoder, &eidArrayEncoder2);
    cbor_encoder_close_container(&arrayEncoder, &eidArrayEncoder);

    cbor_encoder_create_array(&arrayEncoder, &eidArrayEncoder, 2);
    cbor_encode_uint(&eidArrayEncoder, pri->rptscheme);
    cbor_encoder_create_array(&eidArrayEncoder, &eidArrayEncoder2, 2);
    cbor_encode_uint(&eidArrayEncoder2, pri->rptnode);
    cbor_encode_uint(&eidArrayEncoder2, pri->rptserv);
    cbor_encoder_close_container(&eidArrayEncoder, &eidArrayEncoder2);
    cbor_encoder_close_container(&arrayEncoder, &eidArrayEncoder);

    CborEncoder timestampArrayEncoder;
    cbor_encoder_create_array(&arrayEncoder, &timestampArrayEncoder, 2);
    cbor_encode_uint(&timestampArrayEncoder, pri->createmsec);  // Creation Timestamp time
    cbor_encode_uint(&timestampArrayEncoder, pri->createseq);  // Creation Timestamp sequence number
    cbor_encoder_close_container(&arrayEncoder, &timestampArrayEncoder);

    cbor_encode_uint(&arrayEncoder, pri->lifetime);  // Lifetime

    if (pri->is_frag) 
    {
        cbor_encode_uint(&arrayEncoder, pri->fragoffset);  // fragment offset
        cbor_encode_uint(&arrayEncoder, pri->paylen);  // Total Application Data Unit length
    }

    /* any CRC done here */

    cbor_encoder_close_container(encoder, &arrayEncoder);

    return BP_SUCCESS;
}

int bib_cborize(CborEncoder* encoder, bp_blk_bib_t* bib)
{
    //uint32_t flags = 0;
    //uint32_t flags_summary = 0;
    CborEncoder arrayEncoder;
    
    int num_items = 5;  // no CRC
    if (bib->crctype != bp_crctype_none)
        num_items = 6;
    cbor_encoder_create_array(encoder, &arrayEncoder, num_items);

    cbor_encode_uint(&arrayEncoder, BP_BIB_BLK_TYPE);   // block type
    cbor_encode_uint(&arrayEncoder, bib->blknum);     // block number, new to v7
    cbor_encode_uint(&arrayEncoder, bib->bf);         // block processing flags
    cbor_encode_uint(&arrayEncoder, bib->crctype);    // CRC type  new field in v7


    // First make the ASB, in CBOR format.  This will then be placed inside a CBOR byte string in the block.

    #define ASB_BUFFER_SIZE 2048
    CborParser asb_parser;
    CborValue asb_it;
    static    uint8_t buffer_asb[ASB_BUFFER_SIZE];
    CborEncoder asbEncoder;
    cbor_encoder_init(&asbEncoder, buffer_asb, sizeof(buffer_asb), 0);
    cbor_parser_init(buffer_asb, sizeof(buffer_asb), 0, &asb_parser, &asb_it);
    CborEncoder targetArrayEncoder;
    if (trace && !brief) bplog(NULL, 0, "bib->security_target_count %d\n", bib->security_target_count);
    cbor_encoder_create_array(&asbEncoder, &targetArrayEncoder, bib->security_target_count); 
 //   cbor_encode_uint(&targetArrayEncoder, 1);  // block number for payload    // HARDCODED  KRS
    cbor_encoder_close_container(&asbEncoder, &targetArrayEncoder);

    cbor_encode_uint(&asbEncoder, 13);//bib->context_id);       // HARDCODED  KRS
    cbor_encode_uint(&asbEncoder, 14);//bib->context_flags);    // HARDCODED  KRS

    CborEncoder sourceArrayEncoder;
    cbor_encoder_create_array(&asbEncoder, &sourceArrayEncoder, 2);      
    cbor_encode_uint(&sourceArrayEncoder, bib->srcscheme);
    CborEncoder sourceSspArrayEncoder;
    cbor_encoder_create_array(&sourceArrayEncoder, &sourceSspArrayEncoder, 2);  // HARDCODED assumption of ipn scheme  KRS
    cbor_encode_uint(&sourceSspArrayEncoder, bib->srcnode);
    cbor_encode_uint(&sourceSspArrayEncoder, bib->srcserv);
    cbor_encoder_close_container(&sourceArrayEncoder, &sourceSspArrayEncoder);
    cbor_encoder_close_container(&asbEncoder, &sourceArrayEncoder);
    //if (trace) bplog(NULL, 0, "step 1\n");

    // security context parameters if any  array of 2-tuple arrays  new  assume 0 for now  HARDCODED KRS
    CborEncoder contextParamsArrayEncoder;
    cbor_encoder_create_array(&asbEncoder, &contextParamsArrayEncoder, 0);
    cbor_encoder_close_container(&asbEncoder, &contextParamsArrayEncoder);
    //if (trace) bplog(NULL, 0, "step 1.1\n");

    // security results array of arrays of 2-tuple arrays  new
    CborEncoder securityResultsArrayEncoder;
    cbor_encoder_create_array(&asbEncoder, &securityResultsArrayEncoder, bib->security_result_length);
    uint64_t* results = (uint64_t*)&bib->security_result_data;
    //if (trace) bplog(NULL, 0, "step 1.2\n");

    // HARDCODED ASSUMPTION OF 1 UINT TARGET RESULT  KRS
    CborEncoder securityResultArrayEncoder;
    unsigned int i=0;
    //if (trace) bplog(NULL, 0, "step 1.23 %d\n", bib->security_result_length);
    for (i=0; i<bib->security_result_length; i+=2)
    {
        //if (trace) bplog(NULL, 0, "step 1.3\n");
        cbor_encoder_create_array(&securityResultsArrayEncoder, &securityResultArrayEncoder, 2);
        cbor_encode_uint(&securityResultArrayEncoder, results[i]);
        cbor_encode_uint(&securityResultArrayEncoder, results[i+1]);                               
        cbor_encoder_close_container(&securityResultsArrayEncoder, &securityResultArrayEncoder);
        //if (trace) bplog(NULL, 0, "step 1.4\n");
    }

    cbor_encoder_close_container(&asbEncoder, &securityResultsArrayEncoder);

    //size_t size = cborlen(buffer_asb, sizeof(buffer_asb));
    //if (trace) bplog(NULL, 0, "++++ %d\n", size);
//    CborParser asb_parser;
//    CborValue asb_it;
//    cbor_parser_init(buffer_asb, sizeof(buffer_asb), 0, &asb_parser, &asb_it);
    if (trace) if (dump_enable) bplog(NULL, 0, "BIB ASB:\n");
    if (trace) dump(buffer_asb, 50, 50, dump_enable);
#if 0
NEEDS TO REINIT AFTER EACH OBJECT
    unsigned int t = cbor_value_get_type(&asb_it);
    if (trace) bplog(NULL, 0, "value type is %x\n", t);
    if (trace) bplog(NULL, 0, "type is %x\n", cbor_value_get_type((CborValue*)buffer_asb));
    if (trace) bplog(NULL, 0, "buffer is at %p\n", buffer_asb);
    if (trace) bplog(NULL, 0, "next byte is at %p\n", cbor_value_get_next_byte(&asb_it));
    uint64_t v=22;
    //cbor_value_get_uint64(&asb_it, &v);
    CborValue r;
    cbor_value_enter_container(&asb_it, &r);
    if (trace) bplog(NULL, 0, "value type is %x\n", cbor_value_get_type(&asb_it));
    cbor_value_advance(&asb_it);  // target array
    cbor_value_leave_container(&asb_it, &r);
    if (trace) bplog(NULL, 0, "value type is %x\n", cbor_value_get_type(&asb_it));
    if (trace) bplog(NULL, 0, "step 2.1\n");
    cbor_value_advance(&asb_it);  // context_id
    if (trace) bplog(NULL, 0, "value type is %x\n", cbor_value_get_type(&asb_it));
    if (trace) bplog(NULL, 0, "step 2.2\n");
    cbor_value_advance(&asb_it);  // context flags
    if (trace) bplog(NULL, 0, "value type is %x\n", cbor_value_get_type(&asb_it));
    if (trace) bplog(NULL, 0, "step 2.3\n");
    cbor_value_advance(&asb_it);  // source array
    if (trace) bplog(NULL, 0, "value type is %x\n", cbor_value_get_type(&asb_it));
    if (trace) bplog(NULL, 0, "step 2.4\n");
    cbor_value_advance(&asb_it);  // context params array
    if (trace) bplog(NULL, 0, "value type is %x\n", cbor_value_get_type(&asb_it));
    if (trace) bplog(NULL, 0, "step 2.5\n");
    cbor_value_advance(&asb_it);  // resuls array
    if (trace) bplog(NULL, 0, "value type is %x\n", cbor_value_get_type(&asb_it));
    if (trace) bplog(NULL, 0, "step 2.6\n");
    size_t size = cbor_value_get_next_byte(&asb_it) - buffer_asb;
    if (trace) bplog(NULL, 0, "SIZE %d\n", size);
    cbor_encode_uint(&asbEncoder, 0x0001111112121212);
    uint8_t* p=buffer_asb;
    for (; *((uint32_t*)p) != 0x12121212; ++p)
        ;
    size_t size = p - buffer_asb - 5;
    //if (trace) bplog(NULL, 0, "encoded BIB size %d\n", size);
#endif
    size_t size = 0; ////////
    cbor_encode_byte_string(&arrayEncoder, buffer_asb, size);

    /* CRC if any */
    if (bib->context_id == BP_BIB_CRC16_X25)
    {
        cbor_encode_byte_string(&arrayEncoder, (uint8_t*)&results[i], 2);
        i += 2;
    }
    else if (bib->context_id == BP_BIB_CRC32_CASTAGNOLI)
    {
        cbor_encode_byte_string(&arrayEncoder, (uint8_t*)&results[i], 4);
        i += 4;
    }

    cbor_encoder_close_container(encoder, &arrayEncoder);

    //if(flags != 0)
    //{
    //    *flags_sum |= flags;
    //    return BP_ERROR;
    //}
    return BP_SUCCESS;
}

static int pay_cborize(CborEncoder* encoder, bp_blk_pay_t* pay, rb_tree_t* custody_tree)
{
///if (trace) fprintf(stderr, "pay_cborize: bpdu_transmission_id %d\n", pay->bpdu_transmission_id);
///if (trace) fprintf(stderr, "pay_cborize: bpdu_retransmission_time %d\n", pay->bpdu_retransmission_time);
    CborEncoder arrayEncoder;

    int num_items = 5;  // no CRC
    if (pay->crctype != bp_crctype_none)
        num_items = 6;
    cbor_encoder_create_array(encoder, &arrayEncoder, num_items);

    cbor_encode_uint(&arrayEncoder, BP_PAY_BLK_TYPE);    // block type
    cbor_encode_uint(&arrayEncoder, pay->blknum);           // block number, new to v7
    cbor_encode_uint(&arrayEncoder, pay->bf);               // block processing flags
    cbor_encode_uint(&arrayEncoder, pay->crctype);          // CRC type  new field in v7

    if (pay->admin_record_type == 0)  // vanilla bundle, which would be unusual  KRS
    {
        if (trace) fprintf(stderr, "ENCODE INNER/NORMAL PAYLOAD\n");
        cbor_encode_byte_string(&arrayEncoder, pay->payptr, pay->paysize);
    }
    
    else if (pay->admin_record_type == BP_ADMIN_REC_TYPE_BIBE)
    {
if (trace) fprintf(stderr, "ENCODE BIBE OUTER PAYLOAD\n");
        CborEncoder encoder2, arrayEncoder2, arrayEncoder3;
        uint8_t buffer2[OUTER_PAYLOAD_SIZE_MAX];
        cbor_encoder_init(&encoder2, buffer2, sizeof(buffer2), 0);
        cbor_encoder_create_array(&encoder2, &arrayEncoder2, 2);
        cbor_encode_uint(&arrayEncoder2, BP_ADMIN_REC_TYPE_BIBE);
        cbor_encoder_create_array(&arrayEncoder2, &arrayEncoder3, 3);
if (trace) fprintf(stderr, "encoding bpdu_transmission_id for outgoing bundle %d\n", pay->bpdu_transmission_id);
        cbor_encode_uint(&arrayEncoder3, pay->bpdu_transmission_id);
        cbor_encode_uint(&arrayEncoder3, pay->bpdu_retransmission_time);
        cbor_encode_byte_string(&arrayEncoder3, pay->payptr, pay->paysize);
        cbor_encoder_close_container(&arrayEncoder2, &arrayEncoder3);
        cbor_encoder_close_container(&encoder2, &arrayEncoder2);
        size_t len = cborlen(buffer2, sizeof(buffer2));
if (trace) fprintf(stderr, "pay_cborize: size == %d\n", len);
        cbor_encode_byte_string(&arrayEncoder, buffer2, len);
    }
    else if (pay->admin_record_type == BP_ADMIN_REC_TYPE_CUSTODY_SIGNAL)
    {
if (trace) fprintf(stderr, "encode custody signal payload\n");
        CborEncoder encoder2, arrayEncoder2, arrayEncoder3, arrayEncoder4, arrayEncoder5;
        uint8_t buffer2[CUSTODY_SIGNAL_PAYLOAD_SIZE_MAX];
        cbor_encoder_init(&encoder2, buffer2, sizeof(buffer2), 0);
        cbor_encoder_create_array(&encoder2, &arrayEncoder2, 2);
        cbor_encode_uint(&arrayEncoder2, BP_ADMIN_REC_TYPE_CUSTODY_SIGNAL);
        cbor_encoder_create_array(&arrayEncoder2, &arrayEncoder3, 2);
        cbor_encode_uint(&arrayEncoder3, 0);  // HARDWIRED TO MAGIC NUMBER FOR SUCCESSFUL CUSTODY ACCEPTED  KRS

        //// HARDWIRED TO 1 BUNDLE RANGE ACKNOWLEDGED
        cbor_encoder_create_array(&arrayEncoder3, &arrayEncoder4, 1);  
        cbor_encoder_create_array(&arrayEncoder4, &arrayEncoder5, 2);
        cbor_encode_uint(&arrayEncoder5, pay->bpdu_transmission_id);
if (trace) fprintf(stderr, "encoding first transmission id into custody signal: %d\n", pay->bpdu_transmission_id);
        cbor_encode_uint(&arrayEncoder5, 1);  // number of sequential transmission ids that custody signal is for
if (trace) fprintf(stderr, "encoding number of sequential transmission ids into custody signal: %d\n", 1);
        cbor_encoder_close_container(&arrayEncoder4, &arrayEncoder5);
        cbor_encoder_close_container(&arrayEncoder3, &arrayEncoder4);

        int count = 0;
        for (int i=0; i<max_gaps_per_custody_signal+1; ++i)
            if (pay->first_transmission_id[i] != 0)
                ++count;
#if 1  // KRS for CT
        if (trace) fprintf(stderr, "pay_cborize: see if custody tree is non empty\n");
        if (!rb_tree_is_empty(custody_tree))
        {
if (trace) fprintf(stderr, "pay_cborize: custody tree nonempty\n");
            rb_range_t range;
            int status = 0;
            status = rb_tree_goto_first(custody_tree);
            while (custody_tree->iterator != NULL)
            {
                status = rb_tree_get_next(custody_tree, &range, false, false);
                ////++count;
                count+=range.offset+1;
            }
        }
if (trace) fprintf(stderr, "pay_cborize: count of ranges to make == %d\n", count);
        cbor_encoder_create_array(&arrayEncoder3, &arrayEncoder4, count);  
        if (count > 0)
        {
            rb_range_t range;
            int status = rb_tree_goto_first(custody_tree);
            // TODO  MAKE CUSTODY SIGNALS WITH RANGES, IE, WITH NUM_TRANSMISSION_IDS > 1
            while (custody_tree->iterator != NULL)
            {
                //status = rb_tree_get_next(custody_tree, &range, false, false);
                status = rb_tree_get_next(custody_tree, &range, true, false);  //// KRS
                //status = rb_tree_get_next(custody_tree, &range, true, true);  //// KRS

                for (int i=0; i<range.offset+1; ++i)
                {
                cbor_encoder_create_array(&arrayEncoder4, &arrayEncoder5, 2);
                //cbor_encode_uint(&arrayEncoder5, pay->first_transmission_id[i]);
                //cbor_encode_uint(&arrayEncoder5, pay->num_transmission_ids[i]);  // number of sequential transmissions id that custody signal is for
//if (trace) fprintf(stderr, "id: %d\n", range+range.offset);
//if (trace) fprintf(stderr, "num: %d\n", 1);
if (trace) fprintf(stderr, "id: %d\n", range.value);
if (trace) fprintf(stderr, "num: %d\n", range.offset);
                ////cbor_encode_uint(&arrayEncoder5, range);
                cbor_encode_uint(&arrayEncoder5, range.value+i);
                cbor_encode_uint(&arrayEncoder5, 1);  // number of sequential transmissions id that custody signal is for
                ////cbor_encode_uint(&arrayEncoder5, range.offset+1);  // number of sequential transmissions id that custody signal is for
                cbor_encoder_close_container(&arrayEncoder4, &arrayEncoder5);
                }
            }
        }
#endif
        cbor_encoder_close_container(&arrayEncoder3, &arrayEncoder4);
        
        cbor_encoder_close_container(&arrayEncoder2, &arrayEncoder3);
        cbor_encoder_close_container(&encoder2, &arrayEncoder2);

        int siz = cbor_encoder_get_buffer_size(&encoder2, buffer2);
if (trace) fprintf(stderr, "pay_cborize: siz == %d\n", siz);
        cbor_encode_byte_string(&arrayEncoder, buffer2, siz);
    }
    else
    {
if (trace) fprintf(stderr, "ENCODE BAD PAYLOAD, admin rec type %d\n", pay->admin_record_type);
    }

    /* CRC if any */
#if 0
    if (bib->context_id == BP_BIB_CRC16_X25)
    {
        cbor_encode_byte_string(encoder, results[i], 2);
        i += 2;
    }
    else if (bib->context_id == BP_BIB_CRC32_CASTAGNOLI)
    {
        cbor_encode_byte_string(encoder, results[i], 4);
        i += 4;
    }
#endif

    cbor_encoder_close_container(encoder, &arrayEncoder);

    //if(flags != 0)
    //{
    //    *flags_sum |= flags;
    //    return BP_ERROR;
    //}
    return BP_SUCCESS;
}

static int bundle_encode(uint8_t * in, size_t size, uint8_t * buffer, size_t * out_size, int16_t local_service,
                         unsigned long bpdu_transmission_id, bool enbibe, rb_tree_t* custody_tree)  // KRS
{
    if (!enbibe && !custody_tree)
    {
        // do nothing
        *out_size = size;
        memcpy(buffer, in, *out_size);
        return BP_SUCCESS;
    }

    bp_blk_pri_t pri;
    bp_blk_bib_t bib;
    bp_blk_pay_t pay;

    CborEncoder encoder, arrayEncoder;

    if (custody_tree)
        if (trace) fprintf(stderr, "make custody signal bundle\n");
    if (trace && !brief) bplog(NULL, 0, "input buffer size %d\n", size);
    cbor_encoder_init(&encoder, buffer, *out_size, 0);
    cbor_encoder_create_array(&encoder, &arrayEncoder, CborIndefiniteLength);

    memset(&pri, 0, sizeof(pri));
    pri.version = 7;
    pri.allow_frag = 1;
    pri.crctype = 0;            // none
    pri.createmsec = bplib_os_get_dtntime_ms();
    pri.createseq = 1;
    pri.lifetime = 3600000;  // MAGIC NUMBER  KRS
    pri.dstnode = remote_node;
    pri.dstserv = remote_service;
    pri.is_admin_rec = true;
    pri.srcnode = local_node;
    pri.srcserv = local_service;
    pri_cborize(&arrayEncoder, &pri);
    if (trace && !brief) bplog(NULL, 0, "::::primary block encoded\n");
    if (trace && !brief) dump(buffer, 200, 200, dump_enable);

    memset(&bib, 0, sizeof(bib));
    bib.blknum = 2;  // per the standard
    bib.crctype = 0;
    bib_cborize(&arrayEncoder, &bib);
    if (trace && !brief) bplog(NULL, 0, "::::integrity block encoded\n");
    if (trace && !brief) dump(buffer, 200, 200, dump_enable);

    memset(&pay, 0, sizeof(pay));
    pay.blknum = 1;
    pay.crctype = 0;
    pay.payptr = in;
    pay.paysize = size;
    if (!custody_tree)
        pay.admin_record_type = 3;
    else
        pay.admin_record_type = 4;
    pay.bpdu_transmission_id = bpdu_transmission_id;
    pay.bpdu_retransmission_time = bplib_os_get_dtntime_ms();
    pay_cborize(&arrayEncoder, &pay, custody_tree);
    if (trace && !brief) bplog(NULL, 0, "::::payload block encoded\n");
    if (trace && !brief) dump(buffer, 200, 200, dump_enable);

    int status = cbor_encoder_close_container(&encoder, &arrayEncoder);
    if (status != BP_SUCCESS)
        return status;

    *out_size = cborlen(buffer, *out_size);
if (trace) fprintf(stderr, "encoded bundle size = %d\n", *out_size);
    return BP_SUCCESS;
}


/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

int setup_cla_bibe(bplib_routetbl_t *rtbl, int local_nod, uint16_t local_serv, int remote_nod, uint16_t remote_serv)
{
    cid_reuse = true;
    max_gaps_per_custody_signal = MAX_MAX_GAPS_PER_CUSTODY_SIGNAL;

    local_node = local_nod;
    local_service = local_serv;

    remote_node = remote_nod;
    remote_service = remote_serv;
    
    /* Initialize Active Table Signal */
    active_table_signal = bplib_os_createlock();
    if (!bp_handle_is_valid(active_table_signal))
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Failed to create lock for active table\n");
        return BP_ERROR;
    }

    /* Initialize Active Table Functions */
#if 0
    if (table_retransmit_order == BP_RETX_SMALLEST_CID)
    {
        active_table.create    = (bp_table_create_t)cbuf_create;
        active_table.destroy   = (bp_table_destroy_t)cbuf_destroy;
        active_table.add       = (bp_table_add_t)cbuf_add;
        active_table.next      = (bp_table_next_t)cbuf_next;
        active_table.remove    = (bp_table_remove_t)cbuf_remove;
        active_table.available = (bp_table_available_t)cbuf_available;
        active_table.count     = (bp_table_count_t)cbuf_count;
    }
    else 
#endif
    if (table_retransmit_order == BP_RETX_OLDEST_BUNDLE)
    {
        active_table.create    = (bp_table_create_t)rh_hash_create;
        active_table.destroy   = (bp_table_destroy_t)rh_hash_destroy;
        active_table.add       = (bp_table_add_t)rh_hash_add;
        active_table.next      = (bp_table_next_t)rh_hash_next;
        active_table.remove    = (bp_table_remove_t)rh_hash_remove;
        active_table.available = (bp_table_available_t)rh_hash_available;
        active_table.count     = (bp_table_count_t)rh_hash_count;
    }
    else
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Unrecognized attribute for creating active table: %d\n",
              table_retransmit_order);
        return BP_ERROR;
    }

    /* Initialize Active Table */
    int status = active_table.create(&active_table.table, BP_DEFAULT_ACTIVE_TABLE_SIZE);
    if (status != BP_SUCCESS)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Failed to create active table for channel\n");
        return BP_ERROR;
    }


/* (Optional) Global Custody ID */
#if BPLIB_GLOBAL_CUSTODY_ID
    bplib_custody_id_mutex  = bplib_os_createlock();
    bplib_global_custody_id = 1;  // starts at 1 because 0 means no custody transfer requested
#endif

    /* Allocate Memory for DACS Tree to Store Bundle IDs */
    status = rb_tree_create(max_gaps_per_custody_signal+1, &custody_tree);
    if (status != BP_SUCCESS)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Failed to allocate memory for DACS tree\n");
        return BP_ERROR;
    }

    /* Initialize Current Custody ID */
    current_active_cid = 0;

    return BP_SUCCESS;
}

int teardown_cla_bibe()
{
    /* Destroy Custody Tree Lock */
    if (bp_handle_is_valid(custody_tree_lock))
    {
        bplib_os_destroylock(custody_tree_lock);
        custody_tree_lock = BP_INVALID_HANDLE;
    }

    /* Free Custody Tree */
    rb_tree_destroy(&custody_tree);

/* (Optional) Global Custody ID */
#if BPLIB_GLOBAL_CUSTODY_ID
    if (bp_handle_is_valid(bplib_custody_id_mutex))
    {
        bplib_os_destroylock(bplib_custody_id_mutex);
        bplib_custody_id_mutex = BP_INVALID_HANDLE;
    }
#endif


    /* Un-initialize Active Table */
    if (bp_handle_is_valid(active_table_signal))
    {
        bplib_os_destroylock(active_table_signal);
        active_table_signal = BP_INVALID_HANDLE;  // KRS
    }
    if (active_table.destroy)
    {
        active_table.destroy(active_table.table);
    }

    return BP_SUCCESS;
}

//#define BPLIB_BLOCKTYPE_CLA_INGRESS_BLOCK 0x9580be4a
/**
 * @brief Receive complete bundle from a remote system
 *
 * This implements the "ingress" side of the CLA API.  Complete RFC 9171-compliant, CBOR-encoded bundles
 * are passed in, which will be parsed by BPLib to identify the internal fields.  The bundle will then
 * be forwarded internally through the routing table to either its destination (if local) or via a relay
 * storage (if configured) or to another CLA.
 *
 * @param rtbl Routing table instance
 * @param intf_id bp_handle_t value from bplib_create_cla_intf()
 * @param bundle Pointer to bundle buffer
 * @param size Size of encoded bundle
 * @param timeout Timeout
 * @retval BP_SUCCESS if successful
 */
int bplib_bibe_ingress(bplib_routetbl_t *rtbl, bp_handle_t intf_id, const void *bundle, size_t size, uint32_t timeout)
{
    int status = BP_ERROR;
#if 0
#define BPLIB_BLOCKTYPE_CLA_INTF          0x7b643c85 ///
    bplib_mpool_ref_t  flow_ref;
    bplib_cla_stats_t *stats;
    uint64_t           ingress_time_limit;

    flow_ref = bplib_route_get_intf_controlblock(rtbl, intf_id);
    if (flow_ref == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Intf ID invalid\n");
        return BP_ERROR;
    }

    stats = bplib_mpool_generic_data_cast(bplib_mpool_dereference(flow_ref), BPLIB_BLOCKTYPE_CLA_INTF);
    if (stats == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Intf ID is not a CLA\n");
        status = BP_ERROR;
    }
    else
    {
        if (timeout == 0)
        {
            ingress_time_limit = 0;
        }
        else
        {
            ingress_time_limit = bplib_os_get_dtntime_ms() + timeout;
        }

        status = bplib_generic_bundle_ingress(flow_ref, bundle, size, ingress_time_limit);

        if (status == BP_SUCCESS)
        {
            stats->ingress_byte_count += size;
        }
    }

    bplib_route_release_intf_controlblock(rtbl, flow_ref);

    /* trigger the maintenance task to run */
    bplib_route_set_maintenance_request(rtbl);
#endif

    size_t data_fill_sz = size;
    static uint8_t * bundle_buffer = NULL;
if (trace) fprintf(stderr, "bundle that just came in:\n");
    dump((uint8_t *)bundle, 200, 200, dump_enable);

    bp_blk_pay_t pay;
    memset(&pay, 0, sizeof(pay));
    status = bundle_decode(bundle, size, &bundle_buffer, &data_fill_sz, &pay); // debibe, or decode custody signal KRS
    if (status != BP_SUCCESS)
        return status;

    if (pay.admin_record_type == 0)
    {
        ; // nothing to do
    }
    else if (pay.admin_record_type == BP_ADMIN_REC_TYPE_CUSTODY_SIGNAL)  // if is custody sig   // KRS for CT
    {
        // consume and process- remove from active table
        bp_active_bundle_t active_bundle;
        bplib_os_lock(active_table_signal);
        {
            //while(ch->active_table.next(ch->active_table.table, &active_bundle) == BP_SUCCESS)
            int num = active_table.count(active_table.table);
            {
                int i=0;
                for (int num_found=0; num_found<num; ++i)
                {
                    /* Move to Next Oldest */
                    //if (ch->active_table.next(ch->active_table.table, &active_bundle) == BP_SUCCESS)
                    //    if (trace) fprintf(stderr, "sid: %d   cid: %d\n", active_bundle.sid, active_bundle.cid);
                    //else
                    //    if (trace) fprintf(stderr, "NONSUCCESS IN DUMP\n");

                    int status = active_table.remove(active_table.table, i, &active_bundle);
                    if (status == BP_SUCCESS)
                    {
//if (trace) fprintf(stderr, "sid: %u   i: %d cid: %d\n", active_bundle.sid, i, active_bundle.cid);
                        ++num_found;
                        int j=0;
                        for (; j<max_gaps_per_custody_signal+1 && pay.first_transmission_id[j] != active_bundle.cid; ++j)
                            ;
                        if (j == max_gaps_per_custody_signal+1)
                            active_table.add(active_table.table, active_bundle, false);  // not one to remove, put it back
                        else
if (trace) fprintf(stderr, "bundle custody signal received and processed for bundle %d\n", active_bundle.cid);
                    }
                }
            }
        }
        bplib_os_unlock(active_table_signal);
    }
    else if (pay.admin_record_type == BP_ADMIN_REC_TYPE_BIBE)
    {
if (trace) fprintf(stderr, "Call system bplib_cla_ingress()... size=%zu\n", data_fill_sz);
        status = bplib_cla_ingress(rtbl, intf_id, bundle_buffer, data_fill_sz, MAX_WAIT_MSEC);
        if (status != BP_SUCCESS)
        {
            if (status != BP_TIMEOUT)
            {
                fprintf(stderr, "Failed bplib_cla_ingress() code=%zd\n", status);
            }
        }
        else
        {
            // record bundle in custody tree  KRS for CT
            //int insert_status = rb_tree_insert(payload.cid, &custody_tree);
            int insert_status = rb_tree_insert(pay.bpdu_transmission_id, &custody_tree);
            if (insert_status != BP_SUCCESS)
            {
                /* There is no valid reason for an insert to fail on an empty custody_tree */
                bplog(NULL, BP_FLAG_DIAGNOSTIC, "Unexpected error encountered on empty custody tree: %d\n",
                        insert_status);
            }
        }
    }
    else
    {
fprintf(stderr, "Unsupported record type %d; comm error?\n", pay.admin_record_type);
    }

    return status;
}

/**
 * @brief Send complete bundle to remote system
 *
 * Retrieve the next complete RFC 9171-compliant, CBOR-encoded bundle from the local system, which was routed to the
 * specified CLA interface.  This data may then be forwarded to the CLA implementation for actual transmission to
 * a remote node.
 *
 * @param rtbl Routing table instance
 * @param intf_id bp_handle_t value from bplib_create_cla_intf()
 * @param bundle Pointer to bundle buffer
 * @param[inout] size Size of buffer on input, size of actual bundle on output
 * @param timeout Timeout
 * @retval BP_SUCCESS if successful
 */
int bplib_bibe_egress(bplib_routetbl_t *rtbl, bp_handle_t intf_id, void *bundle, size_t *size, uint32_t timeout)
{
    int status = BP_SUCCESS;

    static uint8_t bundle_buffer[INNER_BUNDLE_SIZE_MAX];
    size_t bundle_buffer_sz = sizeof(bundle_buffer);

    bool newcid = !cid_reuse;

    bool bibe = true;
#define CUSTODY_SIGNAL_INTERVAL_MSEC /*5000*/ 0
    static uint64_t last_time_cs_sent = bplib_os_get_dtntime_ms();
    uint64_t now = bplib_os_get_dtntime_ms();
    bool make_custody_signal =  now - last_time_cs_sent >= CUSTODY_SIGNAL_INTERVAL_MSEC && !rb_tree_is_empty(&custody_tree);  // KRS for CT
    if (!make_custody_signal)
    {
        status = bplib_cla_egress(rtbl, intf_id, bundle_buffer, &bundle_buffer_sz, MAX_WAIT_MSEC);
        if (status == BP_SUCCESS)
        {
if (trace) fprintf(stderr, "inner bundle from bplib of size %ld:\n", bundle_buffer_sz);
            dump(bundle_buffer, bundle_buffer_sz, 200, dump_enable);
        }
    }

    unsigned long bpdu_transmission_id = 0;
    if (status == BP_SUCCESS)
    {
        memset(bundle, 0, *size);  // so both easier to debug and more consistent errors
#if BPLIB_GLOBAL_CUSTODY_ID
        bplib_os_lock(bplib_custody_id_mutex);
        {
            bpdu_transmission_id = bplib_global_custody_id++;
            current_active_cid = bplib_global_custody_id;
        }
        bplib_os_unlock(bplib_custody_id_mutex);
#else
        bpdu_transmission_id = current_active_cid++;
#endif
        status = bundle_encode(bundle_buffer, bundle_buffer_sz, bundle, size, local_service, bpdu_transmission_id, bibe
                               make_custody_signal ? &custody_tree : NULL);  // enbibe or make custody signal
        if (status == BP_SUCCESS)
        {
if (trace) fprintf(stderr, "bundle ready to go:\n");
            dump(bundle, 200, 200, dump_enable);
        }
    }
    else if (status != BP_TIMEOUT)
    {
        fprintf(stderr, "Failed bplib_cla_egress() code=%zd\n", status);
    }

    if (status == BP_SUCCESS)
    {
        if (make_custody_signal)  // KRS for CT
        {
            // bundle_encode() already removed entries from custody tree
         
            last_time_cs_sent = now;
        }
        else  // we fetched, enbibed, and sent a data bundle
        {
            if (bpdu_transmission_id != 0)  // custody transfer requested
            {
                /* Save/Update Storage ID */
                static bp_sid_t next_sid = 1;
                bp_active_bundle_t active_bundle;
                //active_bundle.sid = object->header.sid;
                active_bundle.sid = next_sid++;     // can probably eliminate sid  KRS

                active_bundle.cid = bpdu_transmission_id;

                /* Update Retransmit Time */
                active_bundle.retx = bplib_os_get_dtntime_ms();

                /* Save Bundle as Active */
                bplib_os_lock(active_table_signal);
                {
                    /* Assign New Custody ID */
                    if (newcid)
                    {
#if BPLIB_GLOBAL_CUSTODY_ID
                        bplib_os_lock(bplib_custody_id_mutex);
                        {
                            active_bundle.cid  = bplib_global_custody_id++;
                            current_active_cid = bplib_global_custody_id;
                        }
                        bplib_os_unlock(bplib_custody_id_mutex);
#else
                        active_bundle.cid = current_active_cid++;
#endif
                    }

                    /* Update Active Table */
                    status = active_table.add(active_table.table, active_bundle, !newcid);
                    if (status == BP_DUPLICATE)
                    {
                        //bplog(flags, BP_FLAG_DUPLICATES, "Duplicate bundle detected in active table, CID=%lu\n",
                        bplog(NULL, BP_FLAG_DUPLICATES, "Duplicate bundle detected in active table, CID=%lu\n",
                            (unsigned long)active_bundle.cid);
                    }
                    else if (status != BP_SUCCESS)
                    {
                        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Unexpected error adding bundle to active table: %d\n", status);
                    }
                }
                bplib_os_unlock(active_table_signal);
            }
        }
    }

    return status;
}
