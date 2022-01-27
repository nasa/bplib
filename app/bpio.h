/************************************************************************
 * File: bpio.h
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

#ifndef __bpio_h__
#define __bpio_h__

/*************************************************************************
 * Includes
 *************************************************************************/

#include "bplib.h"

/*************************************************************************
 * Defines
 *************************************************************************/

#define PARM_STR_SIZE     64
#define DFLT_SRC_NODE     4
#define DFLT_SRC_SERV     1
#define DFLT_DATA_IP_ADDR "127.0.0.1"
#define DFLT_DATA_PORT    34500
#define DFLT_DACS_IP_ADDR "127.0.0.1"
#define DFLT_DACS_PORT    34501
#define BPLIB_TIMEOUT     1000

/*************************************************************************
 * Typedefs
 *************************************************************************/

typedef struct
{
    bp_desc_t *bpc;
    char       data_ip_addr[PARM_STR_SIZE];
    int        data_port;
    char       dacs_ip_addr[PARM_STR_SIZE];
    int        dacs_port;
} thread_parm_t;

#endif /* __bpio_h__ */