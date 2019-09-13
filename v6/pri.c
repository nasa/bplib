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
#include "pri.h"
#include "sdnv.h"
#include "bplib_os.h"
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
int pri_read (void* block, int size, bp_blk_pri_t* pri, bool update_indices, uint16_t* sdnvflags)
{
    uint8_t* blkbuf = (uint8_t*)block;
    int bytes_read = 0;
    uint16_t flags = 0;
 
    /* Check Size */
    if(size < 1) return bplog(BP_BUNDLEPARSEERR, "Invalid size of primary block: %d\n", size);

    /* Read Block */
    pri->version = blkbuf[0];
    if(!update_indices)
    {
        sdnv_read(blkbuf, size, &pri->pcf,        &flags);
        sdnv_read(blkbuf, size, &pri->blklen,     &flags);
        sdnv_read(blkbuf, size, &pri->dstnode,    &flags);
        sdnv_read(blkbuf, size, &pri->dstserv,    &flags);
        sdnv_read(blkbuf, size, &pri->srcnode,    &flags);
        sdnv_read(blkbuf, size, &pri->srcserv,    &flags);
        sdnv_read(blkbuf, size, &pri->rptnode,    &flags);
        sdnv_read(blkbuf, size, &pri->rptserv,    &flags);
        sdnv_read(blkbuf, size, &pri->cstnode,    &flags);
        sdnv_read(blkbuf, size, &pri->cstserv,    &flags);
        sdnv_read(blkbuf, size, &pri->createsec,  &flags);
        sdnv_read(blkbuf, size, &pri->createseq,  &flags);
        sdnv_read(blkbuf, size, &pri->lifetime,   &flags);

        /* Handle Fragment Fields */
        if(pri->pcf.value & BP_PCF_FRAGMENT_MASK) /* fields are present in bundle */
        {
            sdnv_read(blkbuf, size, &pri->dictlen,    &flags);
            sdnv_read(blkbuf, size, &pri->fragoffset, &flags);
            bytes_read = sdnv_read(blkbuf, size, &pri->paylen,  &flags);
        }
        else
        {
            bytes_read = sdnv_read(blkbuf, size, &pri->dictlen, &flags);
        }
    }
    else
    {

        pri->pcf.width          = 0;
        pri->blklen.width       = 0;
        pri->dstnode.width      = 0;
        pri->dstserv.width      = 0;
        pri->srcnode.width      = 0;
        pri->srcserv.width      = 0;
        pri->rptnode.width      = 0;
        pri->rptserv.width      = 0;
        pri->cstnode.width      = 0;
        pri->cstserv.width      = 0;
        pri->createsec.width    = 0;
        pri->createseq.width    = 0;
        pri->lifetime.width     = 0;
        pri->dictlen.width      = 0;

        pri->pcf.index          = 1;
        pri->blklen.index       = sdnv_read(blkbuf, size, &pri->pcf,        &flags);
        pri->dstnode.index      = sdnv_read(blkbuf, size, &pri->blklen,     &flags);
        pri->dstserv.index      = sdnv_read(blkbuf, size, &pri->dstnode,    &flags);
        pri->srcnode.index      = sdnv_read(blkbuf, size, &pri->dstserv,    &flags);
        pri->srcserv.index      = sdnv_read(blkbuf, size, &pri->srcnode,    &flags);
        pri->rptnode.index      = sdnv_read(blkbuf, size, &pri->srcserv,    &flags);
        pri->rptserv.index      = sdnv_read(blkbuf, size, &pri->rptnode,    &flags);
        pri->cstnode.index      = sdnv_read(blkbuf, size, &pri->rptserv,    &flags);
        pri->cstserv.index      = sdnv_read(blkbuf, size, &pri->cstnode,    &flags);
        pri->createsec.index    = sdnv_read(blkbuf, size, &pri->cstserv,    &flags);
        pri->createseq.index    = sdnv_read(blkbuf, size, &pri->createsec,  &flags);
        pri->lifetime.index     = sdnv_read(blkbuf, size, &pri->createseq,  &flags);
        pri->dictlen.index      = sdnv_read(blkbuf, size, &pri->lifetime,   &flags);

        /* Handle Fragment Fields */
        if(pri->pcf.value & BP_PCF_FRAGMENT_MASK) /* fields are present in bundle */
        {
            pri->fragoffset.width   = 0;
            pri->paylen.width       = 0;

            pri->fragoffset.index   = sdnv_read(blkbuf, size, &pri->dictlen,      &flags);
            pri->paylen.index       = sdnv_read(blkbuf, size, &pri->fragoffset,   &flags);
            bytes_read              = sdnv_read(blkbuf, size, &pri->paylen,       &flags);
        }
        else
        {
            bytes_read              = sdnv_read(blkbuf, size, &pri->dictlen,      &flags);
        }
    }

    /* Set Administrative Record Status */
    if(pri->pcf.value & BP_PCF_ADMIN_MASK)      pri->is_admin_rec = true;
    else                                        pri->is_admin_rec = false;

    /* Set Allow Fragmentation */
    if(pri->pcf.value & BP_PCF_NOFRAG_MASK)     pri->allow_frag = false;
    else                                        pri->allow_frag = true;

    /* Set Is Fragment */
    if(pri->pcf.value & BP_PCF_FRAGMENT_MASK)   pri->is_frag = true;
    else                                        pri->is_frag = false;

    /* Set Custody Request */
    if(pri->pcf.value & BP_PCF_CSTRQST_MASK)    pri->cst_rqst = true;
    else                                        pri->cst_rqst = false;

    /* Success Oriented Error Checking */
    if(flags != 0)
    {
        *sdnvflags |= flags;
        return bplog(BP_BUNDLEPARSEERR, "Flags raised during processing of primary block (%08X)\n", flags);
    }
    else if(pri->version != BP_PRI_VERSION)
    {
        return bplog(BP_WRONGVERSION, "Incorrect version of bundle reported: %d\n", pri->version);
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
int pri_write (void* block, int size, bp_blk_pri_t* pri, bool update_indices, uint16_t* sdnvflags)
{
    uint8_t*    buffer = (uint8_t*)block;
    int         bytes_written = 0;
    uint16_t    flags = 0;

    /* Check Size */
    if(size < 1) return bplog(BP_BUNDLEPARSEERR, "Invalid size of primary block: %d\n", size);

    /* Set Process Control Flags */
    pri->pcf.value |= BP_PCF_SINGLETON_MASK;
    if(pri->is_admin_rec == true)   pri->pcf.value |= BP_PCF_ADMIN_MASK;
    if(pri->is_frag == true)        pri->pcf.value |= BP_PCF_FRAGMENT_MASK;
    if(pri->cst_rqst == true)       pri->pcf.value |= BP_PCF_CSTRQST_MASK;
    if(pri->allow_frag == false)    pri->pcf.value |= BP_PCF_NOFRAG_MASK;

    /* Write Block */
    buffer[0] = pri->version;
    if(!update_indices)
    {
        sdnv_write(buffer, size, pri->pcf,        &flags);
        sdnv_write(buffer, size, pri->dstnode,    &flags);
        sdnv_write(buffer, size, pri->dstserv,    &flags);
        sdnv_write(buffer, size, pri->srcnode,    &flags);
        sdnv_write(buffer, size, pri->srcserv,    &flags);
        sdnv_write(buffer, size, pri->rptnode,    &flags);
        sdnv_write(buffer, size, pri->rptserv,    &flags);
        sdnv_write(buffer, size, pri->cstnode,    &flags);
        sdnv_write(buffer, size, pri->cstserv,    &flags);
        sdnv_write(buffer, size, pri->createsec,  &flags);
        sdnv_write(buffer, size, pri->createseq,  &flags);
        sdnv_write(buffer, size, pri->lifetime,   &flags);

        /* Handle Optional Fragmentation Fields */
        if(pri->allow_frag)
        {
            sdnv_write(buffer, size, pri->dictlen,    &flags);
            sdnv_write(buffer, size, pri->fragoffset, &flags);
            bytes_written = sdnv_write(buffer, size, pri->paylen, &flags);
        }
        else
        {
            bytes_written = sdnv_write(buffer, size, pri->dictlen, &flags);
        }
    }
    else
    {
        pri->pcf.index          = 1;
        pri->pcf.width          = 0;
        pri->blklen.width       = 0;
        pri->dstnode.width      = 0;
        pri->dstserv.width      = 0;
        pri->srcnode.width      = 0;
        pri->srcserv.width      = 0;
        pri->rptnode.width      = 0;
        pri->rptserv.width      = 0;
        pri->cstnode.width      = 0;
        pri->cstserv.width      = 0;
        pri->createsec.width    = 0;
        pri->createseq.width    = 0;
        pri->lifetime.width     = 0;
        pri->dictlen.width      = 0;

        pri->blklen.index       = sdnv_write(buffer, size, pri->pcf,          &flags);
        pri->dstnode.index      = pri->blklen.index + pri->blklen.width;
        pri->dstserv.index      = sdnv_write(buffer, size, pri->dstnode,      &flags);
        pri->srcnode.index      = sdnv_write(buffer, size, pri->dstserv,      &flags);
        pri->srcserv.index      = sdnv_write(buffer, size, pri->srcnode,      &flags);
        pri->rptnode.index      = sdnv_write(buffer, size, pri->srcserv,      &flags);
        pri->rptserv.index      = sdnv_write(buffer, size, pri->rptnode,      &flags);
        pri->cstnode.index      = sdnv_write(buffer, size, pri->rptserv,      &flags);
        pri->cstserv.index      = sdnv_write(buffer, size, pri->cstnode,      &flags);
        pri->createsec.index    = sdnv_write(buffer, size, pri->cstserv,      &flags);
        pri->createseq.index    = sdnv_write(buffer, size, pri->createsec,    &flags);
        pri->lifetime.index     = sdnv_write(buffer, size, pri->createseq,    &flags);
        pri->dictlen.index      = sdnv_write(buffer, size, pri->lifetime,     &flags);

        /* Handle Optional Fragmentation Fields */
        if(pri->is_frag)
        {
            pri->fragoffset.width   = 0;
            pri->paylen.width       = 0;

            pri->fragoffset.index   = sdnv_write(buffer, size, pri->dictlen,      &flags);
            pri->paylen.index       = sdnv_write(buffer, size, pri->fragoffset,   &flags);
            bytes_written           = sdnv_write(buffer, size, pri->paylen,       &flags);
        }
        else
        {
            bytes_written           = sdnv_write(buffer, size, pri->dictlen,      &flags);
        }
    }

    /* Write Block Length */
    pri->blklen.value = bytes_written - pri->dstnode.index;
    sdnv_write(buffer, size, pri->blklen, &flags);

    /* Success Oriented Error Checking */
    if(flags != 0)
    {
        *sdnvflags |= flags;
        return bplog(BP_BUNDLEPARSEERR, "Flags raised during processing of primary block (%08X)\n", flags);
    }
    else
    {
        return bytes_written;
    }
}

/*--------------------------------------------------------------------------------------
 * pri_display -
 *
 *  pri - pointer to a primary bundle block structure to display [INPUT]
 *
 *  Returns: success / fail
 *-------------------------------------------------------------------------------------*/
int pri_display (bp_blk_pri_t* pri)
{
    if(!pri) return false;

    bplog(BP_SUCCESS, "Bundle Primary Block (admin: %d, frag:%d, rqst: %d, allow: %d)\n",
        pri->is_admin_rec, pri->is_frag, pri->cst_rqst, pri->allow_frag);

    bplog(BP_SUCCESS, "PCF: %08X\n",    (unsigned int)pri->pcf.value);
    bplog(BP_SUCCESS, "DST: %ld.%ld\n", (long)pri->dstnode.value, (long)pri->dstserv.value);
    bplog(BP_SUCCESS, "SRC: %ld.%ld\n", (long)pri->srcnode.value, (long)pri->srcserv.value);
    bplog(BP_SUCCESS, "RPT: %ld.%ld\n", (long)pri->rptnode.value, (long)pri->rptserv.value);
    bplog(BP_SUCCESS, "CST: %ld.%ld\n", (long)pri->cstnode.value, (long)pri->cstserv.value);
    bplog(BP_SUCCESS, "SEC: %ld\n",     (long)pri->createsec.value);
    bplog(BP_SUCCESS, "SEQ: %ld\n",     (long)pri->createseq.value);
    bplog(BP_SUCCESS, "LFT: %ld\n",     (long)pri->lifetime.value);
    bplog(BP_SUCCESS, "DCT: %ld\n",     (long)pri->dictlen.value);
    bplog(BP_SUCCESS, "FRG; %ld\n",     (long)pri->fragoffset.value);
    bplog(BP_SUCCESS, "PAY: %ld\n",     (long)pri->paylen.value);

    return true;
}
