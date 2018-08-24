/******************************************************************************
 * Filename     : bplib_blk_pri.c
 * Purpose      : Bundle Protocol Primary Block
 * Design Notes :
 * 
 * -------------------------------------------------
 *               Primary Bundle Block
 * -------------------------------------------------
 * |    MSB    |           |           |    LSB    |
 * | (8 bits)  | (8 bits)  | (8 bits)  | (8 bits)  |
 * |-----------|-----------|-----------|-----------|
 * |  Version  |     Processing Control Flags      |    
 * |-----------|-----------------------------------|
 * |          Block Length of Rest of Block        |    
 * |-----------------------|-----------------------|
 * |   Destination Node    |  Destination Service  |    
 * |-----------------------|-----------------------|
 * |     Source Node       |    Source Service     |    
 * |-----------------------|-----------------------|
 * |    Report To Node     |   Report To Service   |    
 * |-----------------------|-----------------------|
 * |    Custodian Node     |   Custodian Service   |    
 * |-----------------------|-----------------------|
 * |          Creation Timestamp (Seconds)         |    
 * |-----------------------------------------------|
 * |       Creation Timestamp (Nanoseconds)        |    
 * |-----------------------------------------------|
 * |               Creation Sequence               |    
 * |-----------------------------------------------|
 * |                   Lifetime                    |    
 * |-----------------------------------------------|
 * |             Dictionary Length = 0             |    
 * |-----------------------------------------------|
 * |               Fragment Offset                 |    
 * |-----------------------------------------------|
 * |                Payload Length                 |    
 * -------------------------------------------------
 * 
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
 * 
 *  Returns:    Number of bytes processed of bundle
 *-------------------------------------------------------------------------------------*/
