/************************************************************************
 * File: bplib_blk_pri.c
 *
 *  Copyright 2019 United States Government as represented by the
 *  Administrator of the National Aeronautics and Space Administration.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * Maintainer(s):
 *  Joe-Paul Swinski, Code 582 NASA GSFC
 *
 *************************************************************************/

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bplib_blk_pri.h"
#include "bplib_sdnv.h"
#include "bplib_os.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define BP_PCF_FRAGMENT_MASK            0x000001    // bundle is a fragement
#define BP_PCF_ADMIN_MASK               0x000002    // bundle is an administrative record
#define BP_PCF_NOFRAG_MASK              0x000004    // bundle must not be fragmented
#define BP_PCF_CSTRQST_MASK             0x000008    // custody transfer is requested
#define BP_PCF_SINGLETON_MASK           0x000010    // destination endpoint is a singleton
#define BP_PCF_ACKRQST_MASK             0x000020    // acknowledgement is requested from application
#define BP_PCF_COS_MASK                 0x000180    // class of service
#define BP_PCF_COS_SHIFT                7
#define BP_PCF_RPTRCV_MASK              0x004000    // report reception
#define BP_PCF_RPTACT_MASK              0x008000    // report acceptance
#define BP_PCF_RPTFRW_MASK              0x010000    // report forwarding
#define BP_PCF_RPTDLV_MASK              0x020000    // report delivery
#define BP_PCF_RPTDLT_MASK              0x040000    // report deletion

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_blk_pri_read -
 *
 *  block - pointer to block holding bundle block [INPUT]
 *  size - size of block [INPUT]
 *  pri - pointer to a primary bundle block structure to be populated by this function [OUTPUT]
 *  update_indices - boolean, 0: use <sdnv>.index, 1: update <sdnv>.index as you go [INPUT]
 *
 *  Returns:    Number of bytes processed of bundle
 *-------------------------------------------------------------------------------------*/
