/************************************************************************
 * File: pri.c
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

#include "pri.h"
#include "sdnv.h"
#include "v6.h"

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * pri_read -
 *
 *  block - pointer to block holding bundle block [INPUT]
 *  size - size of block [INPUT]
 *  pri - pointer to a primary bundle block structure to be populated by this function [OUTPUT]
 *  update_indices - boolean, 0: use <sdnv>.index, 1: update <sdnv>.index as you go [INPUT]
 *
 *  Returns:    Number of bytes processed of bundle
 *-------------------------------------------------------------------------------------*/
int pri_read(const void *block, int size, bp_blk_pri_t *pri, bool update_indices, uint32_t *flags)
{
    uint8_t *blkbuf     = (uint8_t *)block;
    int      bytes_read = 0;
    uint32_t sdnvflags  = 0;

    /* Check Size */
    if (size < 1)
        return bplog(flags, BP_FLAG_FAILED_TO_PARSE, "Invalid size of primary block: %d\n", size);

    /* Read Block */
    pri->version = blkbuf[0];
    if (!update_indices)
    {
        sdnv_read(blkbuf, size, &pri->pcf, &sdnvflags);
        sdnv_read(blkbuf, size, &pri->blklen, &sdnvflags);
        sdnv_read(blkbuf, size, &pri->dstnode, &sdnvflags);
        sdnv_read(blkbuf, size, &pri->dstserv, &sdnvflags);
        sdnv_read(blkbuf, size, &pri->srcnode, &sdnvflags);
        sdnv_read(blkbuf, size, &pri->srcserv, &sdnvflags);
        sdnv_read(blkbuf, size, &pri->rptnode, &sdnvflags);
        sdnv_read(blkbuf, size, &pri->rptserv, &sdnvflags);
        sdnv_read(blkbuf, size, &pri->cstnode, &sdnvflags);
        sdnv_read(blkbuf, size, &pri->cstserv, &sdnvflags);
        sdnv_read(blkbuf, size, &pri->createsec, &sdnvflags);
        sdnv_read(blkbuf, size, &pri->createseq, &sdnvflags);
        sdnv_read(blkbuf, size, &pri->lifetime, &sdnvflags);

        /* Handle Fragment Fields */
        if (pri->pcf.value & BP_PCF_FRAGMENT_MASK) /* fields are present in bundle */
        {
            sdnv_read(blkbuf, size, &pri->dictlen, &sdnvflags);
            sdnv_read(blkbuf, size, &pri->fragoffset, &sdnvflags);
            bytes_read = sdnv_read(blkbuf, size, &pri->paylen, &sdnvflags);
        }
        else
        {
            bytes_read = sdnv_read(blkbuf, size, &pri->dictlen, &sdnvflags);
        }
    }
    else
    {

        pri->pcf.width       = 0;
        pri->blklen.width    = 0;
        pri->dstnode.width   = 0;
        pri->dstserv.width   = 0;
        pri->srcnode.width   = 0;
        pri->srcserv.width   = 0;
        pri->rptnode.width   = 0;
        pri->rptserv.width   = 0;
        pri->cstnode.width   = 0;
        pri->cstserv.width   = 0;
        pri->createsec.width = 0;
        pri->createseq.width = 0;
        pri->lifetime.width  = 0;
        pri->dictlen.width   = 0;

        pri->pcf.index       = 1;
        pri->blklen.index    = sdnv_read(blkbuf, size, &pri->pcf, &sdnvflags);
        pri->dstnode.index   = sdnv_read(blkbuf, size, &pri->blklen, &sdnvflags);
        pri->dstserv.index   = sdnv_read(blkbuf, size, &pri->dstnode, &sdnvflags);
        pri->srcnode.index   = sdnv_read(blkbuf, size, &pri->dstserv, &sdnvflags);
        pri->srcserv.index   = sdnv_read(blkbuf, size, &pri->srcnode, &sdnvflags);
        pri->rptnode.index   = sdnv_read(blkbuf, size, &pri->srcserv, &sdnvflags);
        pri->rptserv.index   = sdnv_read(blkbuf, size, &pri->rptnode, &sdnvflags);
        pri->cstnode.index   = sdnv_read(blkbuf, size, &pri->rptserv, &sdnvflags);
        pri->cstserv.index   = sdnv_read(blkbuf, size, &pri->cstnode, &sdnvflags);
        pri->createsec.index = sdnv_read(blkbuf, size, &pri->cstserv, &sdnvflags);
        pri->createseq.index = sdnv_read(blkbuf, size, &pri->createsec, &sdnvflags);
        pri->lifetime.index  = sdnv_read(blkbuf, size, &pri->createseq, &sdnvflags);
        pri->dictlen.index   = sdnv_read(blkbuf, size, &pri->lifetime, &sdnvflags);

        /* Handle Fragment Fields */
        if (pri->pcf.value & BP_PCF_FRAGMENT_MASK) /* fields are present in bundle */
        {
            pri->fragoffset.width = 0;
            pri->paylen.width     = 0;

            pri->fragoffset.index = sdnv_read(blkbuf, size, &pri->dictlen, &sdnvflags);
            pri->paylen.index     = sdnv_read(blkbuf, size, &pri->fragoffset, &sdnvflags);
            bytes_read            = sdnv_read(blkbuf, size, &pri->paylen, &sdnvflags);
        }
        else
        {
            bytes_read = sdnv_read(blkbuf, size, &pri->dictlen, &sdnvflags);
        }
    }

    /* Set Administrative Record Status */
    if (pri->pcf.value & BP_PCF_ADMIN_MASK)
        pri->is_admin_rec = true;
    else
        pri->is_admin_rec = false;

    /* Set Allow Fragmentation */
    if (pri->pcf.value & BP_PCF_NOFRAG_MASK)
        pri->allow_frag = false;
    else
        pri->allow_frag = true;

    /* Set Is Fragment */
    if (pri->pcf.value & BP_PCF_FRAGMENT_MASK)
        pri->is_frag = true;
    else
        pri->is_frag = false;

    /* Set Custody Request */
    if (pri->pcf.value & BP_PCF_CSTRQST_MASK)
        pri->cst_rqst = true;
    else
        pri->cst_rqst = false;

    /* Set Acknowledgement from Application Request */
    if (pri->pcf.value & BP_PCF_ACKRQST_MASK)
        pri->ack_app = true;
    else
        pri->ack_app = false;

    /* Set Class of Service */
    pri->cos = (pri->pcf.value & BP_PCF_COS_MASK) >> BP_PCF_COS_SHIFT;

    /* Success Oriented Error Checking */
    if (sdnvflags != 0)
    {
        *flags |= sdnvflags;
        return bplog(flags, BP_FLAG_FAILED_TO_PARSE, "Flags raised during processing of primary block (%08X)\n",
                     sdnvflags);
    }
    else if (pri->version != BP_PRI_VERSION)
    {
        return bplog(flags, BP_FLAG_FAILED_TO_PARSE, "Incorrect version of bundle reported: %d\n", pri->version);
    }
    else
    {
        return bytes_read;
    }
}

