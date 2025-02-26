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


/*
** Look through the provided `BPLib_Bundle_t* bundle` to identify
** which extension block we're currently decoding
** assumes that unused extension block headers will have Header.BlockType set to BPLib_BlockType_Reserved
*/
BPLib_Status_t BPLib_GetNextFreeExtensionBlockIndex(BPLib_Bundle_t* bundle, uint16_t* BlockIndex)
{
    BPLib_Status_t ReturnStatus;
    bool FoundNextBlock = false;
    uint16_t NextBlockIndex = 0;

    if ((bundle == NULL) || (BlockIndex == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    for (NextBlockIndex = 0; NextBlockIndex < BPLIB_MAX_NUM_EXTENSION_BLOCKS; NextBlockIndex++)
    {
        if (bundle->blocks.ExtBlocks->Header.BlockType == BPLib_BlockType_Reserved)
        {
            BlockIndex = NextBlockIndex;
            FoundNextBlock = true;
            break;
        }
    }

    if (FoundNextBlock)
    {
        return BPLIB_SUCCESS;
    }
    else
    {
        return BPLIB_CBOR_DEC_CANON_LIM_ERR;
    }
}

/*******************************************************************************
* RFC-9171 Canonical Block Parsers (Implementation)
*/
BPLib_Status_t BPLib_CBOR_DecodeCanonical(QCBORDecodeContext* ctx, BPLib_Bundle_t* bundle)
{
    BPLib_Status_t Status;
    BPLib_Status_t NextBlockIndexStatus;
    uint16_t BlockIndex = 0;
    BPLib_ExtensionBlock_t* CanonicalBlock;
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

    /* Once we know the block type, we can be smarter about decoding data directly into `bundle` */
    if (BlockType == BPLib_BlockType_Payload)
    {
        CanonicalBlock = &bundle->blocks.PayloadHeader;
    }
    else
    {
        NextBlockIndexStatus = BPLib_GetNextFreeExtensionBlockIndex(bundle, &BlockIndex);
        if (Status != BPLIB_SUCCESS)
        {
            return NextBlockIndexStatus;
        }
        if (BlockIndex >= BPLIB_MAX_NUM_EXTENSION_BLOCKS)
        {
            return BPLIB_CBOR_DEC_CANON_LIM_ERR;
        }
        CanonicalBlock = &bundle->blocks.ExtBlocks[BlockIndex];
    }
    CanonicalBlock->Header.BlockType = BlockType;

    /* Block Number */
    Status = CanonicalBlockParser.BlockNumberParser(ctx, &CanonicalBlock->Header.BlockNum);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_CANON_ERR;
    }

    /* Flags */
    Status = CanonicalBlockParser.FlagsParser(ctx, &CanonicalBlock->Header.BundleProcFlags);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_CANON_ERR;
    }

    /* CRC Type */
    Status = CanonicalBlockParser.CRCTypeParser(ctx, &CanonicalBlock->Header.CrcType);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_CANON_ERR;
    }

    printf("Canonical Block: \n");
    printf("\t Block Type: %lu\n", CanonicalBlock->Header.BlockType);
    printf("\t Block Number: %lu\n", CanonicalBlock->Header.BlockNum);
    printf("\t Flags: %lu\n", CanonicalBlock->Header.BundleProcFlags);
    printf("\t CRC Type: %lu\n", CanonicalBlock->Header.CrcType);

    /* Exit the canonical block array */
    Status = BPLib_QCBOR_ExitDefiniteArray(ctx);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRI_ERR;
    }

    return BPLIB_SUCCESS;
}
