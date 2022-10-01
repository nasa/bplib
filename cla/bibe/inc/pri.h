/************************************************************************
 * File: pri.h
 *
 *  Copyright 2019-2022 United States Government as represented by the 
 *  Administrator of the National Aeronautics and Space Administration. 
 *
 *  This software was created at NASA's Goddard Space Flight Center.
 *
 * Maintainer(s):
 *  Karl Sipfle, for Code 580 NASA GSFC
 *
 *************************************************************************/

#ifndef _pri_h_
#define _pri_h_

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "../../tinycbor-main/src/cbor.h"  // KRS (ION also has a cbor.h)
//#include "bplib.h"
#include "../inc/bplib.h"  // KRS
//#include "bundle_types.h"
#include "../lib/bundle_types.h"  // KRS

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* Bundle Primary Block */
typedef struct {
    /* field data */
    uint8_t     version;           /* bundle protocol version */
    unsigned long  pcf;            /* process control flags */
    unsigned long  crctype; // v7
    unsigned long  dstscheme;        /* destination node */
    unsigned long  dstnode;        /* destination node */
    unsigned long  dstserv;        /* destination service */
    unsigned long  srcscheme;        /* destination node */
    unsigned long  srcnode;        /* source node */
    unsigned long  srcserv;        /* source service */
    unsigned long  rptscheme;        /* destination node */
    unsigned long  rptnode;        /* report to node */
    unsigned long  rptserv;        /* report to service */
    unsigned long  createmsec;     /* creation timestamp - milliseconds */  // milliseconds in v7
    unsigned long  createseq;      /* creation timestamp - sequence number */
    unsigned long  lifetime;       /* bundle lifetime (since J2000) */  // milliseconds in v7
    unsigned long  fragoffset;     /* payload fragment offset in bytes */
    unsigned long  paylen;         /* payload length in bytes */
    unsigned long  crc;         
    /* meta information */
    bool        is_admin_rec;   /* 0: not admin, 1: is admin */
    bool        is_frag;        /* 0: is not a fragment, 1: is a fragment */
    bool        allow_frag;     /* 0: fragmentation not allowed, 1: fragmentation allowed */
    bool        ack_app;        /* 0: no acknowledgement from application, 1: acknowledgement requested */
    //bool        cst_rqst;
} bp_blk_pri_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

//int pri_decborize(CborValue *it, int size, bp_blk_pri_t* pri, uint32_t* flags);
//int pri_cborize (CborEncoder* encoder, bp_blk_pri_t* pri);

#endif  /* _pri_h_ */
