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
 * \brief Maximum length of EID strings
 */

#define BPLIB_MAX_EID_LENGTH                256 /* Maximum EID string length */
#define BPLIB_MAX_NUM_BUNDLE_QUEUES         16  /* Maximum number of queuses */
#define BPLIB_MAX_NUM_SOURCE_EID            16  /* Maximum number of allowed source EIDs */
#define BPLIB_MAX_NUM_STRING                24  /* Maximum string length */
    
/** 
 * \brief Maximum number of contacts that can be running at once
 *          This drives the number of entries in the CLA configuration
 *          tables, as well as the number of CLA In/Out tasks in BPNode
 */
#define BPLIB_MAX_NUM_CONTACTS              1
    
/** 
 * \brief Maximum number of channels that can be running at once
 *          This drives the number of entries in the channel and ADU proxy configuration
 *          tables, as well as the number of ADU In/Out tasks in BPNode
 */
#define BPLIB_MAX_NUM_CHANNELS              2

#define BPLIB_MAX_NUM_CONTACTS              1

/** 
 * \brief Maximum number of canonical blocks per bundle
 */
#define BPLIB_MAX_NUM_CANONICAL_BLOCK       10

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* BPLIB_CFG_H */
