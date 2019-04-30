/************************************************************************
 * File: bplib_store_pfile.c
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

#include "bplib.h"
#include "bplib_os.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#ifndef FILE_MAX_STORES
#define FILE_MAX_STORES     16
#endif

#ifndef FILE_FLUSH_DEFAULT
#define FILE_FLUSH_DEFAULT  false
#endif

#define FILE_MAX_FILENAME   32
#define FILE_TABLE_SIZE     256

/******************************************************************************
 MACROS
 ******************************************************************************/

#define GET_FILEID(did)         ((did) >> 8)
#define GET_DATAOFFSET(did)   ((did) & 0xFF)

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    bool        in_use;

    FILE*       write_fd;
    uint32_t    write_data_id;
    bool        write_error;
    
    FILE*       read_fd;
    uint32_t    read_data_id;
    bool        read_error;

    FILE*       retrieve_fd;

    uint32_t    active_cnt;
} file_store_t;

/******************************************************************************
 FILE DATA
 ******************************************************************************/

static file_store_t file_stores[FILE_MAX_STORES];
static bool file_flush = FILE_FLUSH_DEFAULT;

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_store_pfile_init -
 *-------------------------------------------------------------------------------------*/
void bplib_store_pfile_init (void)
{
    memset(file_stores, 0, sizeof(file_stores));
}

/*--------------------------------------------------------------------------------------
 * bplib_store_pfile_create -
 *-------------------------------------------------------------------------------------*/
int bplib_store_pfile_create (void)
{
    int s;
    for(s = 0; s < FILE_MAX_STORES; s++)
    {
        if(file_stores[s].in_use == false)
        {
            memset(&file_stores[s], 0, sizeof(file_stores[s]));
            file_stores[s].in_use = true;
            return s;
        }
    }

    return BP_INVALID_HANDLE;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_pfile_destroy -
 *-------------------------------------------------------------------------------------*/
int bplib_store_pfile_destroy (int handle)
{
    assert(handle >= 0 && handle < FILE_MAX_STORES);
    assert(file_stores[handle].in_use);

    if(file_stores[handle].write_fd) fclose(file_stores[handle].write_fd);
    if(file_stores[handle].read_fd) fclose(file_stores[handle].read_fd);
    if(file_stores[handle].retrieve_fd) fclose(file_stores[handle].retrieve_fd);

    return 0;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_pfile_enqueue -
 *-------------------------------------------------------------------------------------*/
int bplib_store_pfile_enqueue (int handle, void* data1, int data1_size, void* data2, int data2_size, int timeout)
{
    (void)timeout;

    assert(handle >= 0 && handle < FILE_MAX_STORES);
    assert(file_stores[handle].in_use);

    /* Initialize Variables */
    file_store_t* fs = (file_store_t*)&file_stores[handle];
    uint32_t data_size = data1_size + data2_size;
    uint32_t bytes_written = 0;
    bool flush_error = false;
    
    /* Get IDs */
    uint32_t file_id = GET_FILEID(fs->write_data_id);
    uint32_t data_offset = GET_DATAOFFSET(fs->write_data_id);
    
    /* Check Need to Open Write File */
    if(fs->write_fd == NULL)
    {
        /* Open Write File */
        char filename[FILE_MAX_FILENAME];
        snprintf(filename, FILE_MAX_FILENAME, "%u.dat", file_id);
        fs->write_fd = fopen(filename, "ab");
        
        /* Seek to Current Position */
        if(fs->write_error)
        {
            uint32_t current_size;
            uint32_t bytes_read;
            int status, pos;
            
            /* Start at Beginning of File */
            status = fseek(fs->write_fd, 0, SEEK_SET);
            if(status < 0) return BP_FAILEDSTORE;
            
            for(pos = 0; pos < data_offset; pos++)
            {
                /* Read Current Data Size */
                bytes_read = fread(&current_size, 1, sizeof(current_size), fs->write_fd);
                if(bytes_read != sizeof(current_size)) return BP_FAILEDSTORE;

                /* Seek to End of Current Data */
                status = fseek(fs->write_fd, current_size, SEEK_CUR);
                if(status < 0) return BP_FAILEDSTORE;
            }
        }
    }

    /* Check Write File Open */
    if(fs->write_fd != NULL)
    {
        /* Write Data Size */
        bytes_written += fwrite(&data_size, 1, sizeof(data_size), fs->write_fd);
        
        /* Write Data Buffer 1 */
        bytes_written += fwrite(data1, 1, data1_size, fs->write_fd);

        /* Write Data Buffer 2 */
        bytes_written += fwrite(data2, 1, data2_size, fs->write_fd);
        
        /* Flush File Data */
        if(file_flush)
        {
            int status = fflush(fs->write_fd);
            if(status < 0) flush_error = true;
        }
    }

    /* Check Errors and Return Status */
    if((bytes_written != (data_size + sizeof(data_size))) || flush_error)
    {
        fs->write_error = true;
        if(fs->write_fd)
        {
            fclose(fs->write_fd);
            fs->write_fd = NULL;
        }
        
        /* Return Failure */
        return BP_FAILEDSTORE;
    }
    else
    {
        fs->write_error = false;
        fs->write_data_id++;

        /* Close Write File */
        if(fs->write_data_id % FILE_TABLE_SIZE == 0)
        {
            fclose(fs->write_fd);
            fs->write_fd = NULL;
        }

        /* Return Success */
        return bytes_written;
    }
}

/*--------------------------------------------------------------------------------------
 * bplib_store_pfile_dequeue -
 *-------------------------------------------------------------------------------------*/
int bplib_store_pfile_dequeue (int handle, void** data, int* size, bp_sid_t* sid, int timeout)
{
    (void)timeout;

    assert(handle >= 0 && handle < FILE_MAX_STORES);
    assert(file_stores[handle].in_use);
    assert(data);
    assert(sid);

    /* Initialize Variables */
    file_store_t* fs = (file_store_t*)&file_stores[handle];
    size_t read_status = 0;
    uint32_t data_position = 0;
    uint32_t data_size = 0;
    unsigned char* data_ptr = NULL;

    /* Get IDs */
    uint32_t file_id = GET_FILEID(fs->read_data_id);
    uint32_t data_offset = GET_DATAOFFSET(fs->read_data_id);
    uint32_t data_id = fs->read_data_id;
    
    /* Check Need to Open Read File */
    if(fs->read_fd == NULL)
    {
        char filename[FILE_MAX_FILENAME];

        /* Open Read Table File */
        snprintf(filename, FILE_MAX_FILENAME, "%u.tbl", file_id);
        fs->read_fd = fopen(filename, "rb");
        if(fs->read_fd == NULL) 
        {
            return BP_FAILEDSTORE;
        }

        /* Populate Read Table */
        read_status = fread(fs->read_table, 1, sizeof(fs->read_table), fs->read_fd);
        fclose(fs->read_fd);
        fs->read_fd = NULL;
        if(read_status != sizeof(fs->read_table)) 
        {
            return BP_FAILEDSTORE;
        }
        
        /* Open Read File */
        snprintf(filename, FILE_MAX_FILENAME, "%u.dat", file_id);
        fs->read_fd = fopen(filename, "rb");
        if(fs->read_fd == NULL) 
        {
            return BP_FAILEDSTORE;
        }
    }

    /* Check Read Data ID */
    fs->read_data_id++;
    if(fs->read_table[data_offset] == 0)
    {
        /*
         * A value of zero in the read table indicates that no data block was
         * written for the current read_data_id
         */
        return BP_FAILEDSTORE;
    }
    else if(data_offset == 0)
    {
        data_position = 0;
        data_size = fs->read_table[data_offset];
    }
    else
    {
        data_position = fs->read_table[data_offset - 1];
        data_size = fs->read_table[data_offset] - data_position;
    }
    
    /* Check if File Seek Needed */
    if(fs->read_error)
    {
        int status = fseek(fs->read_fd, data_position, SEEK_SET);
        if(status < 0) return BP_FAILEDSTORE;
    }
    
    /* Read Data */
    data_ptr = (unsigned char*)malloc(data_size);
    read_status = fread(data_ptr, 1, data_size, fs->read_fd);
    if(read_status != data_size)
    {
        fs->read_error = true;
        free(data_ptr);
        return BP_FAILEDSTORE;
    }
    else
    {
        fs->read_error = false;
    }
    
    /* Set Function Parameters and Return Success */
    *data = data_ptr;
    if(size) *size = data_size;
    *sid = (bp_sid_t)(unsigned long long)data_id;
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_pfile_retrieve -
 *-------------------------------------------------------------------------------------*/
int bplib_store_pfile_retrieve (int handle, void** data, int* size, bp_sid_t sid, int timeout)
{
    assert(handle >= 0 && handle < FILE_MAX_STORES);
    assert(file_stores[handle].in_use);

    (void)handle;
    (void)sid;
    (void)timeout;
    (void)data;
    (void)size;
    assert(data);
    assert(size);
    return 0;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_pfile_relinquish -
 *-------------------------------------------------------------------------------------*/
int bplib_store_pfile_relinquish (int handle, bp_sid_t sid)
{
    assert(handle >= 0 && handle < FILE_MAX_STORES);
    assert(file_stores[handle].in_use);

    (void)handle;
    (void)sid;
    return 0;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_pfile_getcount -
 *-------------------------------------------------------------------------------------*/
int bplib_store_pfile_getcount (int handle)
{
    assert(handle >= 0 && handle < FILE_MAX_STORES);
    assert(file_stores[handle].in_use);

    (void)handle;
    return 0;
}
