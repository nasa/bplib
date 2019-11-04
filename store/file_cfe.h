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
 FILE SYSTEM MACROS
 ******************************************************************************/

#define BP_FILE         int32
#define BP_FILE_NULL    OS_FS_ERROR
#define BP_FILE_OPEN    bp_cfe_fopen
#define BP_FILE_CLOSE   bp_cfe_fclose
#define BP_FILE_READ    bp_cfe_fread
#define BP_FILE_WRITE   bp_cfe_fwrite
#define BP_FILE_SEEK    bp_cfe_fseek
#define BP_FILE_FLUSH   bp_cfe_fflush    

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

BP_FILE bp_cfe_fopen    (const char* filename, const char* mode);
int     bp_cfe_fclose   (BP_FILE stream);
size_t  bp_cfe_fread    (void* ptr, size_t size, size_t count, BP_FILE stream);
size_t  bp_cfe_fwrite   (const void* ptr, size_t size, size_t count, BP_FILE stream);
size_t  bp_cfe_fwrite   (const void* ptr, size_t size, size_t count, BP_FILE stream);
int     bp_cfe_fseek    (BP_FILE stream, long int offset, int origin);
int     bp_cfe_fflush    (BP_FILE stream);

#endif /* _bplib_store_file_custom_h_ */
