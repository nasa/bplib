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
 * |  Version  |     Processing Control Flags      |    4
 * |-----------|-----------------------------------|
 * |          Block Length of Rest of Block        |    8
 * |-----------------------|-----------------------|
 * |   Destination Node    |  Destination Service  |    12
 * |-----------------------|-----------------------|
 * |     Source Node       |    Source Service     |    16
 * |-----------------------|-----------------------|
 * |    Report To Node     |   Report To Service   |    20
 * |-----------------------|-----------------------|
 * |    Custodian Node     |   Custodian Service   |    24
 * |-----------------------|-----------------------|
 * |          Creation Timestamp (Seconds)         |    28
 * |-----------------------------------------------|
 * |       Creation Timestamp (Nanoseconds)        |    32
 * |-----------------------------------------------|
 * |               Creation Sequence               |    36
 * |-----------------------------------------------|
 * |                   Lifetime                    |    40
 * |-----------------------------------------------|
 * |             Dictionary Length = 0             |    44
 * |-----------------------------------------------|
 * |               Fragment Offset                 |    48
 * |-----------------------------------------------|
 * |                Payload Length                 |    52
 * -------------------------------------------------
 * 
 ******************************************************************************/

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdlib.h>
#include "bplib.h"
#include "bplib_blk_pri.h"
#include "bplib_sdnv.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

/* Bundle Header Field Indices */
#define BP_DATA_HDR_VER_INDEX           0
#define BP_DATA_HDR_PCF_INDEX           1
#define BP_DATA_HDR_PRI_BLK_LEN_INDEX   4           // block length of rest of block
#define BP_DATA_HDR_DST_NODE_INDEX      8         
#define BP_DATA_HDR_DST_SERV_INDEX      10         
#define BP_DATA_HDR_SRC_NODE_INDEX      12         
#define BP_DATA_HDR_SRC_SERV_INDEX      14         
#define BP_DATA_HDR_RPT_NODE_INDEX      16         
#define BP_DATA_HDR_RPT_SERV_INDEX      18         
#define BP_DATA_HDR_CST_NODE_INDEX      20         
#define BP_DATA_HDR_CST_SERV_INDEX      22         
#define BP_DATA_HDR_CREATE_TIME_INDEX   24         
#define BP_DATA_HDR_SEQ_INDEX           32         
#define BP_DATA_HDR_LIFETIME_INDEX      36         
#define BP_DATA_HDR_DICT_LEN_INDEX      40          // dictionary length
#define BP_DATA_HDR_FRAG_OFFSET_INDEX   44          // fragment offset
#define BP_DATA_HDR_PAY_LEN_INDEX       48          // total payload length

/* Bundle Processing Control Flags */
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
 * 
 *  Returns:    Number of bytes processed of bundle
 *-------------------------------------------------------------------------------------*/
