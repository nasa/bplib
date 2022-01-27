/************************************************************************
 * File: cbuf.c
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
 * Maintainer(s):
 *  Joe-Paul Swinski, Code 582 NASA GSFC
 *
 *************************************************************************/

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bplib_os.h"
#include "bundle_types.h"
#include "cbuf.h"

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*----------------------------------------------------------------------------
 * Create - initializes circular buffer structure
 *----------------------------------------------------------------------------*/
int cbuf_create(cbuf_t **cbuf, int size)
{
    /* Check Buffer Size */
    if (size < 0 || (unsigned long)size > BP_MAX_INDEX)
        return BP_ERROR;

    if (size > 0)
    {
        /* Allocate Structure */
        *cbuf = (cbuf_t *)bplib_os_calloc(sizeof(cbuf_t));

        /* Allocate Circular Buffer */
        (*cbuf)->table = (bp_active_bundle_t *)bplib_os_calloc(sizeof(bp_active_bundle_t) * size);
        if ((*cbuf)->table == NULL)
            return BP_ERROR;
    }
    else
    {
        /* Empty Table */
        (*cbuf)->table = NULL;
    }

    /* Initialize Circular Buffer Attributes */
    (*cbuf)->size        = size;
    (*cbuf)->num_entries = 0;
    (*cbuf)->newest_cid  = 0;
    (*cbuf)->oldest_cid  = 0;

    /* Return Success */
    return BP_SUCCESS;
}

/*----------------------------------------------------------------------------
 * Destroy - frees memory associated with circular buffer
 *----------------------------------------------------------------------------*/
int cbuf_destroy(cbuf_t *cbuf)
{
    if (cbuf)
    {
        if (cbuf->table)
            bplib_os_free(cbuf->table);
        bplib_os_free(cbuf);
    }

    return BP_SUCCESS;
}

/*----------------------------------------------------------------------------
 * Add
 *----------------------------------------------------------------------------*/
int cbuf_add(cbuf_t *cbuf, bp_active_bundle_t bundle, bool overwrite)
{
    /* Add Bundle */
    bp_index_t ati = bundle.cid % cbuf->size;
    if ((!overwrite) && (cbuf->table[ati].sid != BP_SID_VACANT) && (cbuf->table[ati].cid == bundle.cid))
    {
        return BP_DUPLICATE;
    }
    else
    {
        cbuf->table[ati] = bundle;
        cbuf->num_entries++;
        if (!overwrite)
            cbuf->newest_cid = bundle.cid + 1;
    }

    /* Return Success */
    return BP_SUCCESS;
}

/*----------------------------------------------------------------------------
 * cbuf_next
 *----------------------------------------------------------------------------*/
int cbuf_next(cbuf_t *cbuf, bp_active_bundle_t *bundle)
{
    while (cbuf->oldest_cid != cbuf->newest_cid)
    {
        bp_index_t ati = cbuf->oldest_cid % cbuf->size;
        if (cbuf->table[ati].sid == BP_SID_VACANT)
        {
            cbuf->oldest_cid++;
        }
        else
        {
            if (bundle)
                *bundle = cbuf->table[ati];
            return BP_SUCCESS;
        }
    }

    return BP_TIMEOUT;
}

/*----------------------------------------------------------------------------
 * Remove
 *----------------------------------------------------------------------------*/
int cbuf_remove(cbuf_t *cbuf, bp_val_t cid, bp_active_bundle_t *bundle)
{
    bp_index_t ati = cid % cbuf->size;
    if ((cbuf->table[ati].sid != BP_SID_VACANT) && (cbuf->table[ati].cid == cid))
    {
        if (bundle)
            *bundle = cbuf->table[ati];
        cbuf->table[ati].sid = BP_SID_VACANT;
        cbuf->num_entries--;
        return BP_SUCCESS;
    }

    return BP_ERROR;
}

/*----------------------------------------------------------------------------
 * Available - checks if the provided CID can be added
 *----------------------------------------------------------------------------*/
int cbuf_available(cbuf_t *cbuf, bp_val_t cid)
{
    bp_index_t ati = cid % cbuf->size;
    if (cbuf->table[ati].sid == BP_SID_VACANT)
    {
        return BP_SUCCESS;
    }
    else
    {
        return BP_ERROR;
    }
}

/*----------------------------------------------------------------------------
 * Count - returns number of entries in active table currently in use
 *----------------------------------------------------------------------------*/
int cbuf_count(cbuf_t *cbuf)
{
    return cbuf->num_entries;
}
