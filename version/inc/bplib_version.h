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

/**
 * @file
 *  The BPLib header file containing version information
 */

#ifndef BPLIB_VERSION_H
#define BPLIB_VERSION_H

/*
 * Version Macros, see \ref cfsversions for definitions.
 */
#define BPLIB_MAJOR_VERSION (0u)  /*!< @brief Major version number */
#define BPLIB_MINOR_VERSION (0u)  /*!< @brief Minor version number */
#define BPLIB_REVISION      (0u)  /*!< @brief Revision version number. Value of 0 indicates a development version.*/

/*
 * Development Build Macro Definitions
 */
#define BPLIB_BUILD_NUMBER (11u) /*!< Development Build: Number of development builds (not commits) since baseline */

/**
 * @brief Max Version String length.
 * 
 * Maximum length that a sample_lib version string can be.
 * 
 */
#define BPLIB_CFG_MAX_VERSION_STR_LEN (256)


/*************************************************************************
** Function Declarations
*************************************************************************/

/**
 * Library routine to print the version
 */
int BPLib_PrintVersion(void);


#endif /* BPLIB_VERSION_H */

/************************/
/*  End of File Comment */
/************************/
