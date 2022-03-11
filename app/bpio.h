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

#ifndef BPIO_H
#define BPIO_H

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

#endif /* BPIO_H */
