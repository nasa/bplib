/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking 
 * (DTN) Bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this 
 * file except in compliance with the License. You may obtain a copy of the License at 
 *
 * http://www.apache.org/licenses/LICENSE-2.0 
 *
 * Unless required by applicable law or agreed to in writing, software distributed under 
 * the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF 
 * ANY KIND, either express or implied. See the License for the specific language 
 * governing permissions and limitations under the License. The copyright notice to be 
 * included in the software is as follows: 
 *
 * Copyright 2025 United States Government as represented by the Administrator of the 
 * National Aeronautics and Space Administration. All Rights Reserved.
 *
 */

/**
 * @file
 *   BPLib Version Utility
 */

/*************************************************************************
** Includes
*************************************************************************/
#include "bplib_api_types.h"
#include "bplib_version.h"
#include "osapi.h" /* for OS_Printf() */
#include <stdio.h> /* for "snprintf()" */

/*************************************************************************
** Private Data Structures
*************************************************************************/



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Library Initialization Routine                                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int BPLib_PrintVersion(void)
{
    char VersionString[BPLIB_CFG_MAX_VERSION_STR_LEN];

    (void) snprintf(VersionString, BPLIB_CFG_MAX_VERSION_STR_LEN, "v%u.%u.%u-sprint-%u",
        BPLIB_MAJOR_VERSION,
        BPLIB_MINOR_VERSION,
        BPLIB_REVISION,
        BPLIB_BUILD_NUMBER);

    OS_printf("BPLib Version: %s\n", VersionString);

    return BPLIB_SUCCESS;
}
