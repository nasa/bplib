#ifndef __GSFC_BPLIB_TYPES_BUNDLE_H__
#define __GSFC_BPLIB_TYPES_BUNDLE_H__

#define MAX_BUNDLE_EXT_BLOCKS 3

typedef struct BPLib_Bundle_Metadata
{
    void (*job_func)();
    // We may need a dummy field for FPGA-specific meta
    // We may need a field for cache
} BPLib_Bundle_Metadata_t;

typedef struct BPLib_PrimaryBlock
{
} BPLib_PrimaryBlock_t;

// This is attempting to be cross-compatible with FPGA team's bundle definition
typedef struct BPLib_Bundle
{
    BPLib_Bundle_Metadata_t metadata;
    BPLib_PrimaryBlock_t pri_blk;
    // Only the payload block header, not actual payload
    BPLib_PayloadBlock_t pay_blk;
     // For now this is only 9171 blocks
    BPLib_ExtensionBlock_t ext_blks[MAX_BUNDLE_EXT_BLOCKS];

    // Non-7.0 Blocks
    // Defintion for CREB and CTEB
    // Definition for BPSec

    // The full encoded bundle
    BPLib_MEM_Block_t* blob_start;
} BPLib_Bundle_t;

#endif /* __GSFC_BPLIB_TYPES_BUNDLE_H__ */
