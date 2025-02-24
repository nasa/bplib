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
 * Auto-Generated stub implementations for functions defined in bplib_rbtree header
 */

#include "bplib_rbt.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_RBT_ExtractNode()
 * ----------------------------------------------------
 */
int BPLib_RBT_ExtractNode(BPLib_RBT_Root_t *tree, BPLib_RBT_Link_t *link_block)
{
    UT_GenStub_SetupReturnBuffer(BPLib_RBT_ExtractNode, int);

    UT_GenStub_AddParam(BPLib_RBT_ExtractNode, BPLib_RBT_Root_t *, tree);
    UT_GenStub_AddParam(BPLib_RBT_ExtractNode, BPLib_RBT_Link_t *, link_block);

    UT_GenStub_Execute(BPLib_RBT_ExtractNode, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_RBT_ExtractNode, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_RBT_GetKeyValue()
 * ----------------------------------------------------
 */
BPLib_Val_t BPLib_RBT_GetKeyValue(const BPLib_RBT_Link_t *node)
{
    UT_GenStub_SetupReturnBuffer(BPLib_RBT_GetKeyValue, BPLib_Val_t);

    UT_GenStub_AddParam(BPLib_RBT_GetKeyValue, const BPLib_RBT_Link_t *, node);

    UT_GenStub_Execute(BPLib_RBT_GetKeyValue, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_RBT_GetKeyValue, BPLib_Val_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_RBT_InitRoot()
 * ----------------------------------------------------
 */
void BPLib_RBT_InitRoot(BPLib_RBT_Root_t *tree)
{
    UT_GenStub_AddParam(BPLib_RBT_InitRoot, BPLib_RBT_Root_t *, tree);

    UT_GenStub_Execute(BPLib_RBT_InitRoot, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_RBT_InsertValueGeneric()
 * ----------------------------------------------------
 */
int BPLib_RBT_InsertValueGeneric(BPLib_Val_t insert_key_value, BPLib_RBT_Root_t *tree, BPLib_RBT_Link_t *link_block,
                                 BPLib_RBT_CompareFunc_t compare_func, void *compare_arg)
{
    UT_GenStub_SetupReturnBuffer(BPLib_RBT_InsertValueGeneric, int);

    UT_GenStub_AddParam(BPLib_RBT_InsertValueGeneric, BPLib_Val_t, insert_key_value);
    UT_GenStub_AddParam(BPLib_RBT_InsertValueGeneric, BPLib_RBT_Root_t *, tree);
    UT_GenStub_AddParam(BPLib_RBT_InsertValueGeneric, BPLib_RBT_Link_t *, link_block);
    UT_GenStub_AddParam(BPLib_RBT_InsertValueGeneric, BPLib_RBT_CompareFunc_t, compare_func);
    UT_GenStub_AddParam(BPLib_RBT_InsertValueGeneric, void *, compare_arg);

    UT_GenStub_Execute(BPLib_RBT_InsertValueGeneric, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_RBT_InsertValueGeneric, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_RBT_IterGotoMax()
 * ----------------------------------------------------
 */
int BPLib_RBT_IterGotoMax(BPLib_Val_t maximum_value, const BPLib_RBT_Root_t *tree, BPLib_RBT_Iter_t *iter)
{
    UT_GenStub_SetupReturnBuffer(BPLib_RBT_IterGotoMax, int);

    UT_GenStub_AddParam(BPLib_RBT_IterGotoMax, BPLib_Val_t, maximum_value);
    UT_GenStub_AddParam(BPLib_RBT_IterGotoMax, const BPLib_RBT_Root_t *, tree);
    UT_GenStub_AddParam(BPLib_RBT_IterGotoMax, BPLib_RBT_Iter_t *, iter);

    UT_GenStub_Execute(BPLib_RBT_IterGotoMax, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_RBT_IterGotoMax, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_RBT_IterGotoMin()
 * ----------------------------------------------------
 */
int BPLib_RBT_IterGotoMin(BPLib_Val_t minimum_value, const BPLib_RBT_Root_t *tree, BPLib_RBT_Iter_t *iter)
{
    UT_GenStub_SetupReturnBuffer(BPLib_RBT_IterGotoMin, int);

    UT_GenStub_AddParam(BPLib_RBT_IterGotoMin, BPLib_Val_t, minimum_value);
    UT_GenStub_AddParam(BPLib_RBT_IterGotoMin, const BPLib_RBT_Root_t *, tree);
    UT_GenStub_AddParam(BPLib_RBT_IterGotoMin, BPLib_RBT_Iter_t *, iter);

    UT_GenStub_Execute(BPLib_RBT_IterGotoMin, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_RBT_IterGotoMin, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_RBT_IterNext()
 * ----------------------------------------------------
 */
int BPLib_RBT_IterNext(BPLib_RBT_Iter_t *iter)
{
    UT_GenStub_SetupReturnBuffer(BPLib_RBT_IterNext, int);

    UT_GenStub_AddParam(BPLib_RBT_IterNext, BPLib_RBT_Iter_t *, iter);

    UT_GenStub_Execute(BPLib_RBT_IterNext, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_RBT_IterNext, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_RBT_IterPrev()
 * ----------------------------------------------------
 */
int BPLib_RBT_IterPrev(BPLib_RBT_Iter_t *iter)
{
    UT_GenStub_SetupReturnBuffer(BPLib_RBT_IterPrev, int);

    UT_GenStub_AddParam(BPLib_RBT_IterPrev, BPLib_RBT_Iter_t *, iter);

    UT_GenStub_Execute(BPLib_RBT_IterPrev, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_RBT_IterPrev, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_RBT_NodeIsMember()
 * ----------------------------------------------------
 */
bool BPLib_RBT_NodeIsMember(const BPLib_RBT_Root_t *tree, const BPLib_RBT_Link_t *node)
{
    UT_GenStub_SetupReturnBuffer(BPLib_RBT_NodeIsMember, bool);

    UT_GenStub_AddParam(BPLib_RBT_NodeIsMember, const BPLib_RBT_Root_t *, tree);
    UT_GenStub_AddParam(BPLib_RBT_NodeIsMember, const BPLib_RBT_Link_t *, node);

    UT_GenStub_Execute(BPLib_RBT_NodeIsMember, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_RBT_NodeIsMember, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_RBT_NodeIsRed()
 * ----------------------------------------------------
 */
bool BPLib_RBT_NodeIsRed(const BPLib_RBT_Link_t *node)
{
    UT_GenStub_SetupReturnBuffer(BPLib_RBT_NodeIsRed, bool);

    UT_GenStub_AddParam(BPLib_RBT_NodeIsRed, const BPLib_RBT_Link_t *, node);

    UT_GenStub_Execute(BPLib_RBT_NodeIsRed, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_RBT_NodeIsRed, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_RBT_SearchGeneric()
 * ----------------------------------------------------
 */
BPLib_RBT_Link_t *BPLib_RBT_SearchGeneric(BPLib_Val_t search_key_value, const BPLib_RBT_Root_t *tree,
                                          BPLib_RBT_CompareFunc_t compare_func, void *compare_arg)
{
    UT_GenStub_SetupReturnBuffer(BPLib_RBT_SearchGeneric, BPLib_RBT_Link_t *);

    UT_GenStub_AddParam(BPLib_RBT_SearchGeneric, BPLib_Val_t, search_key_value);
    UT_GenStub_AddParam(BPLib_RBT_SearchGeneric, const BPLib_RBT_Root_t *, tree);
    UT_GenStub_AddParam(BPLib_RBT_SearchGeneric, BPLib_RBT_CompareFunc_t, compare_func);
    UT_GenStub_AddParam(BPLib_RBT_SearchGeneric, void *, compare_arg);

    UT_GenStub_Execute(BPLib_RBT_SearchGeneric, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_RBT_SearchGeneric, BPLib_RBT_Link_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_RBT_TreeIsEmpty()
 * ----------------------------------------------------
 */
bool BPLib_RBT_TreeIsEmpty(const BPLib_RBT_Root_t *tree)
{
    UT_GenStub_SetupReturnBuffer(BPLib_RBT_TreeIsEmpty, bool);

    UT_GenStub_AddParam(BPLib_RBT_TreeIsEmpty, const BPLib_RBT_Root_t *, tree);

    UT_GenStub_Execute(BPLib_RBT_TreeIsEmpty, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_RBT_TreeIsEmpty, bool);
}
