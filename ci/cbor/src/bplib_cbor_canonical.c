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

/* Age Block Payload */
struct _AgeBlockParser
{
    QCBOR_UInt64Parser BundleAge;
};

/* Prev Node Block Payload */
struct _PrevNodeBlockParser
{
    QCBOR_UInt64Parser ForwardedEID;
};

/* Hop Count Block Payload */
struct _HopCountBlockParser
{
    QCBOR_UInt64Parser BundleHopLimit;
    QCBOR_UInt64Parser BundleHopCount;

};

#ifdef REMOVED_DUE_TO_ERROR // error: struct has no members [-Werror=pedantic]
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
    uint16_t BlockIndex = 0;
    BPLib_CanBlockHeader_t* CanonicalBlockHdr;
    BPLib_CanBlockHeader_t SpareCanonicalBlockHdr;
    size_t ArrayLen;
    uint64_t BlockType;

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

    /* Once we know the block type, we can be smarter about decoding data directly into `bundle` */
    if (BlockType == BPLib_BlockType_Payload)
    {
        CanonicalBlockHdr = &bundle->blocks.PayloadHeader;
    }
    else
    {
        /* This is an extension block; find a free entry in the bblock data */
        for (BlockIndex = 0; BlockIndex < BPLIB_MAX_NUM_EXTENSION_BLOCKS; BlockIndex++)
        {
            /* assume that a free entry will have the block type set to BPLib_BlockType_Reserved (0) */
            if (bundle->blocks.ExtBlocks[BlockIndex].Header.BlockType == BPLib_BlockType_Reserved)
            {
                break;
            }
        }
        if (BlockIndex >= BPLIB_MAX_NUM_EXTENSION_BLOCKS)
        {
            /*
            ** all of the ext block metadata fields were taken
            ** eventually, we might want to throw an error here.
            ** however, for now, continue decoding the block data into a spare buffer, for debugging
            */
            CanonicalBlockHdr = &SpareCanonicalBlockHdr;
            /*
            return BPLIB_CBOR_DEC_CANON_LIM_ERR;
            */
        }
        else
        {
            CanonicalBlockHdr = &bundle->blocks.ExtBlocks[BlockIndex].Header;
        }
    }
    CanonicalBlockHdr->BlockType = BlockType;

    /* Block Number */
    Status = CanonicalBlockParser.BlockNumberParser(ctx, &CanonicalBlockHdr->BlockNum);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_CANON_ERR;
    }

    /* Flags */
    Status = CanonicalBlockParser.FlagsParser(ctx, &CanonicalBlockHdr->BundleProcFlags);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_CANON_ERR;
    }

    /* CRC Type */
    Status = CanonicalBlockParser.CRCTypeParser(ctx, &CanonicalBlockHdr->CrcType);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_CANON_ERR;
    }

    printf("Canonical Block: \n");
    printf("\t Block Type: %lu\n", CanonicalBlockHdr->BlockType);
    printf("\t Block Number: %lu\n", CanonicalBlockHdr->BlockNum);
    printf("\t Flags: %lu\n", CanonicalBlockHdr->BundleProcFlags);
    printf("\t CRC Type: %lu\n", CanonicalBlockHdr->CrcType);

    /* Exit the canonical block array */
    Status = BPLib_QCBOR_ExitDefiniteArray(ctx);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRI_ERR;
    }

    return BPLIB_SUCCESS;
}
