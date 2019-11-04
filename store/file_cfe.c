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
BP_FILE bp_cfe_fopen(const char* filename, const char* mode)
{
    if(mode[0] == 'r')
    {
        int32 rc = OS_open(filename, OS_READ_ONLY, 0xFFFFFFFF);
        return (rc >= 0) ? rc : OS_FS_ERROR;
    }
    else if(mode[0] == 'w')
    {
        int32 rc = OS_open(filename, OS_WRITE_ONLY, 0xFFFFFFFF);
        return (rc >= 0) ? rc : OS_FS_ERROR;        
    }
    else if(mode[0] == 'a')
    {
        int32 rc = OS_open(filename, OS_WRITE_ONLY, 0xFFFFFFFF);
        if(rc >= 0)
        {
            OS_lseek(rc, 0, OS_SEEK_END);
            return rc;
        }
        else
        {
            return OS_FS_ERROR;
        }
    }
    else
    {
        return OS_FS_ERROR;
    }
}


/*--------------------------------------------------------------------------------------
 * bp_cfe_fclose -
 *-------------------------------------------------------------------------------------*/
int bp_cfe_fclose(BP_FILE stream)
{
    return (OS_close(stream) == OS_FS_SUCCESS) ? 0 : OS_FS_ERROR;
}

/*--------------------------------------------------------------------------------------
 * bp_cfe_fread -
 *-------------------------------------------------------------------------------------*/
size_t bp_cfe_fread(void* ptr, size_t size, size_t count, BP_FILE stream)
{
    return OS_read(stream, ptr, size * count);
}

/*--------------------------------------------------------------------------------------
 * bp_cfe_fwrite -
 *-------------------------------------------------------------------------------------*/
size_t bp_cfe_fwrite(const void* ptr, size_t size, size_t count, BP_FILE stream)
{
    return OS_write(stream, ptr, size * count);
}

/*--------------------------------------------------------------------------------------
 * bp_cfe_fseek -
 *-------------------------------------------------------------------------------------*/
int bp_cfe_fseek(BP_FILE stream, long int offset, int origin)
{
    return OS_lseek(stream, offset, origin);
}

/*--------------------------------------------------------------------------------------
 * bp_cfe_fflush -
 *-------------------------------------------------------------------------------------*/
int bp_cfe_fflush(BP_FILE stream)
{
    (void)stream;
    return 0;
}