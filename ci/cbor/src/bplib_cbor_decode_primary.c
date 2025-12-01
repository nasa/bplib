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

#include "bplib_cbor_internal.h"
#include <stdio.h>

/*******************************************************************************
* RFC-9171 Primary Block Parsing Definition
*/
struct _PrimaryBlockParser
{
    QCBOR_UInt64Parser VersionParser;
    QCBOR_UInt64Parser FlagsParser;
    QCBOR_UInt64Parser CRCTypeParser;
    QCBOR_EIDParser SrcEIDParser;
    QCBOR_EIDParser DestEIDParser;
    QCBOR_EIDParser ReportEIDParser;
    QCBOR_TimestampParser CreationTimestampParser;
    QCBOR_UInt64Parser LifetimeParser;
    QCBOR_CRCParser CRCParser;
};

/* Parser Initialization (only needs to be created once) */
static struct _PrimaryBlockParser PrimaryBlockParser = {
    .VersionParser = BPLib_QCBOR_UInt64ParserImpl,
    .FlagsParser = BPLib_QCBOR_UInt64ParserImpl,
    .CRCTypeParser = BPLib_QCBOR_UInt64ParserImpl,
    .SrcEIDParser = BPLib_QCBOR_EidDtnNoneParserImpl,
    .DestEIDParser = BPLib_QCBOR_EidDtnNoneParserImpl,
    .ReportEIDParser = BPLib_QCBOR_ReportToEidParserImpl,
    .CreationTimestampParser = BPLib_QCBOR_TimestampParserImpl,
    .LifetimeParser = BPLib_QCBOR_UInt64ParserImpl,
    .CRCParser = BPLib_QCBOR_CRCParserImpl
};

BPLib_Status_t BPLib_CBOR_VerifyBundleProcFlags(uint64_t BundleProcFlags)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (BundleProcFlags & BPLIB_BUNDLE_PROC_ADMIN_RECORD_FLAG)
    { /* Bundle processing flags indicate this is an admin record bundle */
        /* Status reporting flags should be 0 */
        if (BundleProcFlags & BPLIB_BUNDLE_PROC_RECV_REPORT_FLAG ||
            BundleProcFlags & BPLIB_BUNDLE_PROC_FORWARD_FLAG     ||
            BundleProcFlags & BPLIB_BUNDLE_PROC_DELIVERY_FLAG    ||
            BundleProcFlags & BPLIB_BUNDLE_PROC_DELETE_FLAG)
        {
            Status = BPLIB_CBOR_DEC_PRIM_WRONG_FLAG_ERR;
        }
    }

    return Status;
}

/*******************************************************************************
* RFC-9171 Primary Block Parser (Implementation)
*/

