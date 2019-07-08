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

#define BYTE_COMBINATIONS pow(2, CHAR_BIT) 

/******************************************************************************
 FILE DATA
 ******************************************************************************/

/* Precalculated CRC16 Table */
static const uint16_t crc16table[256] =
{
	0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
	0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
	0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
	0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
	0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
	0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
	0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
	0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
	0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
	0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
	0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
	0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
	0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
	0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
	0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
	0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
	0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
	0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
	0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
	0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
	0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
	0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
	0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
	0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
	0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
	0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
	0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
	0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
	0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
	0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
	0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
	0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_crc16 - 
 *-------------------------------------------------------------------------------------*/
// X25 = &Parameters{Width: 16, Polynomial: 0x1021, Init: 0xFFFF, ReflectIn: true, ReflectOut: true, FinalXor: 0xFFFF}
// http://reveng.sourceforge.net/crc-catalogue/16.htm#crc.cat-bits.16
// width=16 poly=0x1021 init=0xffff refin=true refout=true xorout=0xffff check=0x906e residue=0xf0b8 name="CRC-16/IBM-SDLC"
// http://www.sunshine2k.de/articles/coding/crc/understanding_crc.html#ch5

// Standard parameters for calculating a CRC.
typedef struct crc_parameters
{
    char* name;               // Name of the CRC.
    size_t size;                 // The size of the resulting CRC in bytes.
    uint64_t polynomial;      // The generator polynomial used to compute the CRC.
    uint64_t initial_value;   // The value used to initialize a CRC.
    bool is_input_reflected;  // Whether or not each byte of the input message is reflected.
    bool is_output_reflected; // Whether or not the resulting CRC is reflected.
    uint64_t final_xor;       // The value ito XOR with the final CRC before returning.
    uint64_t check_value;     // A value used to validate an implementation of a CRC.
} crc_parameters;

typedef struct crc_table
{
    // A ptr to a crc polynomial from which the table was calculated. 
    void* generator_polynomial;
    // A ptr to an array storing 256 elements corresponding to the division of each possible
    // byte by th crc polynomial.
    void* table;      
    // The size in bytes of the crc_polynomial and the elements in table.
    size_t size;
} crc_table;


// Defines the parameters for the CRC-16 IBM-SDLC implementation.
static struct crc_parameters crc_16_ibm_sdlc = {.name="CRC-16 IBM-SDLC", 
                                                .size=2,
                                                .polynomial=0x1021,
                                                .initial_value=0xFFFF,
                                                .is_input_reflected=true,
                                                .is_output_reflected=true,
                                                .final_xor=0xFFFF,
                                                .check_value=0x906e};

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
        for (int i = CHAR_BIT - 1; i >= 0; i--)
        {
            printf("%c", number_ptr[current_byte] & (1 << i) ? '1' : '0');
        }
    }
    printf("\n");
}

/*--------------------------------------------------------------------------------------
 * reflect_bits - Reflects the bits of a number allocated in memory. This function has 
 *      undefined behavior on unallocated memory..
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
        for (int i = CHAR_BIT; i >= 0; i--)
        {
            if ((number_ptr[current_byte] & (1 << i)) != 0)
            {
                reflect_number_ptr[size - current_byte - 1] |= (1 << (CHAR_BIT - i - 1));
            }
        }
    }
}


static void populate_table(struct crc_table* ct)
{
    for (int i = 0; i < BYTE_COMBINATIONS; i++)
    {
        char* b = get_crc(i, ct);
        for (int j = 0; j < 8; j++)
        {
            b = b << 1;
            if (b & (1 << (ct->size / CHAR_BIT)) != 0)
            {
                b = b ^ ct->polynomial_generator;
            }
        }
        memcpy(get_crc(i, ct), b, ct->size);
    }
}

static void* get_crc(int i, struct crc_table* ct)
{
    return ((char *) ct->table) + (i * ct->size);
}

static void free_crc_table(struct crc_table* ct)
{
    free(ct->generator_polynomial);
    free(ct->table);
    free(ct);
}

static struct crc_table* create_crc_table(void* generator_polynomial, size_t size)
{
    crc_table* ct = (crc_table*) malloc(sizeof(crc_table));
    if (cr == NULL)
    {
        // No memory was allocated for the crc_table.
        return NULL;
    }

    ct-size = size;

    ct->generator_polynomial = malloc(size);
    if (ct->generator_polynomial == NULL)
    {
        // No memory was allocated for the generator polynomial.
        free_crc_table(ct);
        return NULL;
    }
    // Assumes that generator polynomial is allocated with size, size.
    memcpy(ct->generator_polynomial, generator_polynomial, size);

    // Allocate the lookup table for the crc.
    ct->table = malloc(size * sizeof(BYTE_COMBINATIONS));
    populate_table(ct);

    if (ct->table == NULL)
    {
        // If no memory is allocated for the lookup array.
        free_crc_table(ct);
        return NULL;
    }
    return ct;
}

uint16_t bplib_crc16(uint8_t* data, int len, uint16_t crc)
{
    int i;
    
	for(i = 0 ; i < len ; i++)
    {
       crc = (crc >> 8) ^ crc16table[(crc ^ data[i]) & 0x00FF];
    }

    return crc;
}

int main (int argc, const char* argv)
{
    uint8_t a = 9;
    uint8_t r;
    print_binary(&a, sizeof(a));
    reflect_bits(&a, &r, sizeof(a));
    print_binary(&r, sizeof(r));
    printf("\n\n\n");
    for (int i = 0; i < 256; i++)
    {
        print_binary(&crc16table[i], 2);
    }
}

