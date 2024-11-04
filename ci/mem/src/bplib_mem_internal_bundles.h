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

#ifndef BPLIB_MEM_INTERNAL_BUNDLES_H
#define BPLIB_MEM_INTERNAL_BUNDLES_H

typedef enum BPLib_MEM_Adminrectype
{
    BPLib_MEM_AdminrectypeUndefined              = 0,
    BPLib_MEM_AdminrectypeStatusreport           = 1,
    BPLib_MEM_AdminrectypeCustodyacknowledgement = 4,

    BPLib_MEM_AdminrectypeMax = 5

} BPLib_MEM_Adminrectype_t;

typedef enum BPLib_MEM_IanaUriScheme
{
    BPLib_MEM_IanaUriSchemeUndefined = 0,
    BPLib_MEM_IanaUriSchemeDtn       = 1,
    BPLib_MEM_IanaUriSchemeIpn       = 2
} BPLib_MEM_IanaUriScheme_t;

typedef enum BPLib_MEM_EndpointidScheme
{
    BPLib_MEM_EndpointidSchemeUndefined = 0,
    BPLib_MEM_EndpointidSchemeDtn       = 1,
    BPLib_MEM_EndpointidSchemeIpn       = 2
} BPLib_MEM_EndpointidScheme_t;

typedef uint64_t BPLib_MEM_IpnNodenumber_t;
typedef uint64_t BPLib_MEM_IpnServicenumber_t;

typedef struct BPLib_MEM_IpnUriSsp
{
    BPLib_MEM_IpnNodenumber_t    node_number;
    BPLib_MEM_IpnServicenumber_t service_number;
} BPLib_MEM_IpnUriSsp_t;

typedef struct BPLib_MEM_DtnUriSsp
{
    bool is_none; /**< indicates the special address "dtn:none" per RFC9171 section 4.2.5.1.1 */
} BPLib_MEM_DtnUriSsp_t;

typedef uint64_t BPLib_MEM_SequenceNumber_t;

typedef struct BPLib_MEM_CreationTimestamp
{
    uint64_t            time;
    BPLib_MEM_SequenceNumber_t sequence_num;
} BPLib_MEM_CreationTimestamp_t;

typedef struct BPLib_MEM_BundleProcessingControlFlags
{
    /**
     * @brief Bundle deletion status reports are requested.
     */
    bool deletion_status_req; /* 1   bits/1   bytes */

    /**
     * @brief Bundle delivery status reports are requested.
     */
    bool delivery_status_req; /* 1   bits/1   bytes */

    /**
     * @brief Bundle forwarding status reports are requested.
     */
    bool forwarding_status_req; /* 1   bits/1   bytes */

    /**
     * @brief Bundle reception status reports are requested.
     */
    bool reception_status_req; /* 1   bits/1   bytes */

    /**
     * @brief Status time is requested in all status reports.
     */
    bool statusTimeRequested; /* 1   bits/1   bytes */

    /**
     * @brief User application acknowledgement is requested.
     */
    bool acknowledgementRequested; /* 1   bits/1   bytes */

    /**
     * @brief Bundle must not be fragmented.
     */
    bool mustNotFragment; /* 1   bits/1   bytes */

    /**
     * @brief Payload is an administrative record.
     */
    bool isAdminRecord; /* 1   bits/1   bytes */

    /**
     * @brief Bundle is a fragment.
     */
    bool isFragment; /* 1   bits/1   bytes */
} BPLib_MEM_BundleProcessingControlFlags_t;

typedef struct BPLib_MEM_BlockProcessingFlags
{

    /**
     * @brief Block must be removed from bundle if it can't be processed.
     */
    bool must_remove; /* 1   bits/1   bytes */

    /**
     * @brief Bundle must be deleted if block can't be processed.
     */
    bool must_delete; /* 1   bits/1   bytes */

    /**
     * @brief Transmission of a status report is requested if block can't be processed.
     */
    bool xmit_status; /* 1   bits/1   bytes */

    /**
     * @brief Block must be replicated in every fragment.
     */
    bool must_replicate; /* 1   bits/1   bytes */

} BPLib_MEM_BlockProcessingFlags_t;

typedef uint64_t BPLib_MEM_Lifetime_t;
typedef uint64_t BPLib_MEM_AduLength_t;

typedef union BPLib_MEM_EndpointidSsp
{
    BPLib_MEM_DtnUriSsp_t dtn; /* present if scheme == BPLib_MEM_EndpointidSchemeDtn */
    BPLib_MEM_IpnUriSsp_t ipn; /* present if scheme == BPLib_MEM_EndpointidSchemeIpn */
} BPLib_MEM_EndpointidSsp_t;

typedef struct BPLib_MEM_EidBuffer
{
    BPLib_MEM_EndpointidScheme_t scheme; /* always present, indicates which union field is valid */
    BPLib_MEM_EndpointidSsp_t    ssp;
} BPLib_MEM_EidBuffer_t;

#endif /* BPLIB_MEM_INTERNAL_BUNDLES_H */
