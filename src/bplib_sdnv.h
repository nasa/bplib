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
#define BP_SDNV_OVERFLOW    0x01    // insufficient room in variable to read/write value
#define BP_SDNV_INCOMPLETE  0x02    // insufficient room in block to read/write value


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

int bplib_sdnv_read(uint8_t* block, int size, bp_sdnv_t* sdnv, uint8_t* flags);
int bplib_sdnv_write(uint8_t* block, int size, bp_sdnv_t sdnv, uint8_t* flags);

#endif  /* __BPLIB_SDNV_H__ */
