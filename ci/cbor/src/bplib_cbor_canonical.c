#include <stdio.h>

#include "bplib_cbor_private.h"

#define BPLIB_CBOR_BLOCKTYPE_PAYLOAD    1
#define BPLIB_CBOR_BLOCKTYPE_PREVNODE   6
#define BPLIB_CBOR_BLOCKTYPE_AGE        7
#define BPLIB_CBOR_BLOCKTYPE_HOPCNT     10

/*******************************************************************************
* RFC-9171 Canonical Block Parsing Definitions
*/

/* Shared Canonical Block State */
struct  _CanonicalBlockBaseParser
{
    QCBOR_UInt64Parser BlockTypeParser;
    QCBOR_UInt64Parser BlockNumberParser;
    QCBOR_UInt64Parser FlagsParser;
    QCBOR_UInt64Parser CRCTypeParser;
};

#ifdef REMOVED_DUE_TO_ERROR // error: struct has no members [-Werror=pedantic]
/* Age Block Payload */
struct _AgeBlockParser
{

};

/* Prev Node Block Payload */
struct _PrevNodeBlockParser
{

};

/* Hop Count Block Payload */
struct _HopCountBlockParser
{

};

struct _PayloadBlockParser
{

};
#endif // REMOVED_DUE_TO_ERROR

static struct _CanonicalBlockBaseParser CanonicalBlockParser = {
    .BlockTypeParser = BPLib_QCBOR_UInt64ParserImpl,
    .BlockNumberParser = BPLib_QCBOR_UInt64ParserImpl,
    .FlagsParser = BPLib_QCBOR_UInt64ParserImpl,
    .CRCTypeParser = BPLib_QCBOR_UInt64ParserImpl
};

/*******************************************************************************
* RFC-9171 Canonical Block Parsers (Implementation)
*/
BPLib_Status_t BPLib_CBOR_DecodeCanonical(QCBORDecodeContext* ctx, BPLib_Bundle_t* bundle)
{
    BPLib_Status_t Status;
    size_t ArrayLen;

    uint64_t BlockType, BlockNum, Flags, CRCType;

    if ((ctx == NULL) || (bundle == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    /* Enter the canonical block array */
    Status = BPLib_QCBOR_EnterDefiniteArray(ctx, &ArrayLen);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_CANON_ERR;
    }

    /* Block Type */
    Status = CanonicalBlockParser.BlockTypeParser(ctx, &BlockType);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_CANON_ERR;
    }

    /* Block Number */
    Status = CanonicalBlockParser.BlockNumberParser(ctx, &BlockNum);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_CANON_ERR;
    }

    /* Flags */
    Status = CanonicalBlockParser.FlagsParser(ctx, &Flags);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_CANON_ERR;
    }

    /* CRC Type */
    Status = CanonicalBlockParser.CRCTypeParser(ctx, &CRCType);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_CANON_ERR;
    }

    printf("Canonical Block: \n");
    printf("\t Block Type: %lu\n", BlockType);
    printf("\t Block Number: %lu\n", BlockNum);
    printf("\t Flags: %lu\n", Flags);
    printf("\t CRC Type: %lu\n", CRCType);

    /* Exit the canonical block array */
    Status = BPLib_QCBOR_ExitDefiniteArray(ctx);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRI_ERR;
    }

    return BPLIB_SUCCESS;
}
