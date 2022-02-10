/************************************************************************
 *
 *  Copyright 2019 United States Government as represented by the
 *  Administrator of the National Aeronautics and Space Administration.
 *  All Other Rights Reserved.
 *
 *  This software was created at NASA's Goddard Space Flight Center.
 *  This software is governed by the NASA Open Source Agreement and may be
 *  used, distributed and modified only pursuant to the terms of that
 *  agreement.
 *
 *************************************************************************/

#ifndef v7_codec_h
#define v7_codec_h

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "v7_mpool.h"

/*
 * On the decode side of things, the bundle buffer is passed in from the network/CLA and all that is known will
 * be a pointer and size.  The first block is always supposed to be primary (per BP) and every block thereafter
 * can be assumed canonical.  Payload of the bundle will be recorded as an offset and size into that block, there
 * is no separate output.
 */
int v7_block_decode_pri(mpool_t *pool, mpool_cache_primary_block_t *cpb, const void *data_ptr, size_t data_size);
int v7_block_decode_canonical(mpool_t *pool, mpool_cache_canonical_block_t *ccb, const void *data_ptr,
                              size_t data_size);

/*
 * On the encode side of things, the block types are known ahead of time.  Encoding of a payload block is separate
 * because the data needs to be passed in, but for all other canonical block types all the information should already be
 * in the logical data - so nothing extra is needed (but this may change as more block types get implemented, too).
 * One possible option would be to pass in NULL/0 for the block types that do not have separate data, to keep the APIs
 * more consistent.
 */
int v7_block_encode_pri(mpool_t *pool, mpool_cache_primary_block_t *cpb);
int v7_block_encode_pay(mpool_t *pool, mpool_cache_canonical_block_t *ccb, const void *data_ptr, size_t data_size);
int v7_block_encode_canonical(mpool_t *pool, mpool_cache_canonical_block_t *ccb);

#endif
