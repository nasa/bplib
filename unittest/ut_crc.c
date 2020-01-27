/************************************************************************
 * File: ut_crc.c
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
 *  Alexander Meade, Code 582 NASA GSFC
 *  Joe-Paul Swinski, Code 582 NASA GSFC
 *
 *************************************************************************/

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "crc.h"
#include "ut_assert.h"

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

extern crc_parameters_t crc16_x25;
extern crc_parameters_t crc32_castagnoli;

/******************************************************************************
 HELPER FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * print_binary - Prints a number in binary. This function has undefined behavior on
 *      unallocated memory.
 *
 * ptr: A void ptr to a number allocated in memory. [INPUT]
 * size: The number of bytes allocated to the provided ptr. [INPUT]
 * num_spaces: Number of spaces to print before the bits. [INPUT]
 *-------------------------------------------------------------------------------------*/
static void print_binary(const void* ptr, const size_t size, const int num_spaces)
{
    unsigned char * number_ptr = (unsigned char *) ptr;

    /* Print spaces to help align the bits. */
    for (int i = 0; i < num_spaces; i++)
    {
        printf(" ");
    }

    for (int current_byte = size - 1; current_byte >= 0; current_byte--)
    {
        for (int i = 8 - 1; i >= 0; i--)
        {
            printf("%c", (number_ptr[current_byte] & (1 << i)) ? '1' : '0');
        }
    }
    printf("\n");
}

/*--------------------------------------------------------------------------------------
 * print_xor_table - Prints the xor table stored by the crc params.
 *
 * params: A ptr to a crc params to print its xor table. [INPUT]
 *-------------------------------------------------------------------------------------*/
static void print_xor_table(const crc_parameters_t* params)
{
    int index = 0;
    if (params->length == 16)
    {
        const uint16_t* table = params->n_bit_params.crc16.xor_table;
        for (int i = 0; i < 16; i++)
        {
            for (int j = 0; j < 16; j++)
            {
                index = (16 * i) + j;
                printf("%4x|", table[index]);
            }
            printf("\n");
        }
    }
    else if (params->length == 32)
    {
        const uint32_t* table = params->n_bit_params.crc32.xor_table;
        for (int i = 0; i < 16; i++)
        {
            for (int j = 0; j < 16; j++)
            {
                index = (16 * i) + j;
                printf("%8x|", table[index]);
            }
            printf("\n");
        }
    }
}

/*--------------------------------------------------------------------------------------
 * validate_crc_parameters_t - Validates that a crc_parameters properly computes its check
 *      value when passed 123456789.
 *
 * params: A ptr to a crc_parameters struct defining how to calculate the crc and check it
 *      value. [INPUT]
 *
 * returns: True or false indicating whether or not the crc matched its check value.
 *-------------------------------------------------------------------------------------*/
static bool validate_crc_parameters(const crc_parameters_t* params)
{
    uint8_t check_message[9] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};
    printf("Input Message:\n");
    print_binary(check_message, 9, 0);
    if (params->length == 16)
    {
        uint16_t check_value = params->n_bit_params.crc16.check_value;
        uint16_t crc = (uint16_t)crc_get(check_message, 9, params);
        printf("Check Value [%04X]: ", check_value);
        print_binary(&check_value, 2, 0);
        printf("CRC Output [%04X]: ", crc);
        print_binary(&crc, 2, 0);
        return crc == check_value;
    }
    else if (params->length == 32)
    {
        uint32_t check_value = params->n_bit_params.crc32.check_value;
        uint32_t crc = (uint32_t)crc_get(check_message, 9, params);
        printf("Check Value [%08X]: ", check_value);
        print_binary(&check_value, 4, 0);
        printf("CRC Output [%08X]: ", crc);
        print_binary(&crc, 4, 0);
        return crc == check_value;
    }

    return false;
}

/*--------------------------------------------------------------------------------------
 * test_crc16_vectors - generates the CRC for testing
 *-------------------------------------------------------------------------------------*/
static uint16_t test_crc16_vectors(crc_parameters_t* params, const uint8_t* vector, const int size)
{
    crc_init(params);
    return (uint16_t)crc_get(vector, size, params);
}

/******************************************************************************
 TEST FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * test_crc - Populates a crc table and validates the check value for the crc.
 *
 * params: A ptr crc_parameters_t for initing the XOR table and testing the check value. [INPUT]
 *--------------------------------------------------------------------------------------*/
static void test_crc(crc_parameters_t* params)
{
    printf("Testing CRC %s\n", params->name);
    crc_init(params);
    print_xor_table(params);
    ut_assert(validate_crc_parameters(params), "Failed to validate %s\n", params->name);
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

int ut_crc (void)
{
    ut_reset();

    /* Test 1 */
    test_crc(&crc16_x25);

    /* Test 2 */
    test_crc(&crc32_castagnoli);

    /* Test 3 */
    uint8_t v3[] = {0x7, 0x46, 0x57, 0x37, 0x43, 0x25, 0xf7, 0x47, 0x26, 0x16, 0x36, 0x50};
    uint16_t v3_crc = 0x0A58;
    uint16_t v3_crc_calc = test_crc16_vectors(&crc16_x25, v3, sizeof(v3));
    ut_assert(v3_crc_calc == v3_crc, "Failed to caluclate correct CRC16 for V3, %04X != %04X\n", v3_crc_calc, v3_crc);

    /* Test 4 */
    uint8_t v4[] = {0x07, 0x46, 0x57, 0x37, 0x45, 0xf7, 0x47, 0x26, 0x16, 0x36, 0x53, 0x60};
    uint16_t v4_crc = 0xD9A2;
    uint16_t v4_crc_calc = test_crc16_vectors(&crc16_x25, v4, sizeof(v4));
    ut_assert(v4_crc_calc == v4_crc, "Failed to caluclate correct CRC16 for V4, %04X != %04X\n", v4_crc_calc, v4_crc);

    /* Test 5 */
    uint8_t v5[] = {0x74, 0x65, 0x73, 0x74, 0x5f, 0x74, 0x72, 0x61, 0x63, 0x65, 0x37};
    uint16_t v5_crc = 0x441A;
    uint16_t v5_crc_calc = test_crc16_vectors(&crc16_x25, v5, sizeof(v5));
    ut_assert(v5_crc_calc == v5_crc, "Failed to caluclate correct CRC16 for V5, %04X != %04X\n", v5_crc_calc, v5_crc);

    /* Return Failures */
    return ut_failures();
}
