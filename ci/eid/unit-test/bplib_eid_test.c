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

void Test_BPLib_EID_IsValid_DTN_Nominal(void)
{
    EID_Test.Scheme  = BPLIB_EID_SCHEME_DTN;
    EID_Test.Node    = 0;
    EID_Test.Service = 0;

    UtAssert_BOOL_TRUE(BPLib_EID_IsValid(EID_Test));
}

void Test_BPLib_EID_IsValid_IPN_Nominal(void)
{
    EID_Test.Scheme    = BPLIB_EID_SCHEME_IPN;
    EID_Test.IpnFormat = 3;
    EID_Test.Authority = 1;
    EID_Test.Node      = BPLIB_EID_WILDCARD;
    EID_Test.Service   = 2;

    UtAssert_BOOL_TRUE(BPLib_EID_IsValid(EID_Test));
}

void Test_BPLib_EID_IsValid_InvalidScheme_Error(void)
{
    EID_Test.Scheme = 3;
    UtAssert_BOOL_FALSE(BPLib_EID_IsValid(EID_Test));
}

void Test_BPLib_EID_IsValid_InvalidDTNNode_Error(void)
{
    EID_Test.Scheme = BPLIB_EID_SCHEME_DTN;
    EID_Test.Node   = 1;

    UtAssert_BOOL_FALSE(BPLib_EID_IsValid(EID_Test));
}

void Test_BPLib_EID_IsValid_InvalidDTNService_Error(void)
{
    EID_Test.Scheme  = BPLIB_EID_SCHEME_DTN;
    EID_Test.Node    = 0;
    EID_Test.Service = 1;

    UtAssert_BOOL_FALSE(BPLib_EID_IsValid(EID_Test));
}

void Test_BPLib_EID_IsValid_InvalidIPN_Error(void)
{
    EID_Test.Scheme    = BPLIB_EID_SCHEME_IPN;
    EID_Test.IpnFormat = 2;
    EID_Test.Authority = 1;

    UtAssert_BOOL_FALSE(BPLib_EID_IsValid(EID_Test));
}

void Test_BPLib_EID_IsMatch_Nominal(void)
{
    EID_Test.Scheme    = BPLIB_EID_SCHEME_DTN;
    EID_Test.IpnFormat = 3;
    EID_Test.Authority = 10;
    EID_Test.Node      = 11;
    EID_Test.Service   = 12;

    EID_Pattern.Scheme       = BPLIB_EID_SCHEME_DTN;
    EID_Pattern.IpnFormat    = 3;
    EID_Pattern.MaxAuthority = 20;
    EID_Pattern.MinAuthority = 0;
    EID_Pattern.MaxNode      = 30;
    EID_Pattern.MinNode      = 5;
    EID_Pattern.MaxService   = 40;
    EID_Pattern.MinService   = 10;

    UtAssert_BOOL_TRUE(BPLib_EID_IsMatch(EID_Test, EID_Pattern));
}

void Test_BPLib_EID_IsMatch_AuthorityWildcard_Nominal(void)
{
    EID_Test.Scheme    = BPLIB_EID_SCHEME_DTN;
    EID_Test.IpnFormat = 3;
    EID_Test.Authority = BPLIB_EID_WILDCARD;
    EID_Test.Node      = 6;
    EID_Test.Service   = 7;

    EID_Pattern.Scheme       = BPLIB_EID_SCHEME_DTN;
    EID_Pattern.IpnFormat    = 3;
    EID_Pattern.MaxAuthority = 100;
    EID_Pattern.MinAuthority = 0;
    EID_Pattern.MaxNode      = 100;
    EID_Pattern.MinNode      = 0;
    EID_Pattern.MaxService   = 100;
    EID_Pattern.MinService   = 0;

    UtAssert_BOOL_TRUE(BPLib_EID_IsMatch(EID_Test, EID_Pattern));
}

void Test_BPLib_EID_IsMatch_NodeWildcard_Nominal(void)
{
    EID_Test.Scheme    = BPLIB_EID_SCHEME_DTN;
    EID_Test.IpnFormat = 2;
    EID_Test.Node      = BPLIB_EID_WILDCARD;
    EID_Test.Service   = 10;

    EID_Pattern.Scheme     = BPLIB_EID_SCHEME_DTN;
    EID_Pattern.IpnFormat  = 2;
    EID_Pattern.MaxNode    = 100;
    EID_Pattern.MinNode    = 90;
    EID_Pattern.MaxService = 100;
    EID_Pattern.MinService = 0;

    UtAssert_BOOL_TRUE(BPLib_EID_IsMatch(EID_Test, EID_Pattern));
}

void Test_BPLib_EID_IsMatch_ServiceWildcard_Nominal(void)
{
    EID_Test.Scheme    = BPLIB_EID_SCHEME_IPN;
    EID_Test.IpnFormat = 2;
    EID_Test.Node      = 12;
    EID_Test.Service   = BPLIB_EID_WILDCARD;

    EID_Pattern.Scheme     = BPLIB_EID_SCHEME_IPN;
    EID_Pattern.IpnFormat  = 2;
    EID_Pattern.MaxNode    = 100;
    EID_Pattern.MinNode    = 0;
    EID_Pattern.MaxService = 1;
    EID_Pattern.MinService = 0;

    UtAssert_BOOL_TRUE(BPLib_EID_IsMatch(EID_Test, EID_Pattern));
}

void Test_BPLib_EID_IsMatch_SchemeMismatch_Error(void)
{
    EID_Test.Scheme    = BPLIB_EID_SCHEME_IPN;
    EID_Pattern.Scheme = BPLIB_EID_SCHEME_DTN;

    UtAssert_BOOL_FALSE(BPLib_EID_IsMatch(EID_Test, EID_Pattern));
}

void Test_BPLib_EID_IsMatch_IpnMismatch_Error(void)
{
    EID_Test.Scheme    = BPLIB_EID_SCHEME_IPN;
    EID_Test.IpnFormat = 2;

    EID_Pattern.Scheme    = BPLIB_EID_SCHEME_IPN;
    EID_Pattern.IpnFormat = 3;

    UtAssert_BOOL_FALSE(BPLib_EID_IsMatch(EID_Test, EID_Pattern));
}

void Test_BPLib_EID_IsMatch_AuthorityMismatch_Error(void)
{
    EID_Test.Scheme    = BPLIB_EID_SCHEME_DTN;
    EID_Test.IpnFormat = 3;

    EID_Pattern.Scheme    = BPLIB_EID_SCHEME_DTN;
    EID_Pattern.IpnFormat = 3;

    /* === Authority > Max Authority === */
    EID_Test.Authority = 17;

    EID_Pattern.MaxAuthority = 2;
    EID_Pattern.MinAuthority = 1;

    UtAssert_BOOL_FALSE(BPLib_EID_IsMatch(EID_Test, EID_Pattern));

    /* === Authority < Min Authority === */
    EID_Test.Authority = 1;

    EID_Pattern.MaxAuthority = 10;
    EID_Pattern.MinAuthority = 9;

    UtAssert_BOOL_FALSE(BPLib_EID_IsMatch(EID_Test, EID_Pattern));
}

void Test_BPLib_EID_IsMatch_NodeMismatch_Error(void)
{
    EID_Test.Scheme    = BPLIB_EID_SCHEME_DTN;
    EID_Test.IpnFormat = 2;

    EID_Pattern.Scheme    = BPLIB_EID_SCHEME_DTN;
    EID_Pattern.IpnFormat = 2;

    /* === Node > Max Node === */
    EID_Test.Node = 22;
    
    EID_Pattern.MaxNode    = 20;
    EID_Pattern.MinService = 0;

    UtAssert_BOOL_FALSE(BPLib_EID_IsMatch(EID_Test, EID_Pattern));

    /* === Node < Min Node === */
    EID_Test.Node = 0;
    
    EID_Pattern.MaxNode    = 20;
    EID_Pattern.MinService = 5;

    UtAssert_BOOL_FALSE(BPLib_EID_IsMatch(EID_Test, EID_Pattern));
}

