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

/*
 * WARNING:
 * to make this appear like/compatible with the C stdio prototypes, FILE* is used, but the pointer
 * should not be ever passed to the actual C library implementation (it is not a file pointer)
 */
typedef FILE *bp_file_t;

bp_file_t bp_cfe_fopen(const char *filename, const char *mode);
int       bp_cfe_fclose(bp_file_t stream);
size_t    bp_cfe_fread(void *ptr, size_t size, size_t count, bp_file_t stream);
size_t    bp_cfe_fwrite(const void *ptr, size_t size, size_t count, bp_file_t stream);
size_t    bp_cfe_fwrite(const void *ptr, size_t size, size_t count, bp_file_t stream);
int       bp_cfe_fseek(bp_file_t stream, long offset, int origin);
int       bp_cfe_fflush(bp_file_t stream);

#endif /* _bplib_store_file_custom_h_ */
