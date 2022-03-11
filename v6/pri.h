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

#ifndef PRI_H
#define PRI_H

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bundle_types.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* Bundle Primary Block */
typedef struct
{
    /* field data */
    uint8_t    version;    /* bundle protocol version */
    bp_field_t pcf;        /* process control flags */
    bp_field_t blklen;     /* block length */
    bp_field_t dstnode;    /* destination node */
    bp_field_t dstserv;    /* destination service */
    bp_field_t srcnode;    /* source node */
    bp_field_t srcserv;    /* source service */
    bp_field_t rptnode;    /* report to node */
    bp_field_t rptserv;    /* report to service */
    bp_field_t cstnode;    /* custody node */
    bp_field_t cstserv;    /* custody service */
    bp_field_t createsec;  /* creation timestamp - seconds */
    bp_field_t createseq;  /* creation timestamp - sequence number */
    bp_field_t lifetime;   /* bundle lifetime (seconds since J2000) */
    bp_field_t dictlen;    /* dictionary length in bytes */
    bp_field_t fragoffset; /* payload fragment offset in bytes */
    bp_field_t paylen;     /* payload length in bytes */
    /* meta information */
    bool    is_admin_rec; /* 0: not admin, 1: is admin */
    bool    is_frag;      /* 0: is not a fragment, 1: is a fragment */
    bool    allow_frag;   /* 0: fragmentation not allowed, 1: fragmentation allowed */
    bool    cst_rqst;     /* 0: custody not requested, 1: custody requested */
    bool    ack_app;      /* 0: no acknowledgement from application, 1: acknowledgement requested */
    uint8_t cos;          /* class of service - 0: bulk, 1: normal, 2: expedited */
} bp_blk_pri_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int pri_read(const void *block, int size, bp_blk_pri_t *pri, bool update_indices, uint32_t *flags);
int pri_write(void *block, int size, bp_blk_pri_t *pri, bool update_indices, uint32_t *flags);

#endif /* PRI_H */
