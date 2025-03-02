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
    uint32_t PrimaryBlockStartOffset;
    uint32_t PrimaryBlockEndOffset;

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
    PrimaryBlockStartOffset = QCBORDecode_Tell(ctx);
    bundle->blocks.PrimaryBlock.OffsetIntoBlob = PrimaryBlockStartOffset;

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
    #endif


    /* Exit the primary block array */
    Status = BPLib_QCBOR_ExitDefiniteArray(ctx);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRIM_EXIT_ARRAY_ERR;
    }

    /* Grab the current offset, to be kept in the primary block's metadata */
    PrimaryBlockEndOffset = QCBORDecode_Tell(ctx);
    if (PrimaryBlockStartOffset >= PrimaryBlockEndOffset)
    {
        /*
        ** TODO:
        **  - do we need this `if` block to sanity check the output offsets from QCBOR, or
        **    can we trust QCBOR?
        **  - make this error code unique if we keep it.
        */
        return BPLIB_ERROR;
    }
    else
    {
        bundle->blocks.PrimaryBlock.EncodedSize = PrimaryBlockEndOffset - PrimaryBlockStartOffset;
    }

    /* Clear the primary block's "dirty bit", so we know we can skip re-encoding it */
    bundle->blocks.PrimaryBlock.RequiresEncode = false;

    return BPLIB_SUCCESS;
}