int bplib_blk_pri_read (void* block, int size, bp_blk_pri_t* pri)
{
    uint32_t pcf;
    uint8_t* blkbuf = (uint8_t*)block;
    uint8_t flags = 0;
    int index = 0;

    /* Read Version */
    index += bplib_sdnv_read(&blkbuf[index], size - index, &pri->version, &flags);
    if(flags != BP_SUCCESS) return BP_BUNDLEPARSEERR;
    if(pri->version != BP_PRI_VERSION) return BP_WRONGVERSION;
        
    /* Read Process Control Flags */
    index += bplib_sdnv_read(&blkbuf[index], size - index, &pcf, &flags);
    if(flags != BP_SUCCESS) return BP_BUNDLEPARSEERR;

    /* Set Administrative Record Status */
    if(pcf & BP_PCF_ADMIN_MASK)     pri->is_admin_rec = BP_TRUE;
    else                            pri->is_admin_rec = BP_FALSE;

    /* Set Fragmentation */
    if(pcf & BP_PCF_FRAGMENT_MASK)  pri->is_frag = BP_TRUE;
    else                            pri->is_frag = BP_FALSE;

    /* Set Custody Request */
    if(pcf & BP_PCF_CSTRQST_MASK)   pri->request_custody = BP_TRUE;
    else                            pri->request_custody = BP_FALSE;
    
    /* Set Fragmentation Allowance */
    if(pcf & BP_PCF_NOFRAG_MASK)    pri->allow_frag = BP_FALSE;
    else                            pri->allow_frag = BP_TRUE;

    /* Set Report Deletion */
    if(pcf & BP_PCF_RPTDLT_MASK)    pri->report_deletion = BP_TRUE;
    else                            pri->report_deletion = BP_FALSE;

    /* Read Block Length */
    index += bplib_sdnv_read(&blkbuf[index], size - index, &pri->blklen, &flags);
    if(flags != BP_SUCCESS) return BP_BUNDLEPARSEERR;
    if(pri->blklen < 15 || pri->blklen > 60) return BP_UNSUPPORTED;

    /* Read Route Information */
    index += bplib_sdnv_read(&blkbuf[index], size - index, &pri->dstnode, &flags);
    index += bplib_sdnv_read(&blkbuf[index], size - index, &pri->dstserv, &flags);
    index += bplib_sdnv_read(&blkbuf[index], size - index, &pri->srcnode, &flags);
    index += bplib_sdnv_read(&blkbuf[index], size - index, &pri->srcserv, &flags);
    index += bplib_sdnv_read(&blkbuf[index], size - index, &pri->rptnode, &flags);
    index += bplib_sdnv_read(&blkbuf[index], size - index, &pri->rptserv, &flags);
    index += bplib_sdnv_read(&blkbuf[index], size - index, &pri->cstnode, &flags);
    index += bplib_sdnv_read(&blkbuf[index], size - index, &pri->cstserv, &flags);

    /* Read Creation Time */
    index += bplib_sdnv_read(&blkbuf[index], size - index, &pri->createtime.s, &flags);
    index += bplib_sdnv_read(&blkbuf[index], size - index, &pri->createtime.ns, &flags);
    index += bplib_sdnv_read(&blkbuf[index], size - index, &pri->createseq, &flags);

    /* Read Lifetime */
    index += bplib_sdnv_read(&blkbuf[index], size - index, &pri->lifetime, &flags);

    /* Read Dictionary Length */
    index += bplib_sdnv_read(&blkbuf[index], size - index, &pri->dictlen, &flags);

    /* Read Fragment Offset */
    index += bplib_sdnv_read(&blkbuf[index], size - index, &pri->fragoffset, &flags);

    /* Read Total Payload Length */
    index += bplib_sdnv_read(&blkbuf[index], size - index, &pri->paylen, &flags);

    /* Success Oriented Error Checking */
    if(flags != BP_SUCCESS) return BP_BUNDLEPARSEERR;

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
int bplib_blk_pri_write (void* block, int size, bp_blk_pri_t* pri)
{
    uint8_t* buffer = (uint8_t*)block;
    uint32_t pcf = 0;

    /* Check Size */
    if(size < BP_PRI_BLK_LENGTH) return BP_BUNDLEPARSEERR;

    /* Set Process Control Flags */
    if(pri->is_admin_rec == BP_TRUE)    pcf |= BP_PCF_ADMIN_MASK;
    if(pri->is_frag == BP_TRUE)         pcf |= BP_PCF_FRAGMENT_MASK;
    if(pri->request_custody == BP_TRUE) pcf |= BP_PCF_CSTRQST_MASK;
    if(pri->allow_frag == BP_FALSE)     pcf |= BP_PCF_NOFRAG_MASK;

    /* Write Block */
    buffer[BP_DATA_HDR_VER_INDEX] = pri->version;
    bplib_sdnv_write(&buffer[BP_DATA_HDR_PCF_INDEX],             3,  pcf);    
    bplib_sdnv_write(&buffer[BP_DATA_HDR_PRI_BLK_LEN_INDEX],     4,  BP_PRI_BLK_LENGTH - 4);
    bplib_sdnv_write(&buffer[BP_DATA_HDR_DST_NODE_INDEX],        2,  pri->dstnode);
    bplib_sdnv_write(&buffer[BP_DATA_HDR_DST_SERV_INDEX],        2,  pri->dstserv);
    bplib_sdnv_write(&buffer[BP_DATA_HDR_SRC_NODE_INDEX],        2,  pri->srcnode);
    bplib_sdnv_write(&buffer[BP_DATA_HDR_SRC_SERV_INDEX],        2,  pri->srcserv);
    bplib_sdnv_write(&buffer[BP_DATA_HDR_RPT_NODE_INDEX],        2,  pri->rptnode);
    bplib_sdnv_write(&buffer[BP_DATA_HDR_RPT_SERV_INDEX],        2,  pri->rptserv);
    bplib_sdnv_write(&buffer[BP_DATA_HDR_CST_NODE_INDEX],        2,  pri->cstnode);
    bplib_sdnv_write(&buffer[BP_DATA_HDR_CST_SERV_INDEX],        2,  pri->cstserv);
    bplib_sdnv_write(&buffer[BP_DATA_HDR_CREATE_TIME_INDEX],     4,  pri->createtime.s);
    bplib_sdnv_write(&buffer[BP_DATA_HDR_CREATE_TIME_INDEX + 4], 4,  pri->createtime.ns);
    bplib_sdnv_write(&buffer[BP_DATA_HDR_SEQ_INDEX],             4,  pri->createseq);
    bplib_sdnv_write(&buffer[BP_DATA_HDR_LIFETIME_INDEX],        4,  pri->lifetime);
    bplib_sdnv_write(&buffer[BP_DATA_HDR_DICT_LEN_INDEX],        4,  pri->dictlen);     
    bplib_sdnv_write(&buffer[BP_DATA_HDR_FRAG_OFFSET_INDEX],     4,  pri->fragoffset);
    bplib_sdnv_write(&buffer[BP_DATA_HDR_PAY_LEN_INDEX],         4,  pri->paylen);

    /* Return Bytes Written */
    return BP_PRI_BLK_LENGTH;
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
