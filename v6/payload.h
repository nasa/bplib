/************************************************************************
 * File: payload.h
 *
 *  Copyright 2019 United States Government as represented by the 
 *  Administrator of the National Aeronautics and Space Administration. 
 *  All Other Rights Reserved.  
 *
 *  This software was created at NASA's Goddard Space Flight Center.
 *  This software is governed by the NASA Open Source Agreement and may be 
 *  used, distributed and modified only pursuant to the terms of that 
 *  agreement.
 *
 * Maintainer(s):
 *  Joe-Paul Swinski, Code 582 NASA GSFC
 *
 *************************************************************************/

#ifndef __BPLIB_PAYLOAD_H__
#define __BPLIB_PAYLOAD_H__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bplib_os.h"
#include "pri.h"
#include "bib.h"
#include "pay.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* Payload Data */
typedef struct {
    bool                request_custody;    /* boolean whether original bundle requested custody on payload delivery */
    int                 payloadsize;        /* size of the payload */
} bp_payload_data_t;

/* Payload */
typedef struct {
    bp_attr_t*          attributes;         /* pointer to channel attributes */
    bp_store_t*         store;              /* pointer to storage service for payloads */
    int                 handle;             /* storage service handle for payload */
    bp_payload_data_t   data;               /* serialized and stored payload data */    
} bp_payload_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int     payload_initialize      (bp_payload_t* payload, bp_attr_t* attr, bp_store_t* service, uint16_t* flags);
void    payload_uninitialize    (bp_payload_t* payload);

int     payload_receive         (bp_payload_t* payload, bool cst_rqst, uint8_t* pay, int pay_size, int timeout, uint16_t* flags);

#endif  /* __BPLIB_PAYLOAD_H__ */
