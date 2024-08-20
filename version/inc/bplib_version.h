/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/**
 * @file
 *  The Sample Lib header file containing version information
 */

#ifndef BPLIB_VERSION_H
#define BPLIB_VERSION_H

/* Development Build Macro Definitions */

#define BPLIB_BUILD_NUMBER     1 /*!< Development Build: Number of development builds (not commits) since baseline */
#define BPLIB_BUILD_BASELINE   "equuleus-rc1" /*!< Development Build: git tag that is the base for the current development */
#define BPLIB_BUILD_DEV_CYCLE  "equuleus-rc2" /**< @brief Development: Release name for current development cycle */
#define BPLIB_BUILD_CODENAME   "Equuleus" /**< @brief: Development: Code name for the current build */

/*
 * Version Macros, see \ref cfsversions for definitions.
 */
#define BPLIB_MAJOR_VERSION 0  /*!< @brief Major version number */
#define BPLIB_MINOR_VERSION 0  /*!< @brief Minor version number */
#define BPLIB_REVISION      0  /*!< @brief Revision version number. Value of 0 indicates a development version.*/

/*!
 * @brief Mission revision.
 *
 * Set to 0 on OFFICIAL releases, and set to 255 (0xFF) on development versions.
 * Values 1-254 are reserved for mission use to denote patches/customizations as needed.
 */

/*!
 * @brief Mission revision.
 *
 * Reserved for mission use to denote patches/customizations as needed.
 * Values 1-254 are reserved for mission use to denote patches/customizations as needed. NOTE: Reserving 0 and 0xFF for
 * cFS open-source development use (pending resolution of nasa/cFS#440)
 */
#define BPLIB_MISSION_REV 0xFF

#define BPLIB_STR_HELPER(x) #x /*!< @brief Helper function to concatenate strings from integer macros */
#define BPLIB_STR(x) \
    BPLIB_STR_HELPER(x) /*!< @brief Helper function to concatenate strings from integer macros */

/*! @brief Development Build Version Number.
 * @details Baseline git tag + Number of commits since baseline. @n
 * See @ref cfsversions for format differences between development and release versions.
 */
#define BPLIB_VERSION BPLIB_BUILD_BASELINE "+dev" BPLIB_STR(BPLIB_BUILD_NUMBER)

/**
 * @brief Max Version String length.
 * 
 * Maximum length that a sample_lib version string can be.
 * 
 */
#define BPLIB_CFG_MAX_VERSION_STR_LEN 256


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