/*--------------------------------------------------------------------------------------
 * pri_write -
 *
 *  block - pointer to block holding bundle block [OUTPUT]
 *  size - size of block [INPUT]
 *  pri - pointer to a primary bundle block structure used to populate block [INPUT]
 *  update_indices - boolean, 0: use <sdnv>.index, 1: update <sdnv>.index as you go [INPUT]
 *
 *  Returns:    Number of bytes processed of bundle
 *-------------------------------------------------------------------------------------*/
int pri_write(void *block, int size, bp_blk_pri_t *pri, bool update_indices, uint32_t *flags)
{
    uint8_t *buffer        = (uint8_t *)block;
    int      bytes_written = 0;
    uint32_t sdnvflags     = 0;

    /* Check Size */
    if (size < 1)
        return bplog(flags, BP_FLAG_FAILED_TO_PARSE, "Invalid size of primary block: %d\n", size);

    /* Set Process Control Flags */
    pri->pcf.value = BP_PCF_SINGLETON_MASK;
    if (pri->is_admin_rec == true)
        pri->pcf.value |= BP_PCF_ADMIN_MASK;
    if (pri->is_frag == true)
        pri->pcf.value |= BP_PCF_FRAGMENT_MASK;
    if (pri->allow_frag == false)
        pri->pcf.value |= BP_PCF_NOFRAG_MASK;
    if (pri->cst_rqst == true)
        pri->pcf.value |= BP_PCF_CSTRQST_MASK;
    if (pri->ack_app == true)
        pri->pcf.value |= BP_PCF_ACKRQST_MASK;

    /* Set Class of Service */
    pri->pcf.value |= (pri->cos << BP_PCF_COS_SHIFT) & BP_PCF_COS_MASK;

    /* Write Block */
    buffer[0] = pri->version;
    if (!update_indices)
    {
        sdnv_write(buffer, size, pri->pcf, &sdnvflags);
        sdnv_write(buffer, size, pri->dstnode, &sdnvflags);
        sdnv_write(buffer, size, pri->dstserv, &sdnvflags);
        sdnv_write(buffer, size, pri->srcnode, &sdnvflags);
        sdnv_write(buffer, size, pri->srcserv, &sdnvflags);
        sdnv_write(buffer, size, pri->rptnode, &sdnvflags);
        sdnv_write(buffer, size, pri->rptserv, &sdnvflags);
        sdnv_write(buffer, size, pri->cstnode, &sdnvflags);
        sdnv_write(buffer, size, pri->cstserv, &sdnvflags);
        sdnv_write(buffer, size, pri->createsec, &sdnvflags);
        sdnv_write(buffer, size, pri->createseq, &sdnvflags);
        sdnv_write(buffer, size, pri->lifetime, &sdnvflags);

        /* Handle Optional Fragmentation Fields */
        if (pri->is_frag)
        {
            sdnv_write(buffer, size, pri->dictlen, &sdnvflags);
            sdnv_write(buffer, size, pri->fragoffset, &sdnvflags);
            bytes_written = sdnv_write(buffer, size, pri->paylen, &sdnvflags);
        }
        else
        {
            bytes_written = sdnv_write(buffer, size, pri->dictlen, &sdnvflags);
        }
    }
    else
    {
        pri->pcf.index       = 1;
        pri->pcf.width       = 0;
        pri->blklen.width    = 0;
        pri->dstnode.width   = 0;
        pri->dstserv.width   = 0;
        pri->srcnode.width   = 0;
        pri->srcserv.width   = 0;
        pri->rptnode.width   = 0;
        pri->rptserv.width   = 0;
        pri->cstnode.width   = 0;
        pri->cstserv.width   = 0;
        pri->createsec.width = 0;
        pri->createseq.width = 0;
        pri->lifetime.width  = 0;
        pri->dictlen.width   = 0;

        pri->blklen.index = sdnv_write(buffer, size, pri->pcf, &sdnvflags);
        pri->dstnode.index =
            pri->blklen.index + pri->blklen.width; /* block length written below (once size is known) */
        pri->dstserv.index   = sdnv_write(buffer, size, pri->dstnode, &sdnvflags);
        pri->srcnode.index   = sdnv_write(buffer, size, pri->dstserv, &sdnvflags);
        pri->srcserv.index   = sdnv_write(buffer, size, pri->srcnode, &sdnvflags);
        pri->rptnode.index   = sdnv_write(buffer, size, pri->srcserv, &sdnvflags);
        pri->rptserv.index   = sdnv_write(buffer, size, pri->rptnode, &sdnvflags);
        pri->cstnode.index   = sdnv_write(buffer, size, pri->rptserv, &sdnvflags);
        pri->cstserv.index   = sdnv_write(buffer, size, pri->cstnode, &sdnvflags);
        pri->createsec.index = sdnv_write(buffer, size, pri->cstserv, &sdnvflags);
        pri->createseq.index = sdnv_write(buffer, size, pri->createsec, &sdnvflags);
        pri->lifetime.index  = sdnv_write(buffer, size, pri->createseq, &sdnvflags);
        pri->dictlen.index   = sdnv_write(buffer, size, pri->lifetime, &sdnvflags);

        /* Handle Optional Fragmentation Fields */
        if (pri->is_frag)
        {
            pri->fragoffset.width = 0;
            pri->paylen.width     = 0;

            pri->fragoffset.index = sdnv_write(buffer, size, pri->dictlen, &sdnvflags);
            pri->paylen.index     = sdnv_write(buffer, size, pri->fragoffset, &sdnvflags);
            bytes_written         = sdnv_write(buffer, size, pri->paylen, &sdnvflags);
        }
        else
        {
            bytes_written = sdnv_write(buffer, size, pri->dictlen, &sdnvflags);
        }
    }

    /* Write Block Length */
    pri->blklen.value = bytes_written - pri->dstnode.index;
    sdnv_write(buffer, size, pri->blklen, &sdnvflags);

    /* Success Oriented Error Checking */
    if (sdnvflags != 0)
    {
        *flags |= sdnvflags;
        return bplog(flags, BP_FLAG_FAILED_TO_PARSE, "Flags raised during processing of primary block (%08X)\n",
                     sdnvflags);
    }
    else
    {
        return bytes_written;
    }
}
