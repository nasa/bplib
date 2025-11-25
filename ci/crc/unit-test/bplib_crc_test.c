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
 * \file
 *  Unit tests for bplib_crc.c
 */

/*
** Include Files
*/

#include "bplib_crc_test_utils.h"


/*
** Global Data
*/

/* Example primary block */
uint8_t PrimaryBlockData[] = {
    0x89, 0x07, 0x04, 0x01, 0x82, 0x02, 0x82, 
    0x18, 0xc8, 0x01, 0x82, 0x02, 0x82, 0x18, 0x64, 
    0x01, 0x82, 0x02, 0x82, 0x18, 0x64, 0x01, 0x82, 
    0x1b, 0x00, 0x00, 0x00, 0xaf, 0xe9, 0x53, 0x7a, 
    0x38, 0x00, 0x1a, 0x00, 0x36, 0xee, 0x80, 0x42, 
    0x00, 0x00
};

/* Expected CRC values verified independently with https://www.crccalc.com/ */
BPLib_CRC_Val_t ExpectedCrc16 = 0x0b19;
BPLib_CRC_Val_t ExpectedCrc32 = 0xF636C45D;


/*
** Function Definitions
*/

/* Test that the CRC-16 calculation works as expected */
void Test_BPLib_CRC_16(void)
{
    /* Initialize CRC tables */
    BPLib_CRC_Init();

    UtAssert_EQ(BPLib_CRC_Val_t, 
        BPLib_CRC_Calculate(PrimaryBlockData, sizeof(PrimaryBlockData), BPLib_CRC_Type_CRC16),
        ExpectedCrc16);
}


/* Test that the CRC-32 calculation works as expected */
void Test_BPLib_CRC_32(void)
{
    /* Initialize CRC tables */
    BPLib_CRC_Init();

    UtAssert_EQ(BPLib_CRC_Val_t, 
        BPLib_CRC_Calculate(PrimaryBlockData, sizeof(PrimaryBlockData), BPLib_CRC_Type_CRC32C),
        ExpectedCrc32);
}

/* Test that the CRC-NONE calculation works as expected */
void Test_BPLib_CRC_NONE(void)
{
    /* Initialize CRC tables */
    BPLib_CRC_Init();

    UtAssert_EQ(BPLib_CRC_Val_t, 
        BPLib_CRC_Calculate(PrimaryBlockData, sizeof(PrimaryBlockData), BPLib_CRC_Type_None),
        0);
}

void UtTest_Setup(void)
{
    ADD_TEST(Test_BPLib_CRC_16);
    ADD_TEST(Test_BPLib_CRC_32);
    ADD_TEST(Test_BPLib_CRC_NONE);
}
