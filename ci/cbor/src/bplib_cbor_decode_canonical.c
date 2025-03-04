#include <stdio.h>

#include "bplib_cbor_internal.h"


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
    QCBOR_CRCParser CRCParser;
};

/* Age Block Payload */
struct _AgeBlockDataParser
{
    QCBOR_UInt64Parser BundleAgeParser;
};

/* Prev Node Block Payload */
struct _PrevNodeBlockDataParser
{
    QCBOR_EIDParser EidForwardedParser;
};

/* Hop Count Block Payload */
struct _HopCountBlockDataParser
{
    QCBOR_UInt64Parser BundleHopLimitParser;
    QCBOR_UInt64Parser BundleHopCountParser;
};


static struct _CanonicalBlockBaseParser CanonicalBlockParser = {
    .BlockTypeParser = BPLib_QCBOR_UInt64ParserImpl,
    .BlockNumberParser = BPLib_QCBOR_UInt64ParserImpl,
    .FlagsParser = BPLib_QCBOR_UInt64ParserImpl,
    .CRCTypeParser = BPLib_QCBOR_UInt64ParserImpl,
    .CRCParser = BPLib_QCBOR_CRCParserImpl
};


static struct _AgeBlockDataParser AgeBlockDataParser = {
    .BundleAgeParser = BPLib_QCBOR_UInt64ParserImpl
};


static struct _PrevNodeBlockDataParser PrevNodeBlockDataParser = {
    .EidForwardedParser = BPLib_QCBOR_EIDParserImpl
};


static struct _HopCountBlockDataParser HopCountBlockDataParser = {
    .BundleHopLimitParser = BPLib_QCBOR_UInt64ParserImpl,
    .BundleHopCountParser = BPLib_QCBOR_UInt64ParserImpl
};



