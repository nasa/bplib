/************************************************************************
 * File: bplib_blk.h
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
#define BP_BLK_REPALL_MASK              0x000001    /* block must be replicated in every fragment */
#define BP_BLK_NOTIFYNOPROC_MASK        0x000002    /* transmit status report if block cannot be processed */
#define BP_BLK_DELETENOPROC_MASK        0x000004    /* delete bundle if block cannot be processed */
#define BP_BLK_LASTBLOCK_MASK           0x000008    /* last block in bundle */
#define BP_BLK_DROPNOPROC_MASK          0x000010    /* drop block if block cannot be processed */
#define BP_BLK_FORWARDNOPROC_MASK       0x000020    /* block was forwarded without being processed */
#define BP_BLK_EIDREF_MASK              0x000040    /* block contains an EID reference field */

#endif  /* __BPLIB_BLK_H__ */
