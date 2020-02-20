/************************************************************************
 * File: bplib_store_file_custom.h
 *
 * Maintainer(s):
 *  Joe-Paul Swinski, Code 582 NASA GSFC
 *
 *************************************************************************/

#ifndef _bplib_store_file_custom_h_
#define _bplib_store_file_custom_h_

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdio.h>
#include "cfe.h"

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

FILE*   bp_cfe_fopen    (const char* filename, const char* mode);
int     bp_cfe_fclose   (FILE* stream);
size_t  bp_cfe_fread    (void* ptr, size_t size, size_t count, FILE* stream);
size_t  bp_cfe_fwrite   (const void* ptr, size_t size, size_t count, FILE* stream);
size_t  bp_cfe_fwrite   (const void* ptr, size_t size, size_t count, FILE* stream);
int     bp_cfe_fseek    (FILE* stream, long int offset, int origin);
int     bp_cfe_fflush   (FILE* stream);

#endif /* _bplib_store_file_custom_h_ */
