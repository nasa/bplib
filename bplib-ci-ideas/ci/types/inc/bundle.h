#ifndef __GSFC_BPLIB_TYPES_BUNDLE_H__
#define __GSFC_BPLIB_TYPES_BUNDLE_H__

#include "pool.h"

// This is attempting to be cross-compatible with FPGA team's bundle definition
typedef struct BPLib_Bundle
{
    BPLib_MEM_Block_t* bblocks; // "Everything we care about"
    BPLib_MEM_Block_t* blob; // "Everything we dont care about"
} BPLib_Bundle_t;

#endif /* __GSFC_BPLIB_TYPES_BUNDLE_H__ */
