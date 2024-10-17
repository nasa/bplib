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

#ifndef BPLIB_NC_H
#define BPLIB_NC_H

/* ======== */
/* Includes */
/* ======== */

#include "bplib_em.h"
#include "bplib_nc_payloads.h"
#include "bplib_fwp.h"
#include "bplib_eventids.h"

/* =================== */
/* Function Prototypes */
/* =================== */

/**
  * \brief     Initialize NC
  * \details   Node Configuration initialization
  * \note      As of right now, this function always returns BPLIB_SUCCESS
  * \param[in] void No arguments accepted
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_Init(void);

/**
  * \brief     Send a no-op that sends an event containing the version number for NC
  * \details   Node Configuration no-operation command
  * \note      This command is mostly handled by DP, this NC function simply returns BPLIB_SUCCESS
  * \param[in] void No arguments accepted
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_Noop(void);

/**
  * \brief     Run add-application for all applications configured to start at startup
  * \details   Node Configuration Add All Applications command
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] void No arguments accepted
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_AddAllApplications(void);

/**
  * \brief     Start accepting and delivering ADUs for all configured applications
  * \details   Node Configuration Start All Applications command
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] void No arguments accepted
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_StartAllApplications(void);

/**
  * \brief     Verify headers of all bundles in persistent storage on a cold start
  * \details   Node Configuration Verify Bundle Storage command
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] void No arguments accepted
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_VerifyBundleStorage(void);

/**
  * \brief     Delete all bundles in storage for a factory reset
  * \details   Node Configuration Initialize Bundle Storage command
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] void No arguments accepted
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_InitBundleStorage(void);

/**
  * \brief     Verify bundle metadata reflects actual bundle storage
  * \details   Node Configuration Verify Bundle Metadata command
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] void No arguments accepted
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_VerifyBundleMetadata(void);

/**
  * \brief     Rebuild bundle storage metadata from stored bundles, including their indices (source EID, destination EID, next action time)
  * \details   Node Configuration Rebuild Bundle Metadata command
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] void No arguments accepted
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_RebuildBundleMetadata(void);

/**
  * \brief     Clear volatile data, including bundle metadata and bundles not in persistent storage
  * \details   Node Configuration Clear Volatile command
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] void No arguments accepted
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_ClearVolatile(void);

/**
  * \brief     Reload saved node configuration, bundle metadata from persistent storage, and saved MIB counters
  * \details   Node Configuration Reload Saved Data command
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] void No arguments accepted
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_ReloadSavedData(void);

/**
  * \brief     Reload saved node configuration, bundle metadata from persistent storage, and saved MIB counters
  * \details   Node Configuration Reload Saved Data command
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] void No arguments accepted
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_ResetAllCounters(void);

/**
  * \brief     Set given MIB counter to zero. If targeted counter is node-only, source EID is unused.
  * \details   Node Configuration Reset Counter command
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] Payload BPLib_ResetCounterCmd_Payload_t type found in bplib_nc_payloads.h
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_ResetCounter(const BPLib_ResetCounterCmd_Payload_t Payload);

/**
  * \brief     Set all resettable MIB counters associated with given source EID pattern to zero
  * \details   Node Configuration Reset Source Counters command
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] Payload BPLib_ResetSourceCountersCmd_Payload_t type found in bplib_nc_payloads.h
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_ResetSourceCounters(const BPLib_ResetSourceCountersCmd_Payload_t Payload);

/**
  * \brief     Set all bundle-related counters to zero
  * \details   Node Configuration Reset Bundle Counters command
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] void No arguments accepted
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_ResetBundleCounters(void);

/**
  * \brief     Set all error counters to zero
  * \details   Node Configuration Reset Error Counters command
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] void No arguments accepted
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_ResetErrorCounters(void);

/**
  * \brief     Adds new application configurations from ADU Proxy and Channel Config Tables
  * \details   Node Configuration Add Application command
  * \note      Calls BPA_ADUP_AddApplication() from fwp_adup.h
  * \param[in] Payload BPLib_AddApplicationCmd_Payload_t type found in bplib_nc_payloads.h
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  * \retval    BPLIB_ADU_ADD_CHAN_ERR: Payload.ChanId is out of bounds
  * \retval    BPLIB_ADU_ADD_STAT_ERR: State of desired app is STARTED
  */
BPLib_Status_t BPLib_NC_AddApplication(const BPLib_AddApplicationCmd_Payload_t Payload);