int bplib_blk_pri_read (void* block, int size, bp_blk_pri_t* pri, bool update_indices)
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
        bplib_sdnv_read(blkbuf, size, &pri->pcf,        &flags);
        bplib_sdnv_read(blkbuf, size, &pri->blklen,     &flags);
        bplib_sdnv_read(blkbuf, size, &pri->dstnode,    &flags);
        bplib_sdnv_read(blkbuf, size, &pri->dstserv,    &flags);
        bplib_sdnv_read(blkbuf, size, &pri->srcnode,    &flags);
        bplib_sdnv_read(blkbuf, size, &pri->srcserv,    &flags);
        bplib_sdnv_read(blkbuf, size, &pri->rptnode,    &flags);
        bplib_sdnv_read(blkbuf, size, &pri->rptserv,    &flags);
        bplib_sdnv_read(blkbuf, size, &pri->cstnode,    &flags);
        bplib_sdnv_read(blkbuf, size, &pri->cstserv,    &flags);
        bplib_sdnv_read(blkbuf, size, &pri->createsec,  &flags);
        bplib_sdnv_read(blkbuf, size, &pri->createseq,  &flags);
        bplib_sdnv_read(blkbuf, size, &pri->lifetime,   &flags);

        /* Handle Fragment Fields */
        if(pri->pcf.value & BP_PCF_FRAGMENT_MASK) // fields are present in bundle
        {
            bplib_sdnv_read(blkbuf, size, &pri->dictlen,    &flags);
            bplib_sdnv_read(blkbuf, size, &pri->fragoffset, &flags);
            bytes_read = bplib_sdnv_read(blkbuf, size, &pri->paylen,  &flags);
        }
        else
        {
            bytes_read = bplib_sdnv_read(blkbuf, size, &pri->dictlen, &flags);
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
        pri->blklen.index       = bplib_sdnv_read(blkbuf, size, &pri->pcf,        &flags);
        pri->dstnode.index      = bplib_sdnv_read(blkbuf, size, &pri->blklen,     &flags);
        pri->dstserv.index      = bplib_sdnv_read(blkbuf, size, &pri->dstnode,    &flags);
        pri->srcnode.index      = bplib_sdnv_read(blkbuf, size, &pri->dstserv,    &flags);
        pri->srcserv.index      = bplib_sdnv_read(blkbuf, size, &pri->srcnode,    &flags);
        pri->rptnode.index      = bplib_sdnv_read(blkbuf, size, &pri->srcserv,    &flags);
        pri->rptserv.index      = bplib_sdnv_read(blkbuf, size, &pri->rptnode,    &flags);
        pri->cstnode.index      = bplib_sdnv_read(blkbuf, size, &pri->rptserv,    &flags);
        pri->cstserv.index      = bplib_sdnv_read(blkbuf, size, &pri->cstnode,    &flags);
        pri->createsec.index    = bplib_sdnv_read(blkbuf, size, &pri->cstserv,    &flags);
        pri->createseq.index    = bplib_sdnv_read(blkbuf, size, &pri->createsec,  &flags);
        pri->lifetime.index     = bplib_sdnv_read(blkbuf, size, &pri->createseq,  &flags);
        pri->dictlen.index      = bplib_sdnv_read(blkbuf, size, &pri->lifetime,   &flags);

        /* Handle Fragment Fields */
        if(pri->pcf.value & BP_PCF_FRAGMENT_MASK) // fields are present in bundle
        {
            pri->fragoffset.width   = 0;
            pri->paylen.width       = 0;

            pri->fragoffset.index   = bplib_sdnv_read(blkbuf, size, &pri->dictlen,      &flags);
            pri->paylen.index       = bplib_sdnv_read(blkbuf, size, &pri->fragoffset,   &flags);
            bytes_read              = bplib_sdnv_read(blkbuf, size, &pri->paylen,       &flags);
        }
        else
        {
            bytes_read              = bplib_sdnv_read(blkbuf, size, &pri->dictlen,      &flags);
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
    if(pri->pcf.value & BP_PCF_CSTRQST_MASK)    pri->request_custody = true;
    else                                        pri->request_custody = false;

    /* Success Oriented Error Checking */
    if(flags != 0)                          return bplog(BP_BUNDLEPARSEERR, "Failed to read primary block sdnv %0X\n", flags);
    else if(pri->version != BP_PRI_VERSION) return bplog(BP_WRONGVERSION, "Incorrect version of bundle reported: %d\n", pri->version);
    else                                    return bytes_read;
}

/*--------------------------------------------------------------------------------------
 * bplib_blk_pri_write -
 *
 *  block - pointer to block holding bundle block [OUTPUT]
 *  size - size of block [INPUT]
 *  pri - pointer to a primary bundle block structure used to populate block [INPUT]
 *  update_indices - boolean, 0: use <sdnv>.index, 1: update <sdnv>.index as you go [INPUT]
 *
 *  Returns:    Number of bytes processed of bundle
 *-------------------------------------------------------------------------------------*/
int bplib_blk_pri_write (void* block, int size, bp_blk_pri_t* pri, bool update_indices)
{
    uint8_t*    buffer = (uint8_t*)block;
    uint32_t    bytes_written = 0;
    uint16_t    flags = 0;

    /* Check Size */
    if(size < 1) return bplog(BP_BUNDLEPARSEERR, "Invalid size of primary block: %d\n", size);

    /* Set Process Control Flags */
    pri->pcf.value |= BP_PCF_SINGLETON_MASK;
    if(pri->is_admin_rec == true)        pri->pcf.value |= BP_PCF_ADMIN_MASK;
    if(pri->is_frag == true)             pri->pcf.value |= BP_PCF_FRAGMENT_MASK;
    if(pri->request_custody == true)     pri->pcf.value |= BP_PCF_CSTRQST_MASK;
    if(pri->allow_frag == false)         pri->pcf.value |= BP_PCF_NOFRAG_MASK;

    /* Write Block */
    buffer[0] = pri->version;
    if(!update_indices)
    {
        bplib_sdnv_write(buffer, size, pri->pcf,        &flags);
        bplib_sdnv_write(buffer, size, pri->dstnode,    &flags);
        bplib_sdnv_write(buffer, size, pri->dstserv,    &flags);
        bplib_sdnv_write(buffer, size, pri->srcnode,    &flags);
        bplib_sdnv_write(buffer, size, pri->srcserv,    &flags);
        bplib_sdnv_write(buffer, size, pri->rptnode,    &flags);
        bplib_sdnv_write(buffer, size, pri->rptserv,    &flags);
        bplib_sdnv_write(buffer, size, pri->cstnode,    &flags);
        bplib_sdnv_write(buffer, size, pri->cstserv,    &flags);
        bplib_sdnv_write(buffer, size, pri->createsec,  &flags);
        bplib_sdnv_write(buffer, size, pri->createseq,  &flags);
        bplib_sdnv_write(buffer, size, pri->lifetime,   &flags);

        /* Handle Optional Fragmentation Fields */
        if(pri->allow_frag)
        {
            bplib_sdnv_write(buffer, size, pri->dictlen,    &flags);
            bplib_sdnv_write(buffer, size, pri->fragoffset, &flags);
            bytes_written = bplib_sdnv_write(buffer, size, pri->paylen, &flags);
        }
        else
        {
            bytes_written = bplib_sdnv_write(buffer, size, pri->dictlen, &flags);
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

        pri->blklen.index       = bplib_sdnv_write(buffer, size, pri->pcf,          &flags);
        pri->dstnode.index      = pri->blklen.index + pri->blklen.width;
        pri->dstserv.index      = bplib_sdnv_write(buffer, size, pri->dstnode,      &flags);
        pri->srcnode.index      = bplib_sdnv_write(buffer, size, pri->dstserv,      &flags);
        pri->srcserv.index      = bplib_sdnv_write(buffer, size, pri->srcnode,      &flags);
        pri->rptnode.index      = bplib_sdnv_write(buffer, size, pri->srcserv,      &flags);
        pri->rptserv.index      = bplib_sdnv_write(buffer, size, pri->rptnode,      &flags);
        pri->cstnode.index      = bplib_sdnv_write(buffer, size, pri->rptserv,      &flags);
        pri->cstserv.index      = bplib_sdnv_write(buffer, size, pri->cstnode,      &flags);
        pri->createsec.index    = bplib_sdnv_write(buffer, size, pri->cstserv,      &flags);
        pri->createseq.index    = bplib_sdnv_write(buffer, size, pri->createsec,    &flags);
        pri->lifetime.index     = bplib_sdnv_write(buffer, size, pri->createseq,    &flags);
        pri->dictlen.index      = bplib_sdnv_write(buffer, size, pri->lifetime,     &flags);

        /* Handle Optional Fragmentation Fields */
        if(pri->is_frag)
        {
            pri->fragoffset.width   = 0;
            pri->paylen.width       = 0;

            pri->fragoffset.index   = bplib_sdnv_write(buffer, size, pri->dictlen,      &flags);
            pri->paylen.index       = bplib_sdnv_write(buffer, size, pri->fragoffset,   &flags);
            bytes_written           = bplib_sdnv_write(buffer, size, pri->paylen,       &flags);
        }
        else
        {
            bytes_written           = bplib_sdnv_write(buffer, size, pri->dictlen,      &flags);
        }
    }

    /* Write Block Length */
    pri->blklen.value = bytes_written - pri->dstnode.index;
    bplib_sdnv_write(buffer, size, pri->blklen, &flags);

    /* Success Oriented Error Checking */
    if(flags != 0)  return bplog(BP_BUNDLEPARSEERR, "Failed to write primary block sdnv %0X\n", flags);
    else            return bytes_written;
}

/*--------------------------------------------------------------------------------------
 * bplib_blk_pri_display -
 *
 *  pri - pointer to a primary bundle block structure to display [INPUT]
 *
 *  Returns: success / fail
 *-------------------------------------------------------------------------------------*/
int bplib_blk_pri_display (bp_blk_pri_t* pri)
{
    if(!pri) return false;

    bplog(BP_SUCCESS, "Bundle Primary Block (admin: %d, frag:%d, rqst: %d, allow: %d)\n",
        pri->is_admin_rec, pri->is_frag, pri->request_custody, pri->allow_frag);

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
