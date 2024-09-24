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

#ifndef BPLIB_NC_PAYLOADS_H
#define BPLIB_NC_PAYLOADS_H

#include "bplib_fwp.h"

typedef struct
{
    uint8_t ExampleParameter;
} BPLib_ResetCounterCmd_Payload_t;

typedef struct
{
    uint8_t ExampleParameter;
} BPLib_ResetSourceCountersCmd_Payload_t;

typedef struct
{
    uint8_t ExampleParameter;
} BPLib_AddApplicationCmd_Payload_t;

typedef struct
{
    uint8_t ExampleParameter;
} BPLib_RemoveApplicationCmd_Payload_t;

typedef struct
{
    uint8_t ExampleParameter;
} BPLib_SetRegistrationStateCmd_Payload_t;

typedef struct
{
    uint8_t ExampleParameter;
} BPLib_StartApplicationCmd_Payload_t;

typedef struct
{
    uint8_t ExampleParameter;
} BPLib_StopApplicationCmd_Payload_t;

typedef struct
{
    uint8_t ExampleParameter;
} BPLib_AddAuthSourcesCmd_Payload_t;

typedef struct
{
    uint8_t ExampleParameter;
} BPLib_RemoveAuthSourcesCmd_Payload_t;

typedef struct
{
    uint8_t ExampleParameter;
} BPLib_AddAuthCustodySourcesCmd_Payload_t;

typedef struct
{
    uint8_t ExampleParameter;
} BPLib_RemoveAuthCustodySourcesCmd_Payload_t;

typedef struct
{
    uint8_t ExampleParameter;
} BPLib_AddAuthCustodiansCmd_Payload_t;

typedef struct
{
    uint8_t ExampleParameter;
} BPLib_RemoveAuthCustodiansCmd_Payload_t;

typedef struct
{
    uint8_t ExampleParameter;
} BPLib_AddAuthReportToEidCmd_Payload_t;

typedef struct
{
    uint8_t ExampleParameter;
} BPLib_RemoveAuthReportToEidCmd_Payload_t;

typedef struct
{
    uint8_t ExampleParameter;
} BPLib_AddLatencyCmd_Payload_t;

typedef struct
{
    uint8_t ExampleParameter;
} BPLib_RemoveLatencyCmd_Payload_t;

typedef struct
{
    uint8_t ExampleParameter;
} BPLib_ContactSetupCmd_Payload_t;

typedef struct
{
    uint8_t ExampleParameter;
} BPLib_ContactStartCmd_Payload_t;

typedef struct
{
    uint8_t ExampleParameter;
} BPLib_ContactStopCmd_Payload_t;

typedef struct
{
    uint8_t ExampleParameter;
} BPLib_ContactTeardownCmd_Payload_t;

typedef struct
{
    uint8_t ExampleParameter;
} BPLib_AddMibArrayKeyCmd_Payload_t;

typedef struct
{
    uint8_t ExampleParameter;
} BPLib_RemoveMibArrayKeyCmd_Payload_t;

typedef struct
{
    uint8_t ExampleParameter;
} BPLib_SetMibItemCmd_Payload_t;

typedef struct
{
    uint8_t ExampleParameter;
} BPLib_AddStorageAllocationCmd_Payload_t;

typedef struct
{
    uint8_t ExampleParameter;
} BPLib_RemoveStorageAllocationCmd_Payload_t;

#endif // BPLIB_NC_PAYLOADS_H