int bplib_blk_pri_read (void* block, int size, bp_blk_pri_t* pri, int update_indices)
{
    uint8_t* blkbuf = (uint8_t*)block;
    uint8_t flags = 0;
    int index = 0;

    /* Check Size */
    if(size <= 0) return BP_BUNDLEPARSEERR;

    /* Read Block */
    pri->version = blkbuf[index++];
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
        bplib_sdnv_read(blkbuf, size, &pri->createtms,  &flags);
        bplib_sdnv_read(blkbuf, size, &pri->createtmns, &flags);
        bplib_sdnv_read(blkbuf, size, &pri->createseq,  &flags);
        bplib_sdnv_read(blkbuf, size, &pri->lifetime,   &flags);
        bplib_sdnv_read(blkbuf, size, &pri->dictlen,    &flags);

        /* Read Fragment Fields */
        if(pri->pcf & BP_PCF_FRAGMENT_MASK) // fields are present in bundle
        {
            bplib_sdnv_read(blkbuf, size, &pri->fragoffset, &flags);
            bplib_sdnv_read(blkbuf, size, &pri->paylen,     &flags);
            index = pri->paylen.index + pri->paylen.width;
        }
        else
        {
            index = pri->dictlen.index + pri->dictlen.width;
        }

    }
    else
    {   
        pri->pcf.index          = index; index += bplib_sdnv_read(blkbuf, size, &pri->pcf,        &flags);
        pri->blklen.index       = index; index += bplib_sdnv_read(blkbuf, size, &pri->blklen,     &flags);
        pri->dstnode.index      = index; index += bplib_sdnv_read(blkbuf, size, &pri->dstnode,    &flags);
        pri->dstserv.index      = index; index += bplib_sdnv_read(blkbuf, size, &pri->dstserv,    &flags);
        pri->srcnode.index      = index; index += bplib_sdnv_read(blkbuf, size, &pri->srcnode,    &flags);
        pri->srcserv.index      = index; index += bplib_sdnv_read(blkbuf, size, &pri->srcserv,    &flags);
        pri->rptnode.index      = index; index += bplib_sdnv_read(blkbuf, size, &pri->rptnode,    &flags);
        pri->rptserv.index      = index; index += bplib_sdnv_read(blkbuf, size, &pri->rptserv,    &flags);
        pri->cstnode.index      = index; index += bplib_sdnv_read(blkbuf, size, &pri->cstnode,    &flags);
        pri->cstserv.index      = index; index += bplib_sdnv_read(blkbuf, size, &pri->cstserv,    &flags);
        pri->createtms.index    = index; index += bplib_sdnv_read(blkbuf, size, &pri->createtms,  &flags);
        pri->createtmns.index   = index; index += bplib_sdnv_read(blkbuf, size, &pri->createtmns, &flags);
        pri->createseq.index    = index; index += bplib_sdnv_read(blkbuf, size, &pri->createseq,  &flags);
        pri->lifetime.index     = index; index += bplib_sdnv_read(blkbuf, size, &pri->lifetime,   &flags);
        pri->dictlen.index      = index; index += bplib_sdnv_read(blkbuf, size, &pri->dictlen,    &flags);

        /* Read Fragment Fields */
        if(pri->pcf & BP_PCF_FRAGMENT_MASK) // fields are present in bundle
        {
            pri->fragoffset.index = index;  index += bplib_sdnv_read(blkbuf, size, &pri->fragoffset,    &flags);
            pri->paylen.index = index;      index += bplib_sdnv_read(blkbuf, size, &pri->paylen,        &flags);
        }
    }

    /* Success Oriented Error Checking */
    if(flags != BP_SUCCESS) return BP_BUNDLEPARSEERR;    
    else if(pri->version != BP_PRI_VERSION) return BP_WRONGVERSION;
    
    /* Set Administrative Record Status */
    if(pri->pcf & BP_PCF_ADMIN_MASK)        pri->is_admin_rec = BP_TRUE;
    else                                    pri->is_admin_rec = BP_FALSE;

    /* Set Fragmentation */
    if( (pri->pcf & BP_PCF_FRAGMENT_MASK) || 
       !(pri->pcf & BP_PCF_NOFRAG_MASK) )   pri->allow_frag = BP_TRUE;
    else                                    pri->allow_frag = BP_FALSE;

    /* Set Custody Request */
    if(pri->pcf & BP_PCF_CSTRQST_MASK)      pri->request_custody = BP_TRUE;
    else                                    pri->request_custody = BP_FALSE;
    
    /* Set Report Deletion */
    if(pri->pcf & BP_PCF_RPTDLT_MASK)       pri->report_deletion = BP_TRUE;
    else                                    pri->report_deletion = BP_FALSE;

    /* Set Creation Time */
    pri->creation_time.s = pri->createtms.value; 
    pri->creation_time.ns = pri->createtmns.value; 
    
    /* Set Sequence */
    pri->sequence = pri->createseq.value;
    
    /* Return Number of Bytes Read */
    return index;
}

/*--------------------------------------------------------------------------------------
 * bplib_blk_pri_write - 
 * 
 *  block - pointer to block holding bundle block [OUTPUT]
 *  size - size of block [INPUT]
 *  pri - pointer to a primary bundle block structure used to populate block [INPUT]
 * 
 *  Returns:    Number of bytes processed of bundle
 *-------------------------------------------------------------------------------------*/
