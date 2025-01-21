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

#ifndef BPLIB_EID_H
#define BPLIB_EID_H

/* ======= */
/* Include */
/* ======= */

#include "bplib_api_types.h"

/* ====== */
/* Macros */
/* ====== */

/* ATTN: Macros here */

/* ======== */
/* Typedefs */
/* ======== */

/* EID schema (Authority.Node.Service) */
typedef struct
{
    uint64_t Authority; /* Defines a set of syntactic and semantic rules that fully explain how to parse and interpret the scheme-specific part (SSP) */
    uint64_t Node;      /* System implementing a set of DTN communications protocol services identified by a unique node ID */
    uint64_t Service;   /* DTN communication protocol service */
} BPLib_EID_t

/* ================== */
/* Exported Functions */
/* ================== */

/** TODO:
 * \brief     
 * \details   
 * \note      
 * \param[in] 
 * \param[in] 
 * \param[in] 
 * \return    
 * \secreflist
 * \refitem   
 * \refitem   
 * \refitem   
 * \refitem   
 * \endsecreflist
 * \anchor    
 */

#endif /* BPLIB_EID_H */