/**
  * \brief     Terminate given application’s connection, close its ADU channel, and empty its queues
  * \details   Node Configuration Remove Application command
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] Payload BPLib_RemoveApplicationCmd_Payload_t type found in bplib_nc_payloads.h
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_RemoveApplication(const BPLib_RemoveApplicationCmd_Payload_t Payload);

/**
  * \brief     Set given application's channel state to specified state
  * \details   Node Configuration Set Registration State command
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] Payload BPLib_SetRegistrationStateCmd_Payload_t type found in bplib_nc_payloads.h
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_SetRegistrationState(const BPLib_SetRegistrationStateCmd_Payload_t Payload);

/**
  * \brief     Verify given application’s channel configuration and begin moving bundles bidirectionally between host and PI
  * \details   Node Configuration Start Application command
  * \note      Calls BPA_ADUP_StartApplication() from fwp_adup.h
  * \param[in] Payload BPLib_StartApplicationCmd_Payload_t type found in bplib_nc_payloads.h
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  * \retval    BPLIB_ADU_START_CHAN_ERR: Payload.ChanId is out of bounds
  * \retval    BPLIB_ADU_START_STAT_ERR: State of desired application is not ADDED
  * \retval    BPLIB_ADU_START_SUB_ERR: Error in platform-dependent software bus subscription
  */
BPLib_Status_t BPLib_NC_StartApplication(const BPLib_StartApplicationCmd_Payload_t Payload);

/**
  * \brief     Stop moving bundles bidirectionally between host and PI for the given application
  * \details   Node Configuration Stop Application command
  * \note      Calls BPA_ADUP_StopApplication() from fwp_adup.h
  * \param[in] Payload BPLib_StopApplicationCmd_Payload_t type found in bplib_nc_payloads.h
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  * \retval    BPLIB_ADU_STOP_CHAN_ERR: Payload.ChanId is out of bounds
  * \retval    BPLIB_ADU_STOP_STAT_ERR: State of desired application is not STARTED
  * \retval    BPLIB_ADU_STOP_UNSUB_ERR: Error in platform-dependent software bus un-subscription
  */
BPLib_Status_t BPLib_NC_StopApplication(const BPLib_StopApplicationCmd_Payload_t Payload);

