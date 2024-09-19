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
 * Auto-Generated stub implementations for functions defined in bplib_mem header
 */

#include "bplib_mem.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_BlockFromExternalId()
 * Generated stub function for BPLib_MEM_BlockFromExternalId()
 * ----------------------------------------------------
 */
BPLib_MEM_Block_t *BPLib_MEM_BlockFromExternalId(BPLib_MEM_Pool_t *pool, bp_handle_t handle)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_BlockFromExternalId, BPLib_MEM_Block_t *);

    UT_GenStub_AddParam(BPLib_MEM_BlockFromExternalId, BPLib_MEM_Pool_t *, pool);
    UT_GenStub_AddParam(BPLib_MEM_BlockFromExternalId, bp_handle_t, handle);

    UT_GenStub_Execute(BPLib_MEM_BlockFromExternalId, Basic, NULL);
    UT_GenStub_Execute(BPLib_MEM_BlockFromExternalId, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_BlockFromExternalId, BPLib_MEM_Block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_Create()
 * ----------------------------------------------------
 */
BPLib_MEM_Pool_t *BPLib_MEM_Create(void *pool_mem, size_t pool_size)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_Create, BPLib_MEM_Pool_t *);

    UT_GenStub_AddParam(BPLib_MEM_Create, void *, pool_mem);
    UT_GenStub_AddParam(BPLib_MEM_Create, size_t, pool_size);

    UT_GenStub_Execute(BPLib_MEM_Create, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_Create, BPLib_MEM_Pool_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_DebugPrintListStats()
 * Generated stub function for BPLib_MEM_DebugPrintListStats()
 * ----------------------------------------------------
 */
void BPLib_MEM_DebugPrintListStats(BPLib_MEM_Block_t *list, const char *label)
{
    UT_GenStub_AddParam(BPLib_MEM_DebugPrintListStats, BPLib_MEM_Block_t *, list);
    UT_GenStub_AddParam(BPLib_MEM_DebugPrintListStats, const char *, label);

    UT_GenStub_Execute(BPLib_MEM_DebugPrintListStats, Basic, NULL);
    UT_GenStub_Execute(BPLib_MEM_DebugPrintListStats, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_DebugScan()
 * Generated stub function for BPLib_MEM_DebugScan()
 * ----------------------------------------------------
 */
void BPLib_MEM_DebugScan(BPLib_MEM_Pool_t *pool)
{
    UT_GenStub_AddParam(BPLib_MEM_DebugScan, BPLib_MEM_Pool_t *, pool);

    UT_GenStub_Execute(BPLib_MEM_DebugScan, Basic, NULL);
    UT_GenStub_Execute(BPLib_MEM_DebugScan, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_ExtractNode()
 * Generated stub function for BPLib_MEM_ExtractNode()
 * ----------------------------------------------------
 */
void BPLib_MEM_ExtractNode(BPLib_MEM_Block_t *node)
{
    UT_GenStub_AddParam(BPLib_MEM_ExtractNode, BPLib_MEM_Block_t *, node);

    UT_GenStub_Execute(BPLib_MEM_ExtractNode, Basic, NULL);
    UT_GenStub_Execute(BPLib_MEM_ExtractNode, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_ForeachItemInList()
 * Generated stub function for BPLib_MEM_ForeachItemInList()
 * ----------------------------------------------------
 */
int BPLib_MEM_ForeachItemInList(BPLib_MEM_Block_t *list, bool always_remove,
                                     BPLib_MEM_CallbackFunc_t callback_fn, void *callback_arg)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_ForeachItemInList, int);
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_ForeachItemInList, int);

    UT_GenStub_AddParam(BPLib_MEM_ForeachItemInList, BPLib_MEM_Block_t *, list);
    UT_GenStub_AddParam(BPLib_MEM_ForeachItemInList, bool, always_remove);
    UT_GenStub_AddParam(BPLib_MEM_ForeachItemInList, BPLib_MEM_CallbackFunc_t, callback_fn);
    UT_GenStub_AddParam(BPLib_MEM_ForeachItemInList, void *, callback_arg);

    UT_GenStub_Execute(BPLib_MEM_ForeachItemInList, Basic, NULL);
    UT_GenStub_Execute(BPLib_MEM_ForeachItemInList, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_ForeachItemInList, int);
    return UT_GenStub_GetReturnValue(BPLib_MEM_ForeachItemInList, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_GenericDataAlloc()
 * Generated stub function for BPLib_MEM_GenericDataAlloc()
 * ----------------------------------------------------
 */
BPLib_MEM_Block_t *BPLib_MEM_GenericDataAlloc(BPLib_MEM_Pool_t *pool, uint32_t magic_number, void *init_arg)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_GenericDataAlloc, BPLib_MEM_Block_t *);

    UT_GenStub_AddParam(BPLib_MEM_GenericDataAlloc, BPLib_MEM_Pool_t *, pool);
    UT_GenStub_AddParam(BPLib_MEM_GenericDataAlloc, uint32_t, magic_number);
    UT_GenStub_AddParam(BPLib_MEM_GenericDataAlloc, void *, init_arg);

    UT_GenStub_Execute(BPLib_MEM_GenericDataAlloc, Basic, NULL);
    UT_GenStub_Execute(BPLib_MEM_GenericDataAlloc, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_GenericDataAlloc, BPLib_MEM_Block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_GenericDataCast()
 * Generated stub function for BPLib_MEM_GenericDataCast()
 * ----------------------------------------------------
 */
void *BPLib_MEM_GenericDataCast(BPLib_MEM_Block_t *cb, uint32_t required_magic)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_GenericDataCast, void *);
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_GenericDataCast, void *);

    UT_GenStub_AddParam(BPLib_MEM_GenericDataCast, BPLib_MEM_Block_t *, cb);
    UT_GenStub_AddParam(BPLib_MEM_GenericDataCast, uint32_t, required_magic);

    UT_GenStub_Execute(BPLib_MEM_GenericDataCast, Basic, NULL);
    UT_GenStub_Execute(BPLib_MEM_GenericDataCast, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_GenericDataCast, void *);
    return UT_GenStub_GetReturnValue(BPLib_MEM_GenericDataCast, void *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_GenericDataUncast()
 * Generated stub function for BPLib_MEM_GenericDataUncast()
 * ----------------------------------------------------
 */
BPLib_MEM_Block_t *BPLib_MEM_GenericDataUncast(void *blk, BPLib_MEM_Blocktype_t parent_bt,
                                                     uint32_t required_magic)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_GenericDataUncast, BPLib_MEM_Block_t *);

    UT_GenStub_AddParam(BPLib_MEM_GenericDataUncast, void *, blk);
    UT_GenStub_AddParam(BPLib_MEM_GenericDataUncast, BPLib_MEM_Blocktype_t, parent_bt);
    UT_GenStub_AddParam(BPLib_MEM_GenericDataUncast, uint32_t, required_magic);

    UT_GenStub_Execute(BPLib_MEM_GenericDataUncast, Basic, NULL);
    UT_GenStub_Execute(BPLib_MEM_GenericDataUncast, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_GenericDataUncast, BPLib_MEM_Block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_GetBlockFromLink()
 * Generated stub function for BPLib_MEM_GetBlockFromLink()
 * ----------------------------------------------------
 */
BPLib_MEM_Block_t *BPLib_MEM_GetBlockFromLink(BPLib_MEM_Block_t *lblk)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_GetBlockFromLink, BPLib_MEM_Block_t *);

    UT_GenStub_AddParam(BPLib_MEM_GetBlockFromLink, BPLib_MEM_Block_t *, lblk);

    UT_GenStub_Execute(BPLib_MEM_GetBlockFromLink, Basic, NULL);
    UT_GenStub_Execute(BPLib_MEM_GetBlockFromLink, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_GetBlockFromLink, BPLib_MEM_Block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_GetGenericDataCapacity()
 * Generated stub function for BPLib_MEM_GetGenericDataCapacity()
 * ----------------------------------------------------
 */
size_t BPLib_MEM_GetGenericDataCapacity(const BPLib_MEM_Block_t *cb)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_GetGenericDataCapacity, size_t);
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_GetGenericDataCapacity, size_t);

    UT_GenStub_AddParam(BPLib_MEM_GetGenericDataCapacity, const BPLib_MEM_Block_t *, cb);

    UT_GenStub_Execute(BPLib_MEM_GetGenericDataCapacity, Basic, NULL);
    UT_GenStub_Execute(BPLib_MEM_GetGenericDataCapacity, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_GetGenericDataCapacity, size_t);
    return UT_GenStub_GetReturnValue(BPLib_MEM_GetGenericDataCapacity, size_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_GetParentPoolFromLink()
 * Generated stub function for BPLib_MEM_GetParentPoolFromLink()
 * ----------------------------------------------------
 */
BPLib_MEM_Pool_t *BPLib_MEM_GetParentPoolFromLink(BPLib_MEM_Block_t *cb)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_GetParentPoolFromLink, BPLib_MEM_Pool_t *);

    UT_GenStub_AddParam(BPLib_MEM_GetParentPoolFromLink, BPLib_MEM_Block_t *, cb);

    UT_GenStub_Execute(BPLib_MEM_GetParentPoolFromLink, Basic, NULL);
    UT_GenStub_Execute(BPLib_MEM_GetParentPoolFromLink, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_GetParentPoolFromLink, BPLib_MEM_Pool_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_GetUserContentSize()
 * Generated stub function for BPLib_MEM_GetUserContentSize()
 * ----------------------------------------------------
 */
size_t BPLib_MEM_GetUserContentSize(const BPLib_MEM_Block_t *ceb)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_GetUserContentSize, size_t);
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_GetUserContentSize, size_t);

    UT_GenStub_AddParam(BPLib_MEM_GetUserContentSize, const BPLib_MEM_Block_t *, ceb);

    UT_GenStub_Execute(BPLib_MEM_GetUserContentSize, Basic, NULL);
    UT_GenStub_Execute(BPLib_MEM_GetUserContentSize, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_GetUserContentSize, size_t);
    return UT_GenStub_GetReturnValue(BPLib_MEM_GetUserContentSize, size_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_InitListHead()
 * Generated stub function for BPLib_MEM_InitListHead()
 * ----------------------------------------------------
 */
void BPLib_MEM_InitListHead(BPLib_MEM_Block_t *base_block, BPLib_MEM_Block_t *list_head)
{
    UT_GenStub_AddParam(BPLib_MEM_InitListHead, BPLib_MEM_Block_t *, base_block);
    UT_GenStub_AddParam(BPLib_MEM_InitListHead, BPLib_MEM_Block_t *, list_head);

    UT_GenStub_Execute(BPLib_MEM_InitListHead, Basic, NULL);
    UT_GenStub_Execute(BPLib_MEM_InitListHead, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_InitSecondaryLink()
 * Generated stub function for BPLib_MEM_InitSecondaryLink()
 * ----------------------------------------------------
 */
void BPLib_MEM_InitSecondaryLink(BPLib_MEM_Block_t *base_block, BPLib_MEM_Block_t *secondary_link,
                                     BPLib_MEM_Blocktype_t block_type)
{
    UT_GenStub_AddParam(BPLib_MEM_InitSecondaryLink, BPLib_MEM_Block_t *, base_block);
    UT_GenStub_AddParam(BPLib_MEM_InitSecondaryLink, BPLib_MEM_Block_t *, secondary_link);
    UT_GenStub_AddParam(BPLib_MEM_InitSecondaryLink, BPLib_MEM_Blocktype_t, block_type);

    UT_GenStub_Execute(BPLib_MEM_InitSecondaryLink, Basic, NULL);
    UT_GenStub_Execute(BPLib_MEM_InitSecondaryLink, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_InsertAfter()
 * Generated stub function for BPLib_MEM_InsertAfter()
 * ----------------------------------------------------
 */
void BPLib_MEM_InsertAfter(BPLib_MEM_Block_t *list, BPLib_MEM_Block_t *node)
{
    UT_GenStub_AddParam(BPLib_MEM_InsertAfter, BPLib_MEM_Block_t *, list);
    UT_GenStub_AddParam(BPLib_MEM_InsertAfter, BPLib_MEM_Block_t *, node);

    UT_GenStub_Execute(BPLib_MEM_InsertAfter, Basic, NULL);
    UT_GenStub_Execute(BPLib_MEM_InsertAfter, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_InsertBefore()
 * Generated stub function for BPLib_MEM_InsertBefore()
 * ----------------------------------------------------
 */
void BPLib_MEM_InsertBefore(BPLib_MEM_Block_t *list, BPLib_MEM_Block_t *node)
{
    UT_GenStub_AddParam(BPLib_MEM_InsertBefore, BPLib_MEM_Block_t *, list);
    UT_GenStub_AddParam(BPLib_MEM_InsertBefore, BPLib_MEM_Block_t *, node);

    UT_GenStub_Execute(BPLib_MEM_InsertBefore, Basic, NULL);
    UT_GenStub_Execute(BPLib_MEM_InsertBefore, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_ListIterForward()
 * Generated stub function for BPLib_MEM_ListIterForward()
 * ----------------------------------------------------
 */
int BPLib_MEM_ListIterForward(BPLib_MEM_ListIter_t *iter)
int BPLib_MEM_ListIterForward(BPLib_MEM_ListIter_t *iter)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_ListIterForward, int);
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_ListIterForward, int);

    UT_GenStub_AddParam(BPLib_MEM_ListIterForward, BPLib_MEM_ListIter_t *, iter);
    UT_GenStub_AddParam(BPLib_MEM_ListIterForward, BPLib_MEM_ListIter_t *, iter);

    UT_GenStub_Execute(BPLib_MEM_ListIterForward, Basic, NULL);
    UT_GenStub_Execute(BPLib_MEM_ListIterForward, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_ListIterForward, int);
    return UT_GenStub_GetReturnValue(BPLib_MEM_ListIterForward, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_ListIterGotoFirst()
 * Generated stub function for BPLib_MEM_ListIterGotoFirst()
 * ----------------------------------------------------
 */
int BPLib_MEM_ListIterGotoFirst(const BPLib_MEM_Block_t *list, BPLib_MEM_ListIter_t *iter)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_ListIterGotoFirst, int);
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_ListIterGotoFirst, int);

    UT_GenStub_AddParam(BPLib_MEM_ListIterGotoFirst, const BPLib_MEM_Block_t *, list);
    UT_GenStub_AddParam(BPLib_MEM_ListIterGotoFirst, BPLib_MEM_ListIter_t *, iter);

    UT_GenStub_Execute(BPLib_MEM_ListIterGotoFirst, Basic, NULL);
    UT_GenStub_Execute(BPLib_MEM_ListIterGotoFirst, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_ListIterGotoFirst, int);
    return UT_GenStub_GetReturnValue(BPLib_MEM_ListIterGotoFirst, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_ListIterGotoLast()
 * Generated stub function for BPLib_MEM_ListIterGotoLast()
 * ----------------------------------------------------
 */
int BPLib_MEM_ListIterGotoLast(const BPLib_MEM_Block_t *list, BPLib_MEM_ListIter_t *iter)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_ListIterGotoLast, int);
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_ListIterGotoLast, int);

    UT_GenStub_AddParam(BPLib_MEM_ListIterGotoLast, const BPLib_MEM_Block_t *, list);
    UT_GenStub_AddParam(BPLib_MEM_ListIterGotoLast, BPLib_MEM_ListIter_t *, iter);

    UT_GenStub_Execute(BPLib_MEM_ListIterGotoLast, Basic, NULL);
    UT_GenStub_Execute(BPLib_MEM_ListIterGotoLast, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_ListIterGotoLast, int);
    return UT_GenStub_GetReturnValue(BPLib_MEM_ListIterGotoLast, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_ListIterReverse()
 * Generated stub function for BPLib_MEM_ListIterReverse()
 * ----------------------------------------------------
 */
int BPLib_MEM_ListIterReverse(BPLib_MEM_ListIter_t *iter)
int BPLib_MEM_ListIterReverse(BPLib_MEM_ListIter_t *iter)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_ListIterReverse, int);
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_ListIterReverse, int);

    UT_GenStub_AddParam(BPLib_MEM_ListIterReverse, BPLib_MEM_ListIter_t *, iter);
    UT_GenStub_AddParam(BPLib_MEM_ListIterReverse, BPLib_MEM_ListIter_t *, iter);

    UT_GenStub_Execute(BPLib_MEM_ListIterReverse, Basic, NULL);
    UT_GenStub_Execute(BPLib_MEM_ListIterReverse, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_ListIterReverse, int);
    return UT_GenStub_GetReturnValue(BPLib_MEM_ListIterReverse, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_Maintain()
 * ----------------------------------------------------
 */
void BPLib_MEM_Maintain(BPLib_MEM_Pool_t *pool)
{
    UT_GenStub_AddParam(BPLib_MEM_Maintain, BPLib_MEM_Pool_t *, pool);

    UT_GenStub_Execute(BPLib_MEM_Maintain, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_MergeList()
 * Generated stub function for BPLib_MEM_MergeList()
 * ----------------------------------------------------
 */
void BPLib_MEM_MergeList(BPLib_MEM_Block_t *dest, BPLib_MEM_Block_t *src)
{
    UT_GenStub_AddParam(BPLib_MEM_MergeList, BPLib_MEM_Block_t *, dest);
    UT_GenStub_AddParam(BPLib_MEM_MergeList, BPLib_MEM_Block_t *, src);

    UT_GenStub_Execute(BPLib_MEM_MergeList, Basic, NULL);
    UT_GenStub_Execute(BPLib_MEM_MergeList, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_QueryMemCurrentUse()
 * Generated stub function for BPLib_MEM_QueryMemCurrentUse()
 * ----------------------------------------------------
 */
size_t BPLib_MEM_QueryMemCurrentUse(BPLib_MEM_Pool_t *pool)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_QueryMemCurrentUse, size_t);
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_QueryMemCurrentUse, size_t);

    UT_GenStub_AddParam(BPLib_MEM_QueryMemCurrentUse, BPLib_MEM_Pool_t *, pool);

    UT_GenStub_Execute(BPLib_MEM_QueryMemCurrentUse, Basic, NULL);
    UT_GenStub_Execute(BPLib_MEM_QueryMemCurrentUse, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_QueryMemCurrentUse, size_t);
    return UT_GenStub_GetReturnValue(BPLib_MEM_QueryMemCurrentUse, size_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_QueryMemMaxUse()
 * Generated stub function for BPLib_MEM_QueryMemMaxUse()
 * ----------------------------------------------------
 */
size_t BPLib_MEM_QueryMemMaxUse(BPLib_MEM_Pool_t *pool)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_QueryMemMaxUse, size_t);
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_QueryMemMaxUse, size_t);

    UT_GenStub_AddParam(BPLib_MEM_QueryMemMaxUse, BPLib_MEM_Pool_t *, pool);

    UT_GenStub_Execute(BPLib_MEM_QueryMemMaxUse, Basic, NULL);
    UT_GenStub_Execute(BPLib_MEM_QueryMemMaxUse, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_QueryMemMaxUse, size_t);
    return UT_GenStub_GetReturnValue(BPLib_MEM_QueryMemMaxUse, size_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_ReadRefcount()
 * Generated stub function for BPLib_MEM_ReadRefcount()
 * ----------------------------------------------------
 */
size_t BPLib_MEM_ReadRefcount(const BPLib_MEM_Block_t *cb)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_ReadRefcount, size_t);
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_ReadRefcount, size_t);

    UT_GenStub_AddParam(BPLib_MEM_ReadRefcount, const BPLib_MEM_Block_t *, cb);

    UT_GenStub_Execute(BPLib_MEM_ReadRefcount, Basic, NULL);
    UT_GenStub_Execute(BPLib_MEM_ReadRefcount, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_ReadRefcount, size_t);
    return UT_GenStub_GetReturnValue(BPLib_MEM_ReadRefcount, size_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_RecycleAllBlocksInList()
 * Generated stub function for BPLib_MEM_RecycleAllBlocksInList()
 * ----------------------------------------------------
 */
void BPLib_MEM_RecycleAllBlocksInList(BPLib_MEM_Pool_t *pool, BPLib_MEM_Block_t *list)
{
    UT_GenStub_AddParam(BPLib_MEM_RecycleAllBlocksInList, BPLib_MEM_Pool_t *, pool);
    UT_GenStub_AddParam(BPLib_MEM_RecycleAllBlocksInList, BPLib_MEM_Block_t *, list);

    UT_GenStub_Execute(BPLib_MEM_RecycleAllBlocksInList, Basic, NULL);
    UT_GenStub_Execute(BPLib_MEM_RecycleAllBlocksInList, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_RecycleBlock()
 * Generated stub function for BPLib_MEM_RecycleBlock()
 * ----------------------------------------------------
 */
void BPLib_MEM_RecycleBlock(BPLib_MEM_Block_t *blk)
{
    UT_GenStub_AddParam(BPLib_MEM_RecycleBlock, BPLib_MEM_Block_t *, blk);

    UT_GenStub_Execute(BPLib_MEM_RecycleBlock, Basic, NULL);
    UT_GenStub_Execute(BPLib_MEM_RecycleBlock, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_RegisterBlocktype()
 * Generated stub function for BPLib_MEM_RegisterBlocktype()
 * ----------------------------------------------------
 */
int BPLib_MEM_RegisterBlocktype(BPLib_MEM_Pool_t *pool, uint32_t magic_number, const BPLib_MEM_BlocktypeApi_t *api,
                                   size_t user_content_size)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_RegisterBlocktype, int);
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_RegisterBlocktype, int);

    UT_GenStub_AddParam(BPLib_MEM_RegisterBlocktype, BPLib_MEM_Pool_t *, pool);
    UT_GenStub_AddParam(BPLib_MEM_RegisterBlocktype, uint32_t, magic_number);
    UT_GenStub_AddParam(BPLib_MEM_RegisterBlocktype, const BPLib_MEM_BlocktypeApi_t *, api);
    UT_GenStub_AddParam(BPLib_MEM_RegisterBlocktype, size_t, user_content_size);

    UT_GenStub_Execute(BPLib_MEM_RegisterBlocktype, Basic, NULL);
    UT_GenStub_Execute(BPLib_MEM_RegisterBlocktype, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_RegisterBlocktype, int);
    return UT_GenStub_GetReturnValue(BPLib_MEM_RegisterBlocktype, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_SearchList()
 * Generated stub function for BPLib_MEM_SearchList()
 * ----------------------------------------------------
 */
BPLib_MEM_Block_t *BPLib_MEM_SearchList(const BPLib_MEM_Block_t *list, BPLib_MEM_CallbackFunc_t match_fn,
                                             void *match_arg)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_SearchList, BPLib_MEM_Block_t *);

    UT_GenStub_AddParam(BPLib_MEM_SearchList, const BPLib_MEM_Block_t *, list);
    UT_GenStub_AddParam(BPLib_MEM_SearchList, BPLib_MEM_CallbackFunc_t, match_fn);
    UT_GenStub_AddParam(BPLib_MEM_SearchList, void *, match_arg);

    UT_GenStub_Execute(BPLib_MEM_SearchList, Basic, NULL);
    UT_GenStub_Execute(BPLib_MEM_SearchList, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_SearchList, BPLib_MEM_Block_t *);
}
