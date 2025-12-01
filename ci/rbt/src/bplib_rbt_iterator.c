/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking 
 * (DTN) Bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this 
 * file except in compliance with the License. You may obtain a copy of the License at 
 *
 * http://www.apache.org/licenses/LICENSE-2.0 
 *
 * Unless required by applicable law or agreed to in writing, software distributed under 
 * the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF 
 * ANY KIND, either express or implied. See the License for the specific language 
 * governing permissions and limitations under the License. The copyright notice to be 
 * included in the software is as follows: 
 *
 * Copyright 2025 United States Government as represented by the Administrator of the 
 * National Aeronautics and Space Administration. All Rights Reserved.
 *
 */

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_rbt_impl.h"

/*--------------------------------------------------------------------------------------
 * BPLib_RBT_IterFindClosestImpl
 *
 * Find a node that is either exactly matching the target value, or if that does not exist,
 * returns the nearest leaf node where that value would be attached.
 *
 * This is a local helper routine and not exposed in the API
 *--------------------------------------------------------------------------------------*/
const BPLib_RBT_Link_t *BPLib_RBT_IterFindClosestImpl(BPLib_Val_t target_value, const BPLib_RBT_Link_t *start_pos)
{
    const BPLib_RBT_Link_t *prev_pos;
    const BPLib_RBT_Link_t *curr_pos;
    BPLib_Val_t             curr_val;

    prev_pos = NULL;
    curr_pos = start_pos;
    while (true)
    {
        if (curr_pos == NULL)
        {
            /* end of tree - go back to last valid node */
            curr_pos = prev_pos;
            break;
        }

        curr_val = BPLib_RBT_GetKeyValueImpl(curr_pos);
        if (curr_val == target_value)
        {
            /* exact match */
            break;
        }

        prev_pos = curr_pos;
        if (curr_val > target_value)
        {
            curr_pos = curr_pos->left;
        }
        else
        {
            curr_pos = curr_pos->right;
        }
    }

    return curr_pos;
}

/***************************************************************************************
 *
 * EXTERNAL ROUTINES (part of public API, called externally)
 *
 ***************************************************************************************/

/*--------------------------------------------------------------------------------------
 * BPLib_RBT_IterNext
 *
 * Move to the next node in an iterator
 *--------------------------------------------------------------------------------------*/
BPLib_Status_t BPLib_RBT_IterNext(BPLib_RBT_Iter_t *iter)
{
    const BPLib_RBT_Link_t *next_pos;

    next_pos = iter->position;
    if (next_pos != NULL)
    {
        if (next_pos->right != NULL)
        {
            /* Descend the right-side tree at this time. */
            next_pos = next_pos->right;
            while (next_pos->left != NULL)
            {
                next_pos = next_pos->left;
            }
        }
        else
        {
            while (true)
            {
                /* Ascend to the parent, until arriving at an unvisited node */
                next_pos = next_pos->parent;

                /* if returning from the left-side child, visit this node now.
                 * otherwise, this is returning from the right side child, so keep going up. */
                if (next_pos == NULL || next_pos->left == iter->position)
                {
                    break;
                }
                iter->position = next_pos;
            }
        }
    }

    iter->position = next_pos;
    if (next_pos == NULL)
    {
        /* reached end of tree */
        return BPLIB_ERROR;
    }

    return BPLIB_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * BPLib_RBT_IterPrev
 *
 * Move to the previous node in an iterator
 *--------------------------------------------------------------------------------------*/
BPLib_Status_t BPLib_RBT_IterPrev(BPLib_RBT_Iter_t *iter)
{
    const BPLib_RBT_Link_t *prev_pos;

    prev_pos = iter->position;
    if (prev_pos != NULL)
    {
        if (prev_pos->left != NULL)
        {
            /* Descend the left-side tree at this time. */
            prev_pos = prev_pos->left;
            while (prev_pos->right != NULL)
            {
                prev_pos = prev_pos->right;
            }
        }
        else
        {
            while (true)
            {
                /* Ascend to the parent, until arriving at an unvisited node */
                prev_pos = prev_pos->parent;

                /* if returning from the right-side child, visit this node now.
                 * otherwise, this is returning from the left side child, so keep going up. */
                if (prev_pos == NULL || prev_pos->right == iter->position)
                {
                    break;
                }
                iter->position = prev_pos;
            }
        }
    }

    iter->position = prev_pos;
    if (prev_pos == NULL)
    {
        /* reached end of tree */
        return BPLIB_ERROR;
    }

    return BPLIB_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * BPLib_RBT_IterGotoMin
 *
 * Position the iterator at (or closest to) the minimum value indicated
 *--------------------------------------------------------------------------------------*/
BPLib_Status_t BPLib_RBT_IterGotoMin(BPLib_Val_t minimum_value, const BPLib_RBT_Root_t *tree, BPLib_RBT_Iter_t *iter)
{
    BPLib_Val_t curr_val;
    int         status;

    iter->position = BPLib_RBT_IterFindClosestImpl(minimum_value, tree->root);
    if (iter->position == NULL)
    {
        /* the tree is empty */
        status = BPLIB_ERROR;
    }
    else
    {
        /*
         * If duplicates are allowed in this tree, then this may not be the only match.
         * So if this is not a leaf node then go left and repeat the search until arriving
         * at a node that has no left child.
         */
        while (iter->position->left != NULL)
        {
            iter->position = BPLib_RBT_IterFindClosestImpl(minimum_value, iter->position->left);
        }

        curr_val = BPLib_RBT_GetKeyValueImpl(iter->position);
        if (curr_val >= minimum_value)
        {
            status = BPLIB_SUCCESS;
        }
        else
        {
            /* the next node in normal iteration may be greater than the minimum */
            status = BPLib_RBT_IterNext(iter);
        }
    }

    return status;
}

/*--------------------------------------------------------------------------------------
 * BPLib_RBT_IterGotoMax
 *
 * Position the iterator at (or closest to) the maximum value indicated
 *--------------------------------------------------------------------------------------*/
BPLib_Status_t BPLib_RBT_IterGotoMax(BPLib_Val_t maximum_value, const BPLib_RBT_Root_t *tree, BPLib_RBT_Iter_t *iter)
{
    BPLib_Val_t curr_val;
    int         status;

    iter->position = BPLib_RBT_IterFindClosestImpl(maximum_value, tree->root);
    if (iter->position == NULL)
    {
        /* the tree is empty */
        status = BPLIB_ERROR;
    }
    else
    {
        /*
         * If duplicates are allowed in this tree, then this may not be the only match.
         * So if this is not a leaf node then go right and repeat the search until arriving
         * at a node that has no right child.
         */
        while (iter->position->right != NULL)
        {
            iter->position = BPLib_RBT_IterFindClosestImpl(maximum_value, iter->position->right);
        }

        curr_val = BPLib_RBT_GetKeyValueImpl(iter->position);
        if (curr_val <= maximum_value)
        {
            status = BPLIB_SUCCESS;
        }
        else
        {
            /* the next node in reverse iteration may be less than the maximum */
            status = BPLib_RBT_IterPrev(iter);
        }
    }

    return status;
}