/**
  * \brief     Reject if adding pattern would exceed maximum size of authorized sources, else add EID pattern to set of authorized source EIDs
  * \details   Node Configuration Add Authorized Sources command
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] Payload BPLib_AddAuthSourcesCmd_Payload_t type found in bplib_nc_payloads.h
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_AddAuthSources(const BPLib_AddAuthSourcesCmd_Payload_t Payload);

/**
  * \brief     Remove EID pattern from set of authorized source EIDs
  * \details   Node Configuration Remove Authorized Sources command
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] Payload BPLib_RemoveAuthSourcesCmd_Payload_t type found in bplib_nc_payloads.h
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_RemoveAuthSources(const BPLib_RemoveAuthSourcesCmd_Payload_t Payload);

/**
  * \brief     Add EID pattern to set of authorized custody source EIDs
  * \details   Node Configuration Add Authorized Custody Source command
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] Payload BPLib_AddAuthCustodySourcesCmd_Payload_t type found in bplib_nc_payloads.h
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_AddAuthCustodySources(const BPLib_AddAuthCustodySourcesCmd_Payload_t Payload);

/**
  * \brief     Remove EID pattern from set of authorized custody source EIDs
  * \details   Node Configuration Remove Authorized Custody Source command
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] Payload BPLib_RemoveAuthCustodySourcesCmd_Payload_t type found in bplib_nc_payloads.h
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_RemoveAuthCustodySources(const BPLib_RemoveAuthCustodySourcesCmd_Payload_t Payload);

/**
  * \brief     Add EID pattern to set of authorized custodian EIDs
  * \details   Node Configuration Add Authorized Custodians command
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] Payload BPLib_AddAuthCustodiansCmd_Payload_t type found in bplib_nc_payloads.h
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_AddAuthCustodians(const BPLib_AddAuthCustodiansCmd_Payload_t Payload);

/**
  * \brief     Remove EID pattern from set of authorized custodian EIDs
  * \details   Node Configuration Remove Authorized Custodians command
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] Payload BPLib_RemoveAuthCustodiansCmd_Payload_t type found in bplib_nc_payloads.h
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_RemoveAuthCustodians(const BPLib_RemoveAuthCustodiansCmd_Payload_t Payload);

/**
  * \brief     Add EID pattern to set of authorized report-to-EIDs
  * \details   Node Configuration Add Authorized Report-to-EIDs command
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] Payload BPLib_AddAuthReportToEidCmd_Payload_t type found in bplib_nc_payloads.h
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_AddAuthReportToEid(const BPLib_AddAuthReportToEidCmd_Payload_t Payload);

/**
  * \brief     Remove EID pattern from set of authorized report-to-EIDs
  * \details   Node Configuration Remove Authorized Report-to-EIDs command
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] Payload BPLib_RemoveAuthReportToEidCmd_Payload_t type found in bplib_nc_payloads.h
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_RemoveAuthReportToEid(const BPLib_RemoveAuthReportToEidCmd_Payload_t Payload);

/**
  * \brief     Add source EID pattern and latency info to set
  * \details   Node Configuration Add Latency Info command
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] Payload BPLib_AddLatencyCmd_Payload_t type found in bplib_nc_payloads.h
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_AddLatency(const BPLib_AddLatencyCmd_Payload_t Payload);

/**
  * \brief     Remove source EID pattern and latency info from set
  * \details   Node Configuration Remove Latency Info command
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] Payload BPLib_RemoveLatencyCmd_Payload_t type found in bplib_nc_payloads.h
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_RemoveLatency(const BPLib_RemoveLatencyCmd_Payload_t Payload);

/**
  * \brief     Establish connection, create output queue, and configure CLA for bundle exchange
  * \details   Node Configuration Contact Setup command
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] Payload BPLib_ContactSetupCmd_Payload_t type found in bplib_nc_payloads.h
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_ContactSetup(const BPLib_ContactSetupCmd_Payload_t Payload);

/**
  * \brief     Start transferring bundles between underlying network and BI
  * \details   Node Configuration Contact Start command
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] Payload BPLib_ContactStartCmd_Payload_t type found in bplib_nc_payloads.h
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_ContactStart(const BPLib_ContactStartCmd_Payload_t Payload);

/**
  * \brief     Stop transferring bundles, stop requesting BI for output queue bundles, and notify BI of incomplete actions
  * \details   Node Configuration Contact Stop command
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] Payload BPLib_ContactStopCmd_Payload_t type found in bplib_nc_payloads.h
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_ContactStop(const BPLib_ContactStopCmd_Payload_t Payload);

/**
  * \brief     Disestablish CLA, free all CLA resources, discard output queue, and delete custody timers
  * \details   Node Configuration Contact Teardown command
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] Payload BPLib_ContactTeardownCmd_Payload_t type found in bplib_nc_payloads.h
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_ContactTeardown(const BPLib_ContactTeardownCmd_Payload_t Payload);

/**
  * \brief     Add given EID pattern as key to map of MIB counters accessed by source EID
  * \details   Node Configuration Add MIB Array Key command.
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \note      Max number of entries is mission-defined (MAX_MIB_ARRAY_KEYS)
  * \param[in] Payload BPLib_AddMibArrayKeyCmd_Payload_t type found in bplib_nc_payloads.h
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_AddMibArrayKey(const BPLib_AddMibArrayKeyCmd_Payload_t Payload);

/**
  * \brief     Remove elements indexed by given EID pattern from the map of MIB counters accessed by source EID
  * \details   Node Configuration Remove MIB Array Key command.
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] Payload BPLib_RemoveMibArrayKeyCmd_Payload_t type found in bplib_nc_payloads.h
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_RemoveMibArrayKey(const BPLib_RemoveMibArrayKeyCmd_Payload_t Payload);

/**
  * \brief     Set given MIB configuration item to given value
  * \details   Node Configuration Set MIB Item command.
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] Payload BPLib_SetMibItemCmd_Payload_t type found in bplib_nc_payloads.h
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_SetMibItem(const BPLib_SetMibItemCmd_Payload_t Payload);

/**
  * \brief     Add storage partition of given size for storing bundles whose source EID matches given pattern
  * \details   Node Configuration Add Storage Allocation command.
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] Payload BPLib_AddStorageAllocationCmd_Payload_t type found in bplib_nc_payloads.h
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_AddStorageAllocation(const BPLib_AddStorageAllocationCmd_Payload_t Payload);

/**
  * \brief     Remove storage partition corresponding to given EID pattern
  * \details   Node Configuration Remove Storage Allocation command.
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] Payload BPLib_RemoveStorageAllocationCmd_Payload_t type found in bplib_nc_payloads.h
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_RemoveStorageAllocation(const BPLib_RemoveStorageAllocationCmd_Payload_t Payload);

/**
  * \brief     Perform TBD tests and return pass/fail
  * \details   Node Configuration Perform Self-Test command.
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] void No arguments accepted
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_PerformSelfTest(void);

/**
  * \brief     Send Per Node MIB Configuration telemetry packet
  * \details   Node Configuration Send Node MIB Configuration Housekeeping Packet command.
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] void No arguments accepted
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_SendNodeMibConfigHk(void);

/**
  * \brief     Send Per Source MIB Configuration telemetry packet
  * \details   Node Configuration Send Source MIB Configuration Housekeeping Packet command.
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] void No arguments accepted
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_SendSourceMibConfigHk(void);

/**
  * \brief     Send Per Node MIB Counter telemetry packet
  * \details   Node Configuration Send Node MIB Counters Housekeeping Packet command.
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] void No arguments accepted
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_SendNodeMibCountersHk(void);

/**
  * \brief     Send Per Source MIB Counter telemetry packet
  * \details   Node Configuration Send Source MIB Counters Housekeeping Packet command.
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] void No arguments accepted
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_SendSourceMibCountersHk(void);

/**
  * \brief     Send Storage housekeeping telemetry packet
  * \details   Node Configuration Send Storage Housekeeping Packet command.
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \param[in] void No arguments accepted
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_SendStorageHk(void);

/**
  * \brief     Send Storage housekeeping telemetry packet
  * \details   Node Configuration Send Storage Housekeeping Packet command.
  * \note      This command is currently unimplemented and only returns BLPIB_SUCCESS
  * \note      Command is primarily handled by BPA_DP_SendChannelContactStatHk() found in fwp_dp.h
  * \param[in] void No arguments accepted
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_SendChannelContactStatHk(void);

#endif /* BPLIB_NC_H */