void Test_BPLib_EID_IsMatch_ServiceMismatch_Error(void)
{
    EID_Test.Scheme    = BPLIB_EID_SCHEME_DTN;
    EID_Test.IpnFormat = 2;
    EID_Test.Node      = 24;

    EID_Pattern.Scheme    = BPLIB_EID_SCHEME_DTN;
    EID_Pattern.IpnFormat = 2;
    EID_Pattern.MaxNode   = 30;
    EID_Pattern.MinNode   = 0;
    
    /* === Service > Max Service === */
    EID_Test.Service = 20;

    EID_Pattern.MaxService = 10;
    EID_Pattern.MinService = 0;

    UtAssert_BOOL_FALSE(BPLib_EID_IsMatch(EID_Test, EID_Pattern));

    /* === Service < Min Service === */
    EID_Test.Service = 0;

    EID_Pattern.MaxService = 10;
    EID_Pattern.MinService = 5;

    UtAssert_BOOL_FALSE(BPLib_EID_IsMatch(EID_Test, EID_Pattern));
}

void Test_BPLib_EID_IsMatch_InvalidRanges_Error(void)
{
    /* === Invalid Authority pattern range === */
    EID_Pattern.MaxAuthority = 0;
    EID_Pattern.MinAuthority = 100;

    UtAssert_BOOL_FALSE(BPLib_EID_IsMatch(EID_Test, EID_Pattern));

    /* === Invalid Node pattern range === */
    EID_Pattern.MaxNode = 0;
    EID_Pattern.MinNode = 100;

    UtAssert_BOOL_FALSE(BPLib_EID_IsMatch(EID_Test, EID_Pattern));

    /* === Invalid Service pattern range === */
    EID_Pattern.MaxService = 0;
    EID_Pattern.MinService = 100;

    UtAssert_BOOL_FALSE(BPLib_EID_IsMatch(EID_Test, EID_Pattern));
}

void TestBplibEid_Register(void)
{
    ADD_TEST(Test_BPLib_EID_IsValid_DTN_Nominal);
    ADD_TEST(Test_BPLib_EID_IsValid_IPN_Nominal);
    ADD_TEST(Test_BPLib_EID_IsValid_InvalidScheme_Error);
    ADD_TEST(Test_BPLib_EID_IsValid_InvalidDTNNode_Error);
    ADD_TEST(Test_BPLib_EID_IsValid_InvalidDTNService_Error);
    ADD_TEST(Test_BPLib_EID_IsValid_InvalidIPN_Error);
    ADD_TEST(Test_BPLib_EID_IsMatch_Nominal);
    ADD_TEST(Test_BPLib_EID_IsMatch_AuthorityWildcard_Nominal);
    ADD_TEST(Test_BPLib_EID_IsMatch_NodeWildcard_Nominal);
    ADD_TEST(Test_BPLib_EID_IsMatch_ServiceWildcard_Nominal);
    ADD_TEST(Test_BPLib_EID_IsMatch_SchemeMismatch_Error);
    ADD_TEST(Test_BPLib_EID_IsMatch_IpnMismatch_Error);
    ADD_TEST(Test_BPLib_EID_IsMatch_AuthorityMismatch_Error);
    ADD_TEST(Test_BPLib_EID_IsMatch_NodeMismatch_Error);
    ADD_TEST(Test_BPLib_EID_IsMatch_ServiceMismatch_Error);
    ADD_TEST(Test_BPLib_EID_IsMatch_InvalidRanges_Error);
}