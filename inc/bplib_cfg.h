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

#ifndef BPLIB_CFG_H
#define BPLIB_CFG_H

#ifdef __cplusplus
extern "C" {
#endif


/*
** Macros
*/

/** 
 * \brief Maximum length of EID strings and other strings
 */

#define BPLIB_MAX_EID_LENGTH                256 /* Maximum EID string length */
#define BPLIB_MAX_NUM_STRING                24  /* Maximum string length */

/**
 * \brief Configuration array constraints
 */
#define BPLIB_MAX_NUM_BUNDLE_QUEUES         16  /* Maximum number of queues */
#define BPLIB_MAX_NUM_SOURCE_EID            16  /* Maximum number of allowed source EIDs */
#define BPLIB_MAX_NUM_LATENCY_POLICY_SETS   10  /* Maximum number of latency policy sets */
#define BPLIB_MAX_NUM_STORE_SET             10  /* Maximum number of storage policy sets. */
#define BPLIB_MAX_NUM_CRS                   10  /* Maximum number of Compressed Reporting CRS Entries */
#define BPLIB_MAX_AUTH_SOURCE_EIDS          10  /* Maximum number of authorized source EID patterns */
#define BPLIB_MAX_AUTH_CUSTODIAN_EIDS       10  /* Maximum number of authorized custodian EID patterns */
#define BPLIB_MAX_AUTH_CUSTODY_SOURCE_EIDS  10  /* Maximum number of authorized custody source EID patterns */
#define BPLIB_MAX_AUTH_REPORT_TO_EIDS       10  /* Maximum number of report-to EID patterns */
#define BPLIB_MAX_NUM_MIB_PS_CFG_ENTRIES    10  /* Maximum number of entries in MIB PS Cfg table */
#define BPLIB_MAX_NUM_MIB_PS_EID_PATTERNS   10  /* Maximum number of EID patterns for each MIB PS Cfg Table Entry */
#define BPLIB_MAX_NUM_STORE_EIDS            10  /* Maximum number of EID patterns per storage policy set */





/** 
 * \brief Maximum number of contacts that can be running at once
 *          This drives the number of entries in the CLA configuration
 *          tables, as well as the number of CLA In/Out tasks in BPNode
 */
#define BPLIB_MAX_NUM_CONTACTS              1

/**
 * \brief Maximum number of destination EID patterns per contact
 */
#define BPLIB_MAX_CONTACT_DEST_EIDS  3

/** 
 * \brief Maximum number of channels that can be running at once
 *          This drives the number of entries in the channel and ADU proxy configuration
 *          tables, as well as the number of ADU In/Out tasks in BPNode
 */
#define BPLIB_MAX_NUM_CHANNELS              2

/** 
 * \brief Maximum number of extension blocks per bundle.
 */
#define BPLIB_MAX_NUM_EXTENSION_BLOCKS       4

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* BPLIB_CFG_H */
