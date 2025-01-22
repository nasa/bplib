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

/* ======== */
/* Includes */
/* ======== */

#include "bplib_eid_test_utils.h"

/* ==================== */
/* Function Definitions */
/* ==================== */

void Test_BPLib_EID_IsValid_Nominal(void)
{
    bool IsValid;

    // EID = 1, 1.2.*
    EID_A.Scheme    = 1;
    EID_A.Authority = 1;
    EID_A.Node      = 2;
    EID_A.Service   = BPLIB_EID_WILDCARD;

    IsValid = BPLib_EID_IsValid(EID_A);

    UtAssert_BOOL_TRUE(IsValid);
}

void Test_BPLib_EID_IsValid_InvalidScheme_Error(void)
{
    bool IsValid;

    // EID = 3, *.2.3
    EID_A.Scheme    = 3;
    EID_A.Authority = BPLIB_EID_WILDCARD;
    EID_A.Node      = 2;
    EID_A.Service   = 3;

    IsValid = BPLib_EID_IsValid(EID_A);

    UtAssert_BOOL_FALSE(IsValid);
}

void Test_BPLib_EID_IsMatch_Nominal(void)
{
    bool IsMatch;

    // Actual EID = 1, 2.3.4
    EID_A.Scheme    = 1;
    EID_A.Authority = 2;
    EID_A.Node      = 3;
    EID_A.Service   = 4;

    // Reference EID = 1, 2.3.4
    EID_B.Scheme    = 1;
    EID_B.Authority = 2;
    EID_B.Node      = 3;
    EID_B.Service   = 4;

    IsMatch = BPLib_EID_IsMatch(EID_A, EID_B);

    UtAssert_BOOL_TRUE(IsMatch);
}

void Test_BPLib_EID_IsMatch_AuthorityWildcard_Nominal(void)
{
    bool IsMatch;

    // Actual EID = 1, 5.6.7
    EID_A.Scheme    = 1;
    EID_A.Authority = 5;
    EID_A.Node      = 6;
    EID_A.Service   = 7;

    // Reference EID = 1, *.6.7
    EID_B.Scheme    = 1;
    EID_B.Authority = BPLIB_EID_WILDCARD;
    EID_B.Node      = 6;
    EID_B.Service   = 7;

    IsMatch = BPLib_EID_IsMatch(EID_A, EID_B);

    UtAssert_BOOL_TRUE(IsMatch);
}

void Test_BPLib_EID_IsMatch_NodeWildcard_Nominal(void)
{
    bool IsMatch;

    // Actual EID = 1, 8.9.10
    EID_A.Scheme    = 1;
    EID_A.Authority = 8;
    EID_A.Node      = 9;
    EID_A.Service   = 10;

    // Reference EID = 1, 8.*.10
    EID_B.Scheme    = 1;
    EID_B.Authority = 8;
    EID_B.Node      = BPLIB_EID_WILDCARD;
    EID_B.Service   = 10;

    IsMatch = BPLib_EID_IsMatch(EID_A, EID_B);

    UtAssert_BOOL_TRUE(IsMatch);
}

void Test_BPLib_EID_IsMatch_ServiceWildcard_Nominal(void)
{
    bool IsMatch;

    // Actual EID = 1, 11.12.13
    EID_A.Scheme    = 1;
    EID_A.Authority = 11;
    EID_A.Node      = 12;
    EID_A.Service   = 13;

    // Reference EID = 1, 11.12.*
    EID_B.Scheme    = 1;
    EID_B.Authority = 11;
    EID_B.Node      = 12;
    EID_B.Service   = BPLIB_EID_WILDCARD;

    IsMatch = BPLib_EID_IsMatch(EID_A, EID_B);

    UtAssert_BOOL_TRUE(IsMatch);
}

void Test_BPLib_EID_IsMatch_SchemeMismatch_Error(void)
{
    bool IsMatch;

    // Actual EID = 2, 14.15.16
    EID_A.Scheme    = 2;
    EID_A.Authority = 14;
    EID_A.Node      = 15;
    EID_A.Service   = 16;

    // Reference EID = 1, 14.15.16
    EID_B.Scheme    = 1;
    EID_B.Authority = 14;
    EID_B.Node      = 15;
    EID_B.Service   = 16;

    IsMatch = BPLib_EID_IsMatch(EID_A, EID_B);

    UtAssert_BOOL_FALSE(IsMatch);
}

void Test_BPLib_EID_IsMatch_AuthorityMismatch_Error(void)
{
    bool IsMatch;

    // Actual EID = 1, 18.18.19
    EID_A.Scheme    = 1;
    EID_A.Authority = 18;
    EID_A.Node      = 18;
    EID_A.Service   = 19;

    // Reference EID = 1, 17.18.19
    EID_B.Scheme    = 1;
    EID_B.Authority = 17;
    EID_B.Node      = 18;
    EID_B.Service   = 19;

    IsMatch = BPLib_EID_IsMatch(EID_A, EID_B);

    UtAssert_BOOL_FALSE(IsMatch);
}

void Test_BPLib_EID_IsMatch_NodeMismatch_Error(void)
{
    bool IsMatch;

    // Actual EID = 1, 20.22.22
    EID_A.Scheme    = 1;
    EID_A.Authority = 20;
    EID_A.Node      = 22;
    EID_A.Service   = 22;

    // Reference EID = 1, 20.21.22
    EID_B.Scheme    = 1;
    EID_B.Authority = 20;
    EID_B.Node      = 21;
    EID_B.Service   = 22;

    IsMatch = BPLib_EID_IsMatch(EID_A, EID_B);

    UtAssert_BOOL_FALSE(IsMatch);
}

void Test_BPLib_EID_IsMatch_ServiceMismatch_Error(void)
{
    bool IsMatch;

    // Actual EID = 1, 23.24.26
    EID_A.Scheme    = 1;
    EID_A.Authority = 23;
    EID_A.Node      = 24;
    EID_A.Service   = 26;

    // Reference EID = 1, 23.24.25
    EID_B.Scheme    = 1;
    EID_B.Authority = 23;
    EID_B.Node      = 24;
    EID_B.Service   = 25;

    IsMatch = BPLib_EID_IsMatch(EID_A, EID_B);

    UtAssert_BOOL_FALSE(IsMatch);
}

void TestBplibEid_Register(void)
{
    ADD_TEST(Test_BPLib_EID_IsValid_Nominal);
    ADD_TEST(Test_BPLib_EID_IsValid_InvalidScheme_Error);
    ADD_TEST(Test_BPLib_EID_IsMatch_Nominal);
    ADD_TEST(Test_BPLib_EID_IsMatch_AuthorityWildcard_Nominal);
    ADD_TEST(Test_BPLib_EID_IsMatch_NodeWildcard_Nominal);
    ADD_TEST(Test_BPLib_EID_IsMatch_ServiceWildcard_Nominal);
    ADD_TEST(Test_BPLib_EID_IsMatch_SchemeMismatch_Error);
    ADD_TEST(Test_BPLib_EID_IsMatch_AuthorityMismatch_Error);
    ADD_TEST(Test_BPLib_EID_IsMatch_NodeMismatch_Error);
    ADD_TEST(Test_BPLib_EID_IsMatch_ServiceMismatch_Error);
}