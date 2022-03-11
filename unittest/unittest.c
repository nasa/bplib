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

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "ut_assert.h"
#include "bplib_store_flash.h"

/******************************************************************************
 EXTERNS
 ******************************************************************************/

extern int ut_crc(void);
extern int ut_rb_tree(void);
extern int ut_rh_hash(void);
extern int ut_flash(void);

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * CRC Unit Test -
 *--------------------------------------------------------------------------------------*/
int bplib_unittest_crc(void)
{
#ifdef UNITTESTS
    return ut_crc();
#else
    return 0;
#endif
}

/*--------------------------------------------------------------------------------------
 * RB Tree Unit Test -
 *--------------------------------------------------------------------------------------*/
int bplib_unittest_rb_tree(void)
{
#ifdef UNITTESTS
    return ut_rb_tree();
#else
    return 0;
#endif
}

/*--------------------------------------------------------------------------------------
 * RH Hash Unit Test -
 *--------------------------------------------------------------------------------------*/
int bplib_unittest_rh_hash(void)
{
#ifdef UNITTESTS
    return ut_rh_hash();
#else
    return 0;
#endif
}

/*--------------------------------------------------------------------------------------
 * Flash Unit Test -
 *--------------------------------------------------------------------------------------*/
int bplib_unittest_flash(void)
{
#ifdef UNITTESTS
    bplib_store_flash_uninit(); /* should always be safe to call */
    return ut_flash();
#else
    return 0;
#endif
}
