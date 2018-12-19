/******************************************************************************
 * Filename:        bplib_blk.h
 * Purpose:         Bundle Protocol Block (header) 
 * Design Notes:
 ******************************************************************************/

#ifndef __BPLIB_BLK_H__
#define __BPLIB_BLK_H__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdint.h>

/******************************************************************************
 DEFINES
 ******************************************************************************/

/* Block Processing Control Flags */
#define BP_BLK_REPALL_MASK              0x000001    // block must be replicated in every fragment
#define BP_BLK_NOTIFYNOPROC_MASK        0x000002    // transmit status report if block cannot be processed
#define BP_BLK_DELETENOPROC_MASK        0x000004    // delete bundle if block cannot be processed
#define BP_BLK_LASTBLOCK_MASK           0x000008    // last block in bundle
#define BP_BLK_DROPNOPROC_MASK          0x000010    // drop block if block cannot be processed
#define BP_BLK_FORWARDNOPROC_MASK       0x000020    // block was forwarded without being processed
#define BP_BLK_EIDREF_MASK              0x000040    // block contains an EID reference field

#endif  /* __BPLIB_BLK_H__ */
