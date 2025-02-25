#include "bplib_cbor_primary.h"

#define BP_VERSION    7

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
        return BPLIB_NULL_PTR_ERR;
    }

    /* ATTENTION: DO NOT USE THESE IN THE FINAL IMPLEMENTATION 
    ** They are a stand in for passing &bundle->Version directly to the parser.
    ** An extra assignment ends up being an implicit copy. I did this because
    ** I didn't have the final Bundle_t definition.
    */
    uint64_t Version;
    uint64_t Flags;
    uint64_t CrcType;

    /* First, enter into the primary block array */
    Status = BPLib_QCBOR_EnterDefiniteArray(ctx, &CurrArrLen);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRI_ERR;
    }

    /* Version */
    Status = PrimaryBlockParser.VersionParser(ctx, &Version);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRI_ERR;
    }
    if (Version != BP_VERSION)
    {
        return BPLIB_CBOR_DEC_PRI_ERR;
    }

    /* Flags */
    Status = PrimaryBlockParser.FlagsParser(ctx, &Flags);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRI_ERR;
    }
    /* Check flags to make sure we support the requested options */
    if (Flags != 4) // ¯\_(ツ)_/¯
    {
        return BPLIB_CBOR_DEC_PRI_ERR;
    }

    /* CRC Type */
    Status = PrimaryBlockParser.CRCTypeParser(ctx, &CrcType);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRI_ERR;
    }

    /* Source EID */
    Status = PrimaryBlockParser.SrcEIDParser(ctx, &bundle->SrcEID);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRI_ERR;
    }

    /* Dest EID */
    Status = PrimaryBlockParser.DestEIDParser(ctx, &bundle->DestEID);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRI_ERR;
    }

    /* Report EID */
    Status = PrimaryBlockParser.ReportEIDParser(ctx, &bundle->ReportEID);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRI_ERR;
    }

    /* Creation Timestamp */
    Status = PrimaryBlockParser.CreationTimestampParser(ctx, &bundle->CreationTimestamp);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRI_ERR;
    }

    /* Exit the primary block array */
    Status = BPLib_QCBOR_ExitDefiniteArray(ctx);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRI_ERR;
    }

    return BPLIB_SUCCESS;
}