/*******************************************************************************
* RFC-9171 Canonical Block Parsers (Implementation)
*/
BPLib_Status_t BPLib_CBOR_DecodeCanonical(QCBORDecodeContext* ctx,
                                          BPLib_Bundle_t* bundle,
                                          uint32_t CanonicalBlockIndex)
{
    BPLib_Status_t Status;
    BPLib_CanBlockHeader_t* CanonicalBlockHdr;
    BPLib_CanBlockHeader_t SpareCanonicalBlockHdr;
    size_t ArrayLen;
    size_t HopCountBlockDataArrayLen;
    QCBORError QStatus;
    uint64_t BlockType;
    uint32_t HeaderStartOffset;
    UsefulBufC CanonBlockDataByteStrInfo;

    if ((ctx == NULL) || (bundle == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    if (CanonicalBlockIndex >= BPLIB_MAX_NUM_CANONICAL_BLOCKS)
    {
        return BPLIB_CBOR_DEC_CANON_BLOCK_INDEX_ERR;
    }

    /* Grab the current offset, to be kept in the canonical block's metadata */
    HeaderStartOffset = QCBORDecode_Tell(ctx);

    /* Enter the canonical block array */
    Status = BPLib_QCBOR_EnterDefiniteArray(ctx, &ArrayLen);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_CANON_ENTER_ARRAY_ERR;
    }

    /* Block Type */
    Status = CanonicalBlockParser.BlockTypeParser(ctx, &BlockType);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_CANON_BLOCK_TYPE_DEC_ERR;
    }

    /* Once we know the block type, we can be smarter about decoding data directly into `bundle` */
    if (BlockType == BPLib_BlockType_Payload)
    {
        CanonicalBlockHdr = &bundle->blocks.PayloadHeader;
    }
    else
    {
        /* This is an extension block  */
        if (CanonicalBlockIndex >= BPLIB_MAX_NUM_EXTENSION_BLOCKS)
        {
            /*
            ** all of the ext block metadata fields were taken
            ** eventually, we might want to throw an error here.
            ** however, for now, continue decoding the block data into a spare buffer, for debugging
            */
            CanonicalBlockHdr = &SpareCanonicalBlockHdr;
        }
        else
        {
            CanonicalBlockHdr = &bundle->blocks.ExtBlocks[CanonicalBlockIndex].Header;
        }
    }
    CanonicalBlockHdr->BlockType = BlockType;
    CanonicalBlockHdr->HeaderOffset = HeaderStartOffset;

    /* Block Number */
    Status = CanonicalBlockParser.BlockNumberParser(ctx, &CanonicalBlockHdr->BlockNum);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_CANON_BLOCK_NUM_DEC_ERR;
    }

    /* Flags */
    Status = CanonicalBlockParser.FlagsParser(ctx, &CanonicalBlockHdr->BundleProcFlags);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_CANON_BLOCK_FLAG_DEC_ERR;
    }

    /* CRC Type */
    Status = CanonicalBlockParser.CRCTypeParser(ctx, &CanonicalBlockHdr->CrcType);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_CANON_CRC_TYPE_DEC_ERR;
    }

    /*
    ** next should be the canonical-block-specific data
    ** this should be wrapped in a CBOR byte-string
    */
    QCBORDecode_EnterBstrWrapped(ctx, QCBOR_TAG_REQUIREMENT_NOT_A_TAG, &CanonBlockDataByteStrInfo);
    QStatus = QCBORDecode_GetError(ctx);
    if (QStatus != QCBOR_SUCCESS)
    {
        return BPLIB_CBOR_DEC_CANON_ENTER_BYTE_STR_ERR;
    }

    /*
    ** Grab the current offset, to be kept in the canonical block's metadata
    ** this should be after we enter the byte string
    */
    CanonicalBlockHdr->DataOffset = QCBORDecode_Tell(ctx);

    if (CanonicalBlockHdr->BlockType == BPLib_BlockType_PrevNode)
    {
        Status = PrevNodeBlockDataParser.EidForwardedParser(ctx,
            &bundle->blocks.ExtBlocks[CanonicalBlockIndex].BlockData.PrevNodeBlockData.PrevNodeId);
        if (Status != BPLIB_SUCCESS)
        {
            return BPLIB_CBOR_DEC_PREV_NODE_EID_DEC_ERR;
        }
    }
    else if (CanonicalBlockHdr->BlockType == BPLib_BlockType_Age)
    {
        Status = AgeBlockDataParser.BundleAgeParser(ctx,
            &bundle->blocks.ExtBlocks[CanonicalBlockIndex].BlockData.AgeBlockData.Age);
        if (Status != BPLIB_SUCCESS)
        {
            return BPLIB_CBOR_DEC_AGE_BLOCK_DEC_ERR;
        }
    }
    else if (CanonicalBlockHdr->BlockType == BPLib_BlockType_HopCount)
    {
        /* Enter the hop count block data array */
        Status = BPLib_QCBOR_EnterDefiniteArray(ctx, &HopCountBlockDataArrayLen);
        if (Status != BPLIB_SUCCESS)
        {
            return BPLIB_CBOR_DEC_HOP_BLOCK_ENTER_ARRAY_ERR;
        }

        Status = HopCountBlockDataParser.BundleHopLimitParser(ctx,
            &bundle->blocks.ExtBlocks[CanonicalBlockIndex].BlockData.HopCountData.HopLimit);
        if (Status != BPLIB_SUCCESS)
        {
            return BPLIB_CBOR_DEC_HOP_BLOCK_HOP_LIMIT_DEC_ERR;
        }
        Status = HopCountBlockDataParser.BundleHopCountParser(ctx,
            &bundle->blocks.ExtBlocks[CanonicalBlockIndex].BlockData.HopCountData.HopCount);
        if (Status != BPLIB_SUCCESS)
        {
            return BPLIB_CBOR_DEC_HOP_BLOCK_HOP_COUNT_DEC_ERR;
        }

        /* Exit the hop count block data array */
        Status = BPLib_QCBOR_ExitDefiniteArray(ctx);
        if (Status != BPLIB_SUCCESS)
        {
            return BPLIB_CBOR_DEC_HOP_BLOCK_EXIT_ARRAY_ERR;
        }
    }
    else if (CanonicalBlockHdr->BlockType == BPLib_BlockType_Payload)
    {
        bundle->blocks.PrimaryBlock.TotalAduLength = CanonBlockDataByteStrInfo.len;
    }
    else
    {
        /* TODO: Should we return an error here? */
    }

    /* exit the byte-string */
    QCBORDecode_ExitBstrWrapped(ctx);
    QStatus = QCBORDecode_GetError(ctx);
    if (QStatus != QCBOR_SUCCESS)
    {
        return BPLIB_CBOR_DEC_CANON_EXIT_BYTE_STR_ERR;
    }

    /* Grab the current offset, to be kept in the canonical block's metadata */
    CanonicalBlockHdr->EncodedCrcValOffset = QCBORDecode_Tell(ctx);

    /* CRC Value */
    Status = CanonicalBlockParser.CRCParser(ctx, &CanonicalBlockHdr->CrcVal, CanonicalBlockHdr->CrcType);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_CANON_CRC_VAL_DEC_ERR;
    }

    /* Grab the current offset, to calculate the encoded CRC value's size */
    CanonicalBlockHdr->EncodedCrcValSize = QCBORDecode_Tell(ctx) - CanonicalBlockHdr->EncodedCrcValOffset;

    /* Exit the canonical block array */
    Status = BPLib_QCBOR_ExitDefiniteArray(ctx);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_CANON_EXIT_ARRAY_ERR;
    }

    #if (BPLIB_CBOR_DEBUG_PRINTS_ENABLED)
    printf("Canonical Block [%u]: \n", CanonicalBlockIndex);
    printf("\t Block Type: %lu\n", CanonicalBlockHdr->BlockType);
    printf("\t Block Number: %lu\n", CanonicalBlockHdr->BlockNum);
    printf("\t Flags: %lu\n", CanonicalBlockHdr->BundleProcFlags);
    printf("\t CRC Type: %lu\n", CanonicalBlockHdr->CrcType);
    printf("\t Header Offset: %lu\n", CanonicalBlockHdr->HeaderOffset);
    printf("\t Data Offset: %lu\n", CanonicalBlockHdr->DataOffset);
    switch (CanonicalBlockHdr->BlockType)
    {
        case BPLib_BlockType_PrevNode:
            printf("\t Prev Node Block Data: \n");
            printf("\t\t Prev Node Block MetaData Length: %lu\n", sizeof(BPLib_PrevNodeBlockData_t));
            printf("\t\t EID Forwarded (scheme.node.service): %lu.%lu.%lu\n",
                bundle->blocks.ExtBlocks[CanonicalBlockIndex].BlockData.PrevNodeBlockData.PrevNodeId.Scheme, 
                bundle->blocks.ExtBlocks[CanonicalBlockIndex].BlockData.PrevNodeBlockData.PrevNodeId.Node,
                bundle->blocks.ExtBlocks[CanonicalBlockIndex].BlockData.PrevNodeBlockData.PrevNodeId.Service);
            break;
        case BPLib_BlockType_Age:
            printf("\t Age Block Data: \n");
            printf("\t\t Age Block MetaData Length: %lu\n", sizeof(BPLib_AgeBlockData_t));
            printf("\t\t Age (in milliseconds): %lu\n",
                bundle->blocks.ExtBlocks[CanonicalBlockIndex].BlockData.AgeBlockData.Age);
            break;
        case BPLib_BlockType_HopCount:
            printf("\t Hop Count Block Data: \n");
            printf("\t\t Hop Count MetaData Length: %lu\n", sizeof(BPLib_HopCountData_t));
            printf("\t\t Hop Count Definite Array Length: %lu\n", HopCountBlockDataArrayLen);
            printf("\t\t Hop Limit: %lu\n",
                bundle->blocks.ExtBlocks[CanonicalBlockIndex].BlockData.HopCountData.HopLimit);
            printf("\t\t Hop Count: %lu\n",
                bundle->blocks.ExtBlocks[CanonicalBlockIndex].BlockData.HopCountData.HopCount);
            break;
        case BPLib_BlockType_CTEB:
            printf("\t CTEB Block Data Parsing Skipped!\n");
            break;
        case BPLib_BlockType_CREB:
            printf("\t CREB Block Data Parsing Skipped!\n");
            break;
        case BPLib_BlockType_Payload:
            printf("\t Payload Block Data Length: %lu bytes\n",
                bundle->blocks.PrimaryBlock.TotalAduLength);
            break;
        default:
            printf("\t Unrecognized Block (%lu) Data Parsing Skipped!\n", CanonicalBlockHdr->BlockType);
            break;
    }
    printf("\t CRC Value: 0x%lX\n", CanonicalBlockHdr->CrcVal);
    #endif


    return BPLIB_SUCCESS;
}
