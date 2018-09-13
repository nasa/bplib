/******************************************************************************
 * Filename     : bplib_blk_pri.c
 * Purpose      : Bundle Protocol Primary Block
 * Design Notes :
 ******************************************************************************/

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdint.h>
#include "bplib.h"
#include "bplib_blk_pri.h"
#include "bplib_sdnv.h"

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
int bplib_blk_pri_read (void* block, int size, bp_blk_pri_t* pri, int update_indices)
{
    uint8_t* blkbuf = (uint8_t*)block;
    int bytes_read = 0;
    uint8_t flags = 0;

    /* Check Size */
    if(size < 1) return BP_BUNDLEPARSEERR;

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
            pri->fragoffset.index   = bplib_sdnv_read(blkbuf, size, &pri->dictlen,      &flags);
            pri->paylen.index       = bplib_sdnv_read(blkbuf, size, &pri->fragoffset,   &flags);
            bytes_read              = bplib_sdnv_read(blkbuf, size, &pri->paylen,       &flags);
        }
        else
        {
            bytes_read              = bplib_sdnv_read(blkbuf, size, &pri->dictlen,      &flags);

        }
    }

    /* Success Oriented Error Checking */
    if(flags != BP_SUCCESS) return BP_BUNDLEPARSEERR;
    else if(pri->version != BP_PRI_VERSION) return BP_WRONGVERSION;

    /* Set Administrative Record Status */
    if(pri->pcf.value & BP_PCF_ADMIN_MASK)      pri->is_admin_rec = BP_TRUE;
    else                                        pri->is_admin_rec = BP_FALSE;

    /* Set Allow Fragmentation */
    if(pri->pcf.value & BP_PCF_NOFRAG_MASK)     pri->allow_frag = BP_FALSE;
    else                                        pri->allow_frag = BP_TRUE;

    /* Set Is Fragment */
    if(pri->pcf.value & BP_PCF_FRAGMENT_MASK)   pri->is_frag = BP_TRUE;
    else                                        pri->is_frag = BP_FALSE;

    /* Set Custody Request */
    if(pri->pcf.value & BP_PCF_CSTRQST_MASK)    pri->request_custody = BP_TRUE;
    else                                        pri->request_custody = BP_FALSE;

    /* Set Report Deletion */
    if(pri->pcf.value & BP_PCF_RPTDLT_MASK)     pri->report_deletion = BP_TRUE;
    else                                        pri->report_deletion = BP_FALSE;

    /* Success Oriented Error Checking */
    if(flags != 0)  return BP_BUNDLEPARSEERR;
    else            return bytes_read;
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
int bplib_blk_pri_write (void* block, int size, bp_blk_pri_t* pri, int update_indices)
{
    uint8_t*    buffer = (uint8_t*)block;
    uint32_t    bytes_written = 0;
    uint8_t     flags = 0;

    /* Check Size */
    if(size < 1) return BP_BUNDLEPARSEERR;

    /* Set Process Control Flags */
    if(pri->is_admin_rec == BP_TRUE)        pri->pcf.value |= BP_PCF_ADMIN_MASK;
    if(pri->is_frag == BP_TRUE)             pri->pcf.value |= BP_PCF_FRAGMENT_MASK;
    if(pri->request_custody == BP_TRUE)     pri->pcf.value |= BP_PCF_CSTRQST_MASK;
    if(pri->allow_frag == BP_FALSE)         pri->pcf.value |= BP_PCF_NOFRAG_MASK;
    if(pri->report_deletion == BP_TRUE)     pri->pcf.value |= BP_PCF_RPTDLT_MASK;

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
    if(flags != 0)  return BP_BUNDLEPARSEERR;
    else            return bytes_written;
}
