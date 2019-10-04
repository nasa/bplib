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
#ifdef UNITTESTS

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "crc.h"

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
            printf("%c", number_ptr[current_byte] & (1 << i) ? '1' : '0');
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
 * params: A ptr to a crc_parameters_t struct defining how to calculate the crc and check it
 *      value. [INPUT]
 *
 * returns: True or false indicating whether or not the crc matched its check value.
 *-------------------------------------------------------------------------------------*/
static bool validate_crc_parameters_t(const crc_parameters_t* params)
{
    uint8_t check_message[9] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};
    printf("Input Message:\n");
    print_binary(check_message, 9, 0);
    if (params->length == 16)
    {
        uint16_t check_value = params->n_bit_params.crc16.check_value;
        uint16_t crc = (uint16_t)crc_get(check_message, 9, params);
        printf("Check Value:\n");
        print_binary(&check_value, 2, 0);
        printf("CRC Output:\n");
        print_binary(&crc, 2, 0);
        return crc == check_value;
    }
    else if (params->length == 32)
    {
        uint32_t check_value = params->n_bit_params.crc32.check_value;
        uint32_t crc = (uint32_t)crc_get(check_message, 9, params);
        printf("Check Value:\n");
        print_binary(&check_value, 4, 0);
        printf("CRC Output:\n");
        print_binary(&crc, 4, 0);
        return crc == check_value;
    }

    return false;
}

/******************************************************************************
 TEST FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * test_crc - Populates a crc table and validates the check value for the crc.
 *
 * params: A ptr crc_parameters_t for initing the XOR table and testing the check value. [INPUT]
 *--------------------------------------------------------------------------------------*/
static bool test_crc(crc_parameters_t* params)
{
    printf("Testing CRC %s\n", params->name);
    crc_init(params);
    print_xor_table(params);
    return validate_crc_parameters_t(params);
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

int ut_crc (void)
{
    int failures = 0;
    
    /* Test 1 */
    
    crc_parameters_t p1 = {
        .name                     = "CRC-16 X25", 
        .length                   = 16,
        .should_reflect_input     = true,
        .should_reflect_output    = true,
        .n_bit_params = {
            .crc16 = {
            .generator_polynomial = 0x1021,
            .initial_value        = 0xFFFF,
            .final_xor            = 0xFFFF,
            .check_value          = 0x906E
    }}};
    
    if(!test_crc(&p1))
    {
        failures++;
    }

    /* Test 2 */
    
    crc_parameters_t p2 = {
        .name                     = "CRC-32 Castagnoli", 
        .length                   = 32,
        .should_reflect_input     = true,
        .should_reflect_output    = true,
        .n_bit_params = {
            .crc32 = {
            .generator_polynomial = 0x1EDC6F41,
            .initial_value        = 0xFFFFFFFF,
            .final_xor            = 0xFFFFFFFF,
            .check_value          = 0xE3069283
    }}};

    if(!test_crc(&p2))
    {
        failures++;
    }
    
    /* Return Failures */

    return failures;
}

#endif /* UNITTESTS */
