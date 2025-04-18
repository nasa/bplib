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

#ifndef BPLIB_NC_INTERNAL_H
#define BPLIB_NC_INTERNAL_H

/*
** Include
*/

#include "bplib_api_types.h"
#include "bplib_nc_payloads.h"


/*
** Global Data
*/

extern BPLib_SourceMibConfigHkTlm_Payload_t    BPLib_NC_SourceMibConfigPayload;
extern BPLib_NodeMibConfigHkTlm_Payload_t      BPLib_NC_NodeMibConfigPayload;
extern BPLib_ChannelContactStatHkTlm_Payload_t BPLib_NC_ChannelContactStatsPayload;


/*
** Function Definitions
*/

void BPLib_NC_UpdateContactHkTlm(void);

#endif // BPLIB_NC_INTERNAL_H