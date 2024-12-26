#ifndef __GSFC_BPLIB_TYPES_BUNDLE_H__
#define __GSFC_BPLIB_TYPES_BUNDLE_H__

#include "pool.h"

typedef enum BPLib_BundleState
{
    STATE_CLA_TO_BI = 0,
    STATE_BI_TO_EBP,
    STATE_EBP_TO_CT,
    STATE_CT_TO_CACHE,

    /* Internal Cache FSM states */

    STATE_CACHE_TO_CT,
    STATE_CT_TO_EBP,
    STATE_EBP_TO_BI,
    STATE_BI_TO_CLA,
    NUM_BUNDLE_STATE
} BPLib_BundleState_t;

// This is attempting to be cross-compatible with FPGA team's bundle definition
typedef struct BPLib_Bundle
{
    BPLib_MEM_Block_t* bblocks; // "Everything we care about"
    BPLib_MEM_Block_t* blob_start; // "Everything we dont care about"
    BPLib_BundleState_t job_state;
} BPLib_Bundle_t;

#endif /* __GSFC_BPLIB_TYPES_BUNDLE_H__ */