int bplib_blk_pri_write (void* block, int size, bp_blk_pri_t* pri, int update_indices)
{
    uint8_t*    buffer = (uint8_t*)block;
    uint32_t    index = 0; // used to make sure buffer not overwritten
    uint8_t     flags = 0;

    /* Check Size */
    if(size < 1) return BP_BUNDLEPARSEERR;

    /* Set Process Control Flags */
    if(pri->is_admin_rec == BP_TRUE)        pri->pcf.value |= BP_PCF_ADMIN_MASK;
    if(pri->allow_frag == BP_TRUE)          pri->pcf.value |= BP_PCF_FRAGMENT_MASK;
    if(pri->request_custody == BP_TRUE)     pri->pcf.value |= BP_PCF_CSTRQST_MASK;
    if(pri->allow_frag == BP_FALSE)         pri->pcf.value |= BP_PCF_NOFRAG_MASK;
    if(pri->report_deletion == BP_FALSE)    pri->pcf.value |= BP_PCF_RPTDLT_MASK;

    /* Set Creation Time */
    pri->createtms.value = pri->creation_time.s; 
    pri->createtmns.value = pri->creation_time.ns; 
    
    /* Set Sequence */
    pri->createseq.value = pri->sequence;
    
    /* Write Block */    
    buffer[index++] = pri->version;
    if(update_indices == 0)
    {
        bplib_sdnv_write(buffer, size, pri->pcf,           &flags);
        bplib_sdnv_write(buffer, size, pri->dstnode,       &flags);
        bplib_sdnv_write(buffer, size, pri->dstserv,       &flags);
        bplib_sdnv_write(buffer, size, pri->srcnode,       &flags);
        bplib_sdnv_write(buffer, size, pri->srcserv,       &flags);
        bplib_sdnv_write(buffer, size, pri->rptnode,       &flags);
        bplib_sdnv_write(buffer, size, pri->rptserv,       &flags);
        bplib_sdnv_write(buffer, size, pri->cstnode,       &flags);
        bplib_sdnv_write(buffer, size, pri->cstserv,       &flags);
        bplib_sdnv_write(buffer, size, pri->createtms,     &flags);
        bplib_sdnv_write(buffer, size, pri->createtmns,    &flags);
        bplib_sdnv_write(buffer, size, pri->createseq,     &flags);
        bplib_sdnv_write(buffer, size, pri->lifetime,      &flags);
        bplib_sdnv_write(buffer, size, pri->dictlen,       &flags);     
        
        /* Write Optional Fragmentation Fields */
        if(pri->allow_frag)
        {
            bplib_sdnv_write(buffer, size, pri->fragoffset,    &flags);
            bplib_sdnv_write(buffer, size, pri->paylen,        &flags);
            index = pri->paylen.index + pri->paylen.width;
        }
        else
        {
            index = pri->dictlen.index + pri->dictlen.width;
        }        
    }
    else
    {
        pri->pcf.index        = index;  index += bplib_sdnv_write(buffer, size - index, pri->pcf,           &flags); 
        pri->blklen.index     = index;  index += pri->blklen.width;
        pri->dstnode.index    = index;  index += bplib_sdnv_write(buffer, size - index, pri->dstnode,       &flags);
        pri->dstserv.index    = index;  index += bplib_sdnv_write(buffer, size - index, pri->dstserv,       &flags);
        pri->srcnode.index    = index;  index += bplib_sdnv_write(buffer, size - index, pri->srcnode,       &flags);
        pri->srcserv.index    = index;  index += bplib_sdnv_write(buffer, size - index, pri->srcserv,       &flags);
        pri->rptnode.index    = index;  index += bplib_sdnv_write(buffer, size - index, pri->rptnode,       &flags);
        pri->rptserv.index    = index;  index += bplib_sdnv_write(buffer, size - index, pri->rptserv,       &flags);
        pri->cstnode.index    = index;  index += bplib_sdnv_write(buffer, size - index, pri->cstnode,       &flags);
        pri->cstserv.index    = index;  index += bplib_sdnv_write(buffer, size - index, pri->cstserv,       &flags);
        pri->createtms.index  = index;  index += bplib_sdnv_write(buffer, size - index, pri->createtms,     &flags);
        pri->createtmns.index = index;  index += bplib_sdnv_write(buffer, size - index, pri->createtmns,    &flags);
        pri->createseq.index  = index;  index += bplib_sdnv_write(buffer, size - index, pri->createseq,     &flags);
        pri->lifetime.index   = index;  index += bplib_sdnv_write(buffer, size - index, pri->lifetime,      &flags);
        pri->dictlen.index    = index;  index += bplib_sdnv_write(buffer, size - index, pri->dictlen,       &flags);     

        /* Write Optional Fragmentation Fields */
        if(pri->allow_frag)
        {
            pri->fragoffset.index = index; index += bplib_sdnv_write(buffer, size - index, pri->fragoffset,    &flags);
            pri->paylen.index     = index; index += bplib_sdnv_write(buffer, size - index, pri->paylen,        &flags);
        }        
    }

    /* Write Block Length */ 
    pri->blklen.value = index - pri->blklen.index;
    bplib_sdnv_write(buffer, pri->blklen, &flags);

    /* Success Oriented Error Checking */
    if(flags != BP_SUCCESS) return BP_BUNDLEPARSEERR;

    /* Return Bytes Written */
    return index;
}

