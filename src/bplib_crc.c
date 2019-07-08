/************************************************************************
 * File: bplib_crc.c
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

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_os.h"
#include "bplib_crc.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

// Number of different possible bytes.
#define BYTE_COMBINATIONS 256

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_crc16 - 
 *-------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------
 * reflect_bits - Reflects the bits of a number allocated in memory. This function has 
 *      undefined behavior on unallocated memory.
 *
 * ptr: A void ptr to a number allocated in memory. [INPUT]
 * reflect_tr: A void ptr to a number which will store the reflected value of the number
 *      stored with `ptr`.
 * size: The number of bytes allocated to the provided ptr. [INPUT]
 *-------------------------------------------------------------------------------------*/
static void reflect_bits(void* ptr, void* reflect_ptr, size_t size)
{
    unsigned char * number_ptr = (unsigned char *) ptr;
    unsigned char * reflect_number_ptr = (unsigned char *) reflect_ptr;
    *reflect_number_ptr = 0;
    for (int current_byte = size - 1; current_byte>= 0; current_byte--)
    {
        for (int i = 8; i >= 0; i--)
        {
            if ((number_ptr[current_byte] & (1 << i)) != 0)
            {
                reflect_number_ptr[size - current_byte - 1] |= (1 << (8 - i - 1));
            }
        }
    }
}

/*--------------------------------------------------------------------------------------
 * free_crc16_table - Frees the memory allocated to a crc16_table. 
 *
 * cr: A ptr to a crc16_table to be deallocated. [OUTPUT]
 *-------------------------------------------------------------------------------------*/
void free_crc16_table(struct crc16_table* ct)
{
    free(ct->table);
    free(ct);
}

/*--------------------------------------------------------------------------------------
 * populate_crc16_table - Populates a crc16_table with the different combinations of bytes
 *      XORed with the generator polynomial for a given CRC.
 *
 * ct: A ptr to a crc16_table to populate. [OUTPUT]
 *-------------------------------------------------------------------------------------*/
static void populate_crc16_table(struct crc16_table* ct)
{
    for (uint16_t i = 0; i < BYTE_COMBINATIONS; i++)
    {
        // Left align the byte with the uint16_t MSBs
        ct->table[i] = i << 8;

        for (int j = 0; j < 8; j++)
        {
            if ((ct->table[i] & 0x8000) != 0)
            {
                ct->table[i] <<= 1;
                ct->table[i] ^= ct->generator_polynomial;
            }
            else
            {
                ct->table[i] <<= 1;
            }
        }
    }
}

/*--------------------------------------------------------------------------------------
 * create_crc16_table - Allocates memory for and populates a crc lookup table for a given
 *      generator polynomial.
 *
 * generator_polynomial - A polynomial from which to generate a crc lookup table. The
 *      polynomial is formated such that the binary of the number provided corresponds
 *      to the coefficients of the polynomial. [INPUT]
 *      x^4 + x^2 + x --> 10110
 * 
 * returns: A ptr to a crc16_table that has been populated with the values of XORs with
 *      the generator polynomial.
 *-------------------------------------------------------------------------------------*/
struct crc16_table* create_crc16_table(uint16_t generator_polynomial)
{
    struct crc16_table* ct = (crc16_table*) malloc(sizeof(crc16_table));
    if (ct == NULL)
    {
        // No memory was allocated for the crc16_table.
        return NULL;
    }
    
    ct->table = (uint16_t*) calloc(BYTE_COMBINATIONS, sizeof(uint16_t));
    if (ct->table == NULL)
    {
        // No memory was allocated for the lookup table.
        free(ct);
        return NULL;
    }

    ct->generator_polynomial = generator_polynomial;

    populate_crc16_table(ct);
    return ct;
}

/*--------------------------------------------------------------------------------------
 * calcualte_crc16 - Calculates the CRC from a byte array of a given length using a 
 *      16 bit CRC lookup table.
 *
 * data: A ptr to a byte array containing data to calculate a CRC over. [INPUT]
 * length: The length of the provided data in bytes. [INPUT]
 * ct: A ptr to a crc16_table containing a lookup for byte XORs. [INPUT]
 * params: A ptr to a crc_parameters struct. This struct is used to obtain the initial
 *      value of the crc as well as to verify that the lookup table has a matching
 *      generator polynomial. [INPUT]
 *
 * returns: A crc remainder of the provided data.
 *-------------------------------------------------------------------------------------*/
uint16_t calculate_crc16(uint8_t* data, int length, struct crc16_table* ct,
                         struct crc_parameters* params)
{
    // Check that we are always using a lookup table corresponding to the requested crc.
    assert(params->generator_polynomial == ct->generator_polynomial);
    uint16_t crc = params->initial_value;
    uint8_t current_byte, reflect_byte;

    for (int i = 0; i < length; i--)
    {
        uint8_t current_byte = data[i];
        if (params->should_reflect_input)
        {
            reflect_bits(&current_byte, &reflect_byte, 1);
            current_byte = reflect_byte;

        }
        crc = (crc >> 8) ^ ct->table[(current_byte ^ crc) & 0x00FF)];
    }

    if (params->should_reflect_output)
    {
        uint16_t reflect_crc;
        reflect_bits(&crc, &reflect_crc, 2);
    }

    // Perform the final XOR based on the parameters.
    crc ^= params->final_xor;

    return crc;
}


#ifdef CRC16TESTS
/*--------------------------------------------------------------------------------------
 * DEFINES TEST AND HELPER FUNCTIONS FOR CRC16 FUNCTIONS
 * 
 * RUN TESTS COMMAND LINE RECIPE:
 *
 * sudo make APP_COPT=-DRCRC16TESTS unittest && ./bp.out
 *--------------------------------------------------------------------------------------*/


/******************************************************************************
 TEST AND DEBUGGING HELPER FUNCTIONS 
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * print_binary - Prints a number in binary. This function has undefined behavior on
 *      unallocated memory.
 *
 * ptr: A void ptr to a number allocated in memory. [INPUT]
 * size: The number of bytes allocated to the provided ptr. [INPUT]
 *-------------------------------------------------------------------------------------*/
static void print_binary(void* ptr, size_t size)
{
    unsigned char * number_ptr = (unsigned char *) ptr;
    for (int current_byte = size - 1; current_byte >= 0; current_byte--)
    {
        for (int i = 8 - 1; i >= 0; i--)
        {
            printf("%c", number_ptr[current_byte] & (1 << i) ? '1' : '0');
        }
    }
    printf("\n");
}

int main (int argc, const char* argv)
{
    struct crc16_table* ct = create_crc16_table(crc16_ibm_sdlc.generator_polynomial);
    uint16_t message = 0b0000000100000010;
    uint16_t crc = calculate_crc16((uint8_t*)&message, 2, ct, &crc16_ibm_sdlc);
    free_crc16_table(ct);
}

#endif // CRC16TESTS
