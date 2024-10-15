/*
 * NASA Docket No. GSC-18,587-1 and identified as “The Bundle Protocol Core Flight
 * System Application (BP) v6.5”
 *
 * Copyright © 2020 United States Government as represented by the Administrator of
 * the National Aeronautics and Space Administration. All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in v7_mpool header
 */

#include "utgenstub.h"

#include "bplib_stor_cache_block.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_BlockFromExternalId()
 * ----------------------------------------------------
 */
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_BlockFromExternalId(BPLib_STOR_CACHE_Pool_t *pool, bp_handle_t handle)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_BlockFromExternalId, BPLib_STOR_CACHE_Block_t *);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_BlockFromExternalId, BPLib_STOR_CACHE_Pool_t *, pool);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_BlockFromExternalId, bp_handle_t, handle);

    UT_GenStub_Execute(BPLib_STOR_CACHE_BlockFromExternalId, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_BlockFromExternalId, BPLib_STOR_CACHE_Block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_CollectBlocks()
 * ----------------------------------------------------
 */
uint32_t BPLib_STOR_CACHE_CollectBlocks(BPLib_STOR_CACHE_Pool_t *pool, uint32_t limit)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_CollectBlocks, uint32_t);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_CollectBlocks, BPLib_STOR_CACHE_Pool_t *, pool);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_CollectBlocks, uint32_t, limit);

    UT_GenStub_Execute(BPLib_STOR_CACHE_CollectBlocks, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_CollectBlocks, uint32_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_Create()
 * ----------------------------------------------------
 */
BPLib_STOR_CACHE_Pool_t *BPLib_STOR_CACHE_Create(void *pool_mem, size_t pool_size)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_Create, BPLib_STOR_CACHE_Pool_t *);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_Create, void *, pool_mem);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_Create, size_t, pool_size);

    UT_GenStub_Execute(BPLib_STOR_CACHE_Create, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_Create, BPLib_STOR_CACHE_Pool_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_DebugPrintListStats()
 * ----------------------------------------------------
 */
void BPLib_STOR_CACHE_DebugPrintListStats(BPLib_STOR_CACHE_Block_t *list, const char *label)
{
    UT_GenStub_AddParam(BPLib_STOR_CACHE_DebugPrintListStats, BPLib_STOR_CACHE_Block_t *, list);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_DebugPrintListStats, const char *, label);

    UT_GenStub_Execute(BPLib_STOR_CACHE_DebugPrintListStats, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_DebugScan()
 * ----------------------------------------------------
 */
void BPLib_STOR_CACHE_DebugScan(BPLib_STOR_CACHE_Pool_t *pool)
{
    UT_GenStub_AddParam(BPLib_STOR_CACHE_DebugScan, BPLib_STOR_CACHE_Pool_t *, pool);

    UT_GenStub_Execute(BPLib_STOR_CACHE_DebugScan, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_ExtractNode()
 * ----------------------------------------------------
 */
void BPLib_STOR_CACHE_ExtractNode(BPLib_STOR_CACHE_Block_t *node)
{
    UT_GenStub_AddParam(BPLib_STOR_CACHE_ExtractNode, BPLib_STOR_CACHE_Block_t *, node);

    UT_GenStub_Execute(BPLib_STOR_CACHE_ExtractNode, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_ForeachItemInList()
 * ----------------------------------------------------
 */
int BPLib_STOR_CACHE_ForeachItemInList(BPLib_STOR_CACHE_Block_t *list, bool always_remove,
                                     BPLib_STOR_CACHE_CallbackFunc_t callback_fn, void *callback_arg)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_ForeachItemInList, int);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_ForeachItemInList, BPLib_STOR_CACHE_Block_t *, list);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_ForeachItemInList, bool, always_remove);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_ForeachItemInList, BPLib_STOR_CACHE_CallbackFunc_t, callback_fn);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_ForeachItemInList, void *, callback_arg);

    UT_GenStub_Execute(BPLib_STOR_CACHE_ForeachItemInList, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_ForeachItemInList, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_GenericDataAlloc()
 * ----------------------------------------------------
 */
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_GenericDataAlloc(BPLib_STOR_CACHE_Pool_t *pool, uint32_t magic_number, void *init_arg)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_GenericDataAlloc, BPLib_STOR_CACHE_Block_t *);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_GenericDataAlloc, BPLib_STOR_CACHE_Pool_t *, pool);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_GenericDataAlloc, uint32_t, magic_number);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_GenericDataAlloc, void *, init_arg);

    UT_GenStub_Execute(BPLib_STOR_CACHE_GenericDataAlloc, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_GenericDataAlloc, BPLib_STOR_CACHE_Block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_GetBlockFromLink()
 * ----------------------------------------------------
 */
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_GetBlockFromLink(BPLib_STOR_CACHE_Block_t *lblk)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_GetBlockFromLink, BPLib_STOR_CACHE_Block_t *);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_GetBlockFromLink, BPLib_STOR_CACHE_Block_t *, lblk);

    UT_GenStub_Execute(BPLib_STOR_CACHE_GetBlockFromLink, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_GetBlockFromLink, BPLib_STOR_CACHE_Block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_GetGenericDataCapacity()
 * ----------------------------------------------------
 */
size_t BPLib_STOR_CACHE_GetGenericDataCapacity(const BPLib_STOR_CACHE_Block_t *cb)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_GetGenericDataCapacity, size_t);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_GetGenericDataCapacity, const BPLib_STOR_CACHE_Block_t *, cb);

    UT_GenStub_Execute(BPLib_STOR_CACHE_GetGenericDataCapacity, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_GetGenericDataCapacity, size_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_GetParentPoolFromLink()
 * ----------------------------------------------------
 */
BPLib_STOR_CACHE_Pool_t *BPLib_STOR_CACHE_GetParentPoolFromLink(BPLib_STOR_CACHE_Block_t *cb)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_GetParentPoolFromLink, BPLib_STOR_CACHE_Pool_t *);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_GetParentPoolFromLink, BPLib_STOR_CACHE_Block_t *, cb);

    UT_GenStub_Execute(BPLib_STOR_CACHE_GetParentPoolFromLink, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_GetParentPoolFromLink, BPLib_STOR_CACHE_Pool_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_GetUserContentSize()
 * ----------------------------------------------------
 */
size_t BPLib_STOR_CACHE_GetUserContentSize(const BPLib_STOR_CACHE_Block_t *cb)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_GetUserContentSize, size_t);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_GetUserContentSize, const BPLib_STOR_CACHE_Block_t *, cb);

    UT_GenStub_Execute(BPLib_STOR_CACHE_GetUserContentSize, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_GetUserContentSize, size_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_GetUserDataOffsetByBlocktype()
 * ----------------------------------------------------
 */
size_t BPLib_STOR_CACHE_GetUserDataOffsetByBlocktype(BPLib_STOR_CACHE_Blocktype_t bt)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_GetUserDataOffsetByBlocktype, size_t);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_GetUserDataOffsetByBlocktype, BPLib_STOR_CACHE_Blocktype_t, bt);

    UT_GenStub_Execute(BPLib_STOR_CACHE_GetUserDataOffsetByBlocktype, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_GetUserDataOffsetByBlocktype, size_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_InitListHead()
 * ----------------------------------------------------
 */
void BPLib_STOR_CACHE_InitListHead(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_Block_t *list_head)
{
    UT_GenStub_AddParam(BPLib_STOR_CACHE_InitListHead, BPLib_STOR_CACHE_Block_t *, base_block);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_InitListHead, BPLib_STOR_CACHE_Block_t *, list_head);

    UT_GenStub_Execute(BPLib_STOR_CACHE_InitListHead, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_InitSecondaryLink()
 * ----------------------------------------------------
 */
void BPLib_STOR_CACHE_InitSecondaryLink(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_Block_t *secondary_link,
                                     BPLib_STOR_CACHE_Blocktype_t block_type)
{
    UT_GenStub_AddParam(BPLib_STOR_CACHE_InitSecondaryLink, BPLib_STOR_CACHE_Block_t *, base_block);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_InitSecondaryLink, BPLib_STOR_CACHE_Block_t *, secondary_link);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_InitSecondaryLink, BPLib_STOR_CACHE_Blocktype_t, block_type);

    UT_GenStub_Execute(BPLib_STOR_CACHE_InitSecondaryLink, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_InsertAfter()
 * ----------------------------------------------------
 */
void BPLib_STOR_CACHE_InsertAfter(BPLib_STOR_CACHE_Block_t *list, BPLib_STOR_CACHE_Block_t *node)
{
    UT_GenStub_AddParam(BPLib_STOR_CACHE_InsertAfter, BPLib_STOR_CACHE_Block_t *, list);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_InsertAfter, BPLib_STOR_CACHE_Block_t *, node);

    UT_GenStub_Execute(BPLib_STOR_CACHE_InsertAfter, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_InsertBefore()
 * ----------------------------------------------------
 */
void BPLib_STOR_CACHE_InsertBefore(BPLib_STOR_CACHE_Block_t *list, BPLib_STOR_CACHE_Block_t *node)
{
    UT_GenStub_AddParam(BPLib_STOR_CACHE_InsertBefore, BPLib_STOR_CACHE_Block_t *, list);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_InsertBefore, BPLib_STOR_CACHE_Block_t *, node);

    UT_GenStub_Execute(BPLib_STOR_CACHE_InsertBefore, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_ListIterForward()
 * ----------------------------------------------------
 */
int BPLib_STOR_CACHE_ListIterForward(BPLib_STOR_CACHE_ListIter_t *iter)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_ListIterForward, int);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_ListIterForward, BPLib_STOR_CACHE_ListIter_t *, iter);

    UT_GenStub_Execute(BPLib_STOR_CACHE_ListIterForward, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_ListIterForward, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_ListIterGotoFirst()
 * ----------------------------------------------------
 */
int BPLib_STOR_CACHE_ListIterGotoFirst(const BPLib_STOR_CACHE_Block_t *list, BPLib_STOR_CACHE_ListIter_t *iter)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_ListIterGotoFirst, int);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_ListIterGotoFirst, const BPLib_STOR_CACHE_Block_t *, list);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_ListIterGotoFirst, BPLib_STOR_CACHE_ListIter_t *, iter);

    UT_GenStub_Execute(BPLib_STOR_CACHE_ListIterGotoFirst, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_ListIterGotoFirst, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_ListIterGotoLast()
 * ----------------------------------------------------
 */
int BPLib_STOR_CACHE_ListIterGotoLast(const BPLib_STOR_CACHE_Block_t *list, BPLib_STOR_CACHE_ListIter_t *iter)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_ListIterGotoLast, int);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_ListIterGotoLast, const BPLib_STOR_CACHE_Block_t *, list);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_ListIterGotoLast, BPLib_STOR_CACHE_ListIter_t *, iter);

    UT_GenStub_Execute(BPLib_STOR_CACHE_ListIterGotoLast, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_ListIterGotoLast, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_ListIterReverse()
 * ----------------------------------------------------
 */
int BPLib_STOR_CACHE_ListIterReverse(BPLib_STOR_CACHE_ListIter_t *iter)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_ListIterReverse, int);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_ListIterReverse, BPLib_STOR_CACHE_ListIter_t *, iter);

    UT_GenStub_Execute(BPLib_STOR_CACHE_ListIterReverse, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_ListIterReverse, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_LockInit()
 * ----------------------------------------------------
 */
void BPLib_STOR_CACHE_LockInit(void)
{

    UT_GenStub_Execute(BPLib_STOR_CACHE_LockInit, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_Maintain()
 * ----------------------------------------------------
 */
void BPLib_STOR_CACHE_Maintain(BPLib_STOR_CACHE_Pool_t *pool)
{
    UT_GenStub_AddParam(BPLib_STOR_CACHE_Maintain, BPLib_STOR_CACHE_Pool_t *, pool);

    UT_GenStub_Execute(BPLib_STOR_CACHE_Maintain, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_MergeList()
 * ----------------------------------------------------
 */
void BPLib_STOR_CACHE_MergeList(BPLib_STOR_CACHE_Block_t *dest, BPLib_STOR_CACHE_Block_t *src)
{
    UT_GenStub_AddParam(BPLib_STOR_CACHE_MergeList, BPLib_STOR_CACHE_Block_t *, dest);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_MergeList, BPLib_STOR_CACHE_Block_t *, src);

    UT_GenStub_Execute(BPLib_STOR_CACHE_MergeList, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_QueryMemCurrentUse()
 * ----------------------------------------------------
 */
size_t BPLib_STOR_CACHE_QueryMemCurrentUse(BPLib_STOR_CACHE_Pool_t *pool)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_QueryMemCurrentUse, size_t);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_QueryMemCurrentUse, BPLib_STOR_CACHE_Pool_t *, pool);

    UT_GenStub_Execute(BPLib_STOR_CACHE_QueryMemCurrentUse, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_QueryMemCurrentUse, size_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_QueryMemMaxUse()
 * ----------------------------------------------------
 */
size_t BPLib_STOR_CACHE_QueryMemMaxUse(BPLib_STOR_CACHE_Pool_t *pool)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_QueryMemMaxUse, size_t);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_QueryMemMaxUse, BPLib_STOR_CACHE_Pool_t *, pool);

    UT_GenStub_Execute(BPLib_STOR_CACHE_QueryMemMaxUse, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_QueryMemMaxUse, size_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_ReadRefcount()
 * ----------------------------------------------------
 */
size_t BPLib_STOR_CACHE_ReadRefcount(const BPLib_STOR_CACHE_Block_t *cb)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_ReadRefcount, size_t);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_ReadRefcount, const BPLib_STOR_CACHE_Block_t *, cb);

    UT_GenStub_Execute(BPLib_STOR_CACHE_ReadRefcount, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_ReadRefcount, size_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_RecycleAllBlocksInList()
 * ----------------------------------------------------
 */
void BPLib_STOR_CACHE_RecycleAllBlocksInList(BPLib_STOR_CACHE_Pool_t *pool, BPLib_STOR_CACHE_Block_t *list)
{
    UT_GenStub_AddParam(BPLib_STOR_CACHE_RecycleAllBlocksInList, BPLib_STOR_CACHE_Pool_t *, pool);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_RecycleAllBlocksInList, BPLib_STOR_CACHE_Block_t *, list);

    UT_GenStub_Execute(BPLib_STOR_CACHE_RecycleAllBlocksInList, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_RecycleBlock()
 * ----------------------------------------------------
 */
void BPLib_STOR_CACHE_RecycleBlock(BPLib_STOR_CACHE_Block_t *blk)
{
    UT_GenStub_AddParam(BPLib_STOR_CACHE_RecycleBlock, BPLib_STOR_CACHE_Block_t *, blk);

    UT_GenStub_Execute(BPLib_STOR_CACHE_RecycleBlock, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_RegisterBlocktype()
 * ----------------------------------------------------
 */
int BPLib_STOR_CACHE_RegisterBlocktype(BPLib_STOR_CACHE_Pool_t *pool, uint32_t magic_number, const BPLib_STOR_CACHE_BlocktypeApi_t *api,
                                   size_t user_content_size)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_RegisterBlocktype, int);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_RegisterBlocktype, BPLib_STOR_CACHE_Pool_t *, pool);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_RegisterBlocktype, uint32_t, magic_number);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_RegisterBlocktype, const BPLib_STOR_CACHE_BlocktypeApi_t *, api);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_RegisterBlocktype, size_t, user_content_size);

    UT_GenStub_Execute(BPLib_STOR_CACHE_RegisterBlocktype, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_RegisterBlocktype, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_SearchList()
 * ----------------------------------------------------
 */
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_SearchList(const BPLib_STOR_CACHE_Block_t *list, BPLib_STOR_CACHE_CallbackFunc_t match_fn,
                                             void *match_arg)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_SearchList, BPLib_STOR_CACHE_Block_t *);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_SearchList, const BPLib_STOR_CACHE_Block_t *, list);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_SearchList, BPLib_STOR_CACHE_CallbackFunc_t, match_fn);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_SearchList, void *, match_arg);

    UT_GenStub_Execute(BPLib_STOR_CACHE_SearchList, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_SearchList, BPLib_STOR_CACHE_Block_t *);
}