/*--------------------------------------------------------------------------------------
 * bplib_blk_pri_setfrag - makes bundle a fragment
 * 
 *  block       pointer to primary block [OUTPUT]
 *  size        size of block in bytes
 *  fragoffset  fragment offset of bundle      
 * 
 *  Returns -   1: success, other: error code
 *-------------------------------------------------------------------------------------*/
int bplib_blk_pri_setfrag(void* block, uint32_t fragoffset)
{
    uint8_t* buffer = (uint8_t*)block;
    uint32_t pcf = 0;
    uint8_t flags = 0;

    /* Read Process Control Flags */
    bplib_sdnv_read(&buffer[BP_DATA_HDR_PCF_INDEX], 3, &pcf, &flags);
    if(flags != BP_SUCCESS) return flags;

    /* Set Fragmentation Flag */
    pcf |= BP_PCF_FRAGMENT_MASK;
    bplib_sdnv_write(&buffer[BP_DATA_HDR_PCF_INDEX], 3, pcf); 
    bplib_sdnv_write(&buffer[BP_DATA_HDR_FRAG_OFFSET_INDEX], 4, fragoffset);

    /* Return Success */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_blk_pri_settime - 
 * 
 *  block -     pointer to primary block [OUTPUT]
 *  tm -        creation time of bundle
 * 
 *  Returns -   1: success, other: error code
 *-------------------------------------------------------------------------------------*/
int bplib_blk_pri_settime(void* block, bp_time_t tm)
{
    uint8_t* buffer = (uint8_t*)block;
    bplib_sdnv_write(&buffer[BP_DATA_HDR_CREATE_TIME_INDEX], 4, tm.s);
    bplib_sdnv_write(&buffer[BP_DATA_HDR_CREATE_TIME_INDEX + 4], 4, tm.ns);
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_blk_pri_setseq - 
 * 
 *  block -     pointer to primary block [OUTPUT]
 *  seq -       creation time sequence number of bundle
 * 
 *  Returns -   1: success, other: error code
 *-------------------------------------------------------------------------------------*/
int bplib_blk_pri_setseq(void* block, uint32_t seq)
{
    uint8_t* buffer = (uint8_t*)block;
    bplib_sdnv_write(&buffer[BP_DATA_HDR_SEQ_INDEX], 4, seq);
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_blk_pri_setpaylen - 
 * 
 *  block -     pointer to primary block [OUTPUT]
 *  paylen -    total payload length of bundle
 * 
 *  Returns -   1: success, other: error code
 *-------------------------------------------------------------------------------------*/
int bplib_blk_pri_setpaylen(void* block, uint32_t paylen)
{
    uint8_t* buffer = (uint8_t*)block;
    bplib_sdnv_write(&buffer[BP_DATA_HDR_PAY_LEN_INDEX], 4, paylen);
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_blk_pri_setpaylen - 
 * 
 *  block -     pointer to primary block [OUTPUT]
 *  dstnode -   destination node
 *  dstserv -   destination service
 * 
 *  Returns -   1: success, other: error code
 *-------------------------------------------------------------------------------------*/
int bplib_blk_pri_setdst(void* block, uint32_t dstnode, uint32_t dstserv)
{
    uint8_t* buffer = (uint8_t*)block;
    bplib_sdnv_write(&buffer[BP_DATA_HDR_DST_NODE_INDEX], 2, dstnode);
    bplib_sdnv_write(&buffer[BP_DATA_HDR_DST_SERV_INDEX], 2, dstserv);
    return BP_SUCCESS;
}
