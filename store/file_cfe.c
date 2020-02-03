/************************************************************************
 * File: bplib_store_file_custom.c
 *
 * Maintainer(s):
 *  Joe-Paul Swinski, Code 582 NASA GSFC
 *
 *************************************************************************/

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdio.h>
#include "cfe.h"
#include "file_cfe.h"

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bp_cfe_fopen -
 *-------------------------------------------------------------------------------------*/
FILE* bp_cfe_fopen(const char* filename, const char* mode)
{
    int32 rc = OS_FS_ERROR;
    FILE* fhandle = NULL;

    if(mode[0] == 'r')
    {
        rc = OS_open(filename, OS_READ_ONLY, S_IRUSR | S_IWUSR);
    }
    else if(mode[0] == 'w')
    {
        rc = OS_open(filename, OS_WRITE_ONLY, S_IRUSR | S_IWUSR);
    }
    else if(mode[0] == 'a')
    {
        rc = OS_open(filename, OS_READ_WRITE, S_IRUSR | S_IWUSR);
        if(rc >= 0) OS_lseek(rc, 0, OS_SEEK_END);
    }

    if(rc >= 0)
    {
        fhandle = (FILE*)(unsigned long)(rc + 1);
    }

    return fhandle;
}

/*--------------------------------------------------------------------------------------
 * bp_cfe_fclose -
 *-------------------------------------------------------------------------------------*/
int bp_cfe_fclose(FILE* stream)
{
    int32 rc = (int32)((unsigned long)stream - 1);
    return (OS_close(rc) == OS_FS_SUCCESS) ? 0 : -1;
}

/*--------------------------------------------------------------------------------------
 * bp_cfe_fread -
 *-------------------------------------------------------------------------------------*/
size_t bp_cfe_fread(void* ptr, size_t size, size_t count, FILE* stream)
{
    int32 rc = (int32)((unsigned long)stream - 1);
    return OS_read(rc, ptr, size * count);
}

/*--------------------------------------------------------------------------------------
 * bp_cfe_fwrite -
 *-------------------------------------------------------------------------------------*/
size_t bp_cfe_fwrite(const void* ptr, size_t size, size_t count, FILE* stream)
{
    int32 rc = (int32)((unsigned long)stream - 1);
    return OS_write(rc, ptr, size * count);
}

/*--------------------------------------------------------------------------------------
 * bp_cfe_fseek -
 *-------------------------------------------------------------------------------------*/
int bp_cfe_fseek(FILE* stream, long int offset, int origin)
{
    int32 rc = (int32)((unsigned long)stream - 1);
    return OS_lseek(rc, offset, origin);
}

/*--------------------------------------------------------------------------------------
 * bp_cfe_fflush -
 *-------------------------------------------------------------------------------------*/
int bp_cfe_fflush(FILE* stream)
{
    (void)stream;
    return 0;
}