BPLib_Status_t BPLib_CBOR_DecodePrimary(QCBORDecodeContext* ctx, BPLib_Bundle_t* bundle,
                                                                const void *CandBundle)
{
    BPLib_Status_t Status;
    size_t CurrArrLen;
    QCBORError NextElementType;
    QCBORItem PeekItem;
    
    if ((ctx == NULL) || (bundle == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    /*
    ** The version field is currently not stored in our bundle metadata,
    ** so we decode this to a local stack buffer (for verification purposes only).
    */
    uint64_t Version;

    /* Grab the current offset, to be kept in the primary block's metadata */
    /* TODO: we may not need this info */
    bundle->blocks.PrimaryBlock.BlockOffsetStart = QCBORDecode_Tell(ctx);

    /* First, enter into the primary block array */
    Status = BPLib_QCBOR_EnterDefiniteArray(ctx, &CurrArrLen);
    if (Status != BPLIB_SUCCESS)
    {
        return Status;
    }

    /* Version */
    Status = PrimaryBlockParser.VersionParser(ctx, &Version);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRIM_VERSION_DEC_ERR;
    }
    if (Version != BPLIB_BUNDLE_PROTOCOL_VERSION)
    {
        return BPLIB_CBOR_DEC_PRIM_WRONG_VERSION_ERR;
    }

    /* Flags */
    Status = PrimaryBlockParser.FlagsParser(ctx, &bundle->blocks.PrimaryBlock.BundleProcFlags);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRIM_FLAG_DEC_ERR;
    }

    Status = BPLib_CBOR_VerifyBundleProcFlags(bundle->blocks.PrimaryBlock.BundleProcFlags);
    if (Status != BPLIB_SUCCESS)
    {
        return Status;
    }

    /* CRC Type */
    Status = PrimaryBlockParser.CRCTypeParser(ctx, &bundle->blocks.PrimaryBlock.CrcType);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRIM_CRC_TYPE_DEC_ERR;
    }

    /* Validate CRC Type */
    /* Note: CRC NONE is not currently supported since bundle security blocks are not allowed yet */
    if (bundle->blocks.PrimaryBlock.CrcType != BPLib_CRC_Type_CRC16 &&
        bundle->blocks.PrimaryBlock.CrcType != BPLib_CRC_Type_CRC32C)
    {
        return BPLIB_CBOR_DEC_TYPES_CRC_UNSUPPORTED_TYPE_ERR;
    }

    /* Dest EID */
    Status = PrimaryBlockParser.DestEIDParser(ctx, &bundle->blocks.PrimaryBlock.DestEID);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRIM_DEST_EID_DEC_ERR;
    }

    /* Source EID */
    Status = PrimaryBlockParser.SrcEIDParser(ctx, &bundle->blocks.PrimaryBlock.SrcEID);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRIM_SRC_EID_DEC_ERR;
    }

    /* Report EID */
    Status = PrimaryBlockParser.ReportEIDParser(ctx, &bundle->blocks.PrimaryBlock.ReportToEID);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRIM_REPORT_EID_DEC_ERR;
    }

    /* Creation Timestamp */
    Status = PrimaryBlockParser.CreationTimestampParser(ctx, &bundle->blocks.PrimaryBlock.Timestamp);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRIM_CREATE_TIME_DEC_ERR;
    }

    /* Lifetime */
    Status = PrimaryBlockParser.LifetimeParser(ctx, &bundle->blocks.PrimaryBlock.Lifetime);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRIM_LIFETIME_DEC_ERR;
    }

    /* CRC Value */
    Status = PrimaryBlockParser.CRCParser(ctx, &bundle->blocks.PrimaryBlock.CrcVal,
                                                bundle->blocks.PrimaryBlock.CrcType);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRIM_CRC_VAL_DEC_ERR;
    }

    /* Exit the primary block array */
    Status = BPLib_QCBOR_ExitDefiniteArray(ctx);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRIM_EXIT_ARRAY_ERR;
    }

    NextElementType = QCBORDecode_PeekNext(ctx, &PeekItem);
    if (NextElementType == QCBOR_ERR_NO_MORE_ITEMS)
    {
        return BPLIB_CBOR_DEC_NO_PAYLOAD_ERR;
    }

    /* Grab the current offset, to be kept in the primary block's metadata */
    bundle->blocks.PrimaryBlock.BlockOffsetEnd = QCBORDecode_Tell(ctx) - 1;

    /* Clear the primary block's "dirty bit", so we know we can skip re-encoding it */
    bundle->blocks.PrimaryBlock.RequiresEncode = false;

    /* Validate primary block CRC */
    Status = BPLib_CBOR_ValidateBlockCrc(CandBundle, bundle->blocks.PrimaryBlock.CrcType,
                                    bundle->blocks.PrimaryBlock.CrcVal,
                                    bundle->blocks.PrimaryBlock.BlockOffsetStart,
                                    bundle->blocks.PrimaryBlock.BlockOffsetEnd -
                                    bundle->blocks.PrimaryBlock.BlockOffsetStart + 1);
    if (Status != BPLIB_SUCCESS)
    {
        return Status;
    }

    #if (BPLIB_CBOR_DEBUG_PRINTS_ENABLED)
    printf("Primary Block: \n");
    printf("\t CRC Type: %lu\n", bundle->blocks.PrimaryBlock.CrcType);
    printf("\t Flags: %lu\n", bundle->blocks.PrimaryBlock.BundleProcFlags);
    printf("\t Dest EID (scheme.node.service): %lu.%lu.%lu\n", bundle->blocks.PrimaryBlock.DestEID.Scheme,
                                                               bundle->blocks.PrimaryBlock.DestEID.Node,
                                                               bundle->blocks.PrimaryBlock.DestEID.Service);
    printf("\t Source EID (scheme.node.service): %lu.%lu.%lu\n", bundle->blocks.PrimaryBlock.SrcEID.Scheme,
                                                                 bundle->blocks.PrimaryBlock.SrcEID.Node,
                                                                 bundle->blocks.PrimaryBlock.SrcEID.Service);
    printf("\t Report-To EID (scheme.node.service): %lu.%lu.%lu\n", bundle->blocks.PrimaryBlock.ReportToEID.Scheme,
                                                                    bundle->blocks.PrimaryBlock.ReportToEID.Node,
                                                                    bundle->blocks.PrimaryBlock.ReportToEID.Service);
    printf("\t Timestamp (created, seq): %lu, %lu\n", bundle->blocks.PrimaryBlock.Timestamp.CreateTime,
                                                      bundle->blocks.PrimaryBlock.Timestamp.SequenceNumber);
    printf("\t Lifetime: %lu\n", bundle->blocks.PrimaryBlock.Lifetime);
    printf("\t CRC Value: 0x%lX\n", bundle->blocks.PrimaryBlock.CrcVal);
    printf("\t Requires Encode: %u\n", bundle->blocks.PrimaryBlock.RequiresEncode);
    printf("\t Block Offset Start: %lu\n", bundle->blocks.PrimaryBlock.BlockOffsetStart);
    printf("\t Block Offset End: %lu\n", bundle->blocks.PrimaryBlock.BlockOffsetEnd);
    printf("\t Block Size: %lu\n",
        bundle->blocks.PrimaryBlock.BlockOffsetEnd - bundle->blocks.PrimaryBlock.BlockOffsetStart + 1);
    #endif

    return BPLIB_SUCCESS;
}
