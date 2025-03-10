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

#include "bplib_cbor_internal.h"


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
    .SrcEIDParser = BPLib_QCBOR_EIDParserImpl,
    .DestEIDParser = BPLib_QCBOR_EIDParserImpl,
    .ReportEIDParser = BPLib_QCBOR_EIDParserImpl,
    .CreationTimestampParser = BPLib_QCBOR_TimestampParserImpl,
    .LifetimeParser = BPLib_QCBOR_UInt64ParserImpl,
    .CRCParser = BPLib_QCBOR_CRCParserImpl
};

/*******************************************************************************
* RFC-9171 Primary Block Parser (Implementation)
*/

BPLib_Status_t BPLib_CBOR_DecodePrimary(QCBORDecodeContext* ctx, BPLib_Bundle_t* bundle)
{
    BPLib_Status_t Status;
    size_t CurrArrLen;

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
        return BPLIB_CBOR_DEC_PRIM_ENTER_ARRAY_ERR;
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
    /* Check flags to make sure we support the requested options */
    if (bundle->blocks.PrimaryBlock.BundleProcFlags != 4)
    {
        return BPLIB_CBOR_DEC_PRIM_WRONG_FLAG_ERR;
    }

    /* CRC Type */
    Status = PrimaryBlockParser.CRCTypeParser(ctx, &bundle->blocks.PrimaryBlock.CrcType);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRIM_CRC_TYPE_DEC_ERR;
    }
    /*
    ** TODO: since we don't support BPSec yet,
    ** technically we could throw an error here when CRC is "none".
    ** Ref: https://www.rfc-editor.org/rfc/rfc9171.html#section-4.3.1-5.6
    */

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

    /* Grab the current offset, to be kept in the primary block's metadata */
    bundle->blocks.PrimaryBlock.BlockOffsetEnd = QCBORDecode_Tell(ctx) - 1;

    /* Clear the primary block's "dirty bit", so we know we can skip re-encoding it */
    bundle->blocks.PrimaryBlock.RequiresEncode = false;


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
    printf("\t Block Offset Start: %u\n", bundle->blocks.PrimaryBlock.BlockOffsetStart);
    printf("\t Block Offset End: %u\n", bundle->blocks.PrimaryBlock.BlockOffsetEnd);
    printf("\t Block Size: %u\n",
        bundle->blocks.PrimaryBlock.BlockOffsetEnd - bundle->blocks.PrimaryBlock.BlockOffsetStart + 1);
    #endif

    return BPLIB_SUCCESS;
}
