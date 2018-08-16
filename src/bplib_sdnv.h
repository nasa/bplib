/******************************************************************************
 * Filename:        bplib_sdnv.h
 * Purpose:         Header file for Bundle Protocol Library 
 *                  Self-Delimiting Numeric Values
 * Design Notes:
 ******************************************************************************/

#ifndef __BPLIB_SDNV_H__
#define __BPLIB_SDNV_H__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdint.h>

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define BP_SDNV_SUCCESS     0
#define BP_SDNV_OVERFLOW    0x01
#define BP_SDNV_UNDERFLOW   0x02

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int     bplib_sdnv_read     (uint8_t* block, int size, uint32_t* value, uint8_t* flags);
int     bplib_sdnv_write    (uint8_t* block, int size, uint32_t value);

#endif  /* __BPLIB_SDNV_H__ */
