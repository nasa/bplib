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
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    uint32_t    value;
    uint32_t    index;
    uint32_t    width;
} bp_sdnv_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int bplib_sdnv_read(uint8_t* block, int size, bp_sdnv_t* sdnv, uint16_t* flags);
int bplib_sdnv_write(uint8_t* block, int size, bp_sdnv_t sdnv, uint16_t* flags);

#endif  /* __BPLIB_SDNV_H__ */
