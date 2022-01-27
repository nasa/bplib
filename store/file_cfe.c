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

typedef union
{
    bp_file_t bpfhandle;
    osal_id_t OsHandle;
} bp_handlewrap_t;

static inline osal_id_t bpfile_to_osalhdl(bp_file_t stream)
{
    bp_handlewrap_t wrap = {.bpfhandle = stream};
    return wrap.OsHandle;
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bp_cfe_fopen -
 *-------------------------------------------------------------------------------------*/
bp_file_t bp_cfe_fopen(const char *filename, const char *mode)
{
    int32           rc;
    bp_handlewrap_t HandleBuf;
    int32           OsAccessMode;
    bool            SeekToEnd;

    SeekToEnd           = false;
    rc                  = OS_SUCCESS;
    HandleBuf.bpfhandle = NULL;

    if (mode[0] == 'r')
    {
        OsAccessMode = OS_READ_ONLY;
    }
    else if (mode[0] == 'w')
    {
        OsAccessMode = OS_WRITE_ONLY;
    }
    else if (mode[0] == 'a')
    {
        OsAccessMode = OS_READ_WRITE;
        SeekToEnd    = true;
    }
    else
    {
        rc = OS_ERROR;
    }

    rc = OS_OpenCreate(&HandleBuf.OsHandle, filename, 0, OsAccessMode);

    if (rc == OS_SUCCESS && SeekToEnd)
    {
        OS_lseek(HandleBuf.OsHandle, 0, OS_SEEK_END);
    }

    if (rc < OS_SUCCESS)
    {
        return NULL;
    }

    return HandleBuf.bpfhandle;
}

/*--------------------------------------------------------------------------------------
 * bp_cfe_fclose -
 *-------------------------------------------------------------------------------------*/
int bp_cfe_fclose(bp_file_t stream)
{
    if (OS_close(bpfile_to_osalhdl(stream)) != OS_SUCCESS)
    {
        return -1;
    }

    return 0;
}

/*--------------------------------------------------------------------------------------
 * bp_cfe_fread -
 *-------------------------------------------------------------------------------------*/
size_t bp_cfe_fread(void *ptr, size_t size, size_t count, bp_file_t stream)
{
    int32 rc = OS_read(bpfile_to_osalhdl(stream), ptr, size * count);
    if (rc < OS_SUCCESS)
    {
        return -1;
    }

    return rc / size;
}

/*--------------------------------------------------------------------------------------
 * bp_cfe_fwrite -
 *-------------------------------------------------------------------------------------*/
size_t bp_cfe_fwrite(const void *ptr, size_t size, size_t count, bp_file_t stream)
{
    int32 rc = OS_write(bpfile_to_osalhdl(stream), ptr, size * count);
    if (rc < OS_SUCCESS)
    {
        return -1;
    }

    return rc / size;
}

/*--------------------------------------------------------------------------------------
 * bp_cfe_fseek -
 *-------------------------------------------------------------------------------------*/
int bp_cfe_fseek(bp_file_t stream, long offset, int origin)
{
    int32  rc;
    uint32 osal_whence;

    switch (origin)
    {
        case SEEK_CUR:
            osal_whence = OS_SEEK_CUR;
            break;
        case SEEK_END:
            osal_whence = OS_SEEK_END;
            break;
        default:
            osal_whence = OS_SEEK_SET;
            break;
    }

    rc = OS_lseek(bpfile_to_osalhdl(stream), offset, osal_whence);

    if (rc < OS_SUCCESS)
    {
        return -1;
    }

    return 0;
}

/*--------------------------------------------------------------------------------------
 * bp_cfe_fflush -
 *-------------------------------------------------------------------------------------*/
int bp_cfe_fflush(bp_file_t stream)
{
    (void)stream;
    return 0;
}
