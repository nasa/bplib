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

#define FILE_MAX_STORES         16
#define FILE_FLUSH_DEFAULT      false
#define FILE_MAX_FILENAME       32
#define FILE_DATA_COUNT         256
#define FILE_DATA_CACHE_SIZE    16384

/******************************************************************************
 MACROS
 ******************************************************************************/

#define GET_FILEID(did)     ((did) >> 8)
#define GET_DATAOFFSET(did) ((did) & 0xFF)

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    void*           mem_ptr;
    uint32_t        mem_size;
    uint32_t        mem_data_id;
} data_cache_t;

typedef struct {
    bool            freed[FILE_DATA_COUNT];
    int             free_cnt;
} free_table_t;

typedef struct {
    bool            in_use;

    FILE*           write_fd;
    uint32_t        write_data_id;
    bool            write_error;
    
    FILE*           read_fd;
    uint32_t        read_data_id;
    bool            read_error;

    FILE*           retrieve_fd;
    uint32_t        retrieve_data_id;

    FILE*           relinquish_fd;
    uint32_t        relinquish_data_id;
    free_table_t    relinquish_table;         

    uint32_t        active_cnt;
    
    data_cache_t    data_cache[FILE_DATA_CACHE_SIZE];
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
            unsigned int pos;
            int status;
            
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
        fs->active_cnt++;

        /* Close Write File */
        if(fs->write_data_id % FILE_DATA_COUNT == 0)
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
    uint32_t bytes_read = 0;
    uint32_t data_size = 0;
    unsigned char* data_ptr = NULL;
    uint32_t cache_index = 0;

    /* Get IDs */
    uint32_t file_id = GET_FILEID(fs->read_data_id);
    uint32_t data_offset = GET_DATAOFFSET(fs->read_data_id);
    uint32_t data_id = fs->read_data_id;
    
    /* Check Need to Open Read File */
    if(fs->read_fd == NULL)
    {
        /* Open Read File */
        char filename[FILE_MAX_FILENAME];
        snprintf(filename, FILE_MAX_FILENAME, "%u.dat", file_id);
        fs->read_fd = fopen(filename, "rb");
        if(fs->read_fd == NULL) 
        {
            return BP_FAILEDSTORE;
        }
    }

    /* Seek to Current Position */
    if(fs->read_error)
    {
        uint32_t current_size;
        unsigned int pos;
        int status;

        /* Start at Beginning of File */
        status = fseek(fs->read_fd, 0, SEEK_SET);
        if(status < 0) return BP_FAILEDSTORE;

        for(pos = 0; pos < data_offset; pos++)
        {
            /* Read Current Data Size */
            bytes_read = fread(&current_size, 1, sizeof(current_size), fs->read_fd);
            if(bytes_read != sizeof(current_size)) return BP_FAILEDSTORE;

            /* Seek to End of Current Data */
            status = fseek(fs->read_fd, current_size, SEEK_CUR);
            if(status < 0) return BP_FAILEDSTORE;
        }
    }
    
    /* Read Data */
    bytes_read = fread(&data_size, 1, sizeof(data_size), fs->read_fd);
    if(bytes_read == sizeof(data_size))
    {
        data_ptr = (unsigned char*)malloc(data_size);
        bytes_read = fread(data_ptr, 1, data_size, fs->read_fd);
        if(bytes_read == data_size)
        {
            fs->read_error = false;
            fs->read_data_id++;

            /* Close Read File */
            if(fs->read_data_id % FILE_DATA_COUNT == 0)
            {
                fclose(fs->read_fd);
                fs->read_fd = NULL;
            }
        }
        else
        {
            fs->read_error = true;
            free(data_ptr);
            
            /* Close Read File */
            if(fs->read_fd)
            {
                fclose(fs->read_fd);
                fs->read_fd = NULL;
            }
            
            /* Return Failure */
            return BP_FAILEDSTORE;
        }
    }
    
    /* Set Data Cache */
    cache_index = data_id % FILE_DATA_CACHE_SIZE;
    if(fs->data_cache[cache_index].mem_ptr) free(fs->data_cache[cache_index].mem_ptr);
    fs->data_cache[cache_index].mem_ptr = data_ptr;
    fs->data_cache[cache_index].mem_size = data_size;
    fs->data_cache[cache_index].mem_data_id = data_id;
    
    /* Set Function Parameters and Return Success */
    *data = data_ptr;
    if(size) *size = data_size;
    *sid = (bp_sid_t)(unsigned long)data_id;
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_pfile_retrieve -
 *-------------------------------------------------------------------------------------*/
int bplib_store_pfile_retrieve (int handle, void** data, int* size, bp_sid_t sid, int timeout)
{
    assert(handle >= 0 && handle < FILE_MAX_STORES);
    assert(file_stores[handle].in_use);
    
    (void)timeout;
    
    /* Initialize Variables */
    file_store_t* fs = (file_store_t*)&file_stores[handle];
    int seek_status = 0;
    uint32_t bytes_read = 0;
    uint32_t data_size = 0;
    unsigned char* data_ptr = NULL;
    uint32_t cache_index = 0;
    int offset_delta = 0;

    /* Get IDs */
    uint32_t data_id = (uint32_t)(unsigned long)sid;
    uint32_t file_id = GET_FILEID(data_id);
    uint32_t data_offset = GET_DATAOFFSET(data_id);
    uint32_t curr_file_id = GET_FILEID(fs->retrieve_data_id);
    uint32_t curr_data_offset = GET_DATAOFFSET(fs->retrieve_data_id);
            
    /* Check Data Cache */
    cache_index = data_id;
    if(fs->data_cache[cache_index].mem_ptr)
    {
        if(fs->data_cache[cache_index].mem_data_id == data_id)
        {
            /* Return Data Cache */
            *data = fs->data_cache[cache_index].mem_ptr;
            if(size) *size = fs->data_cache[cache_index].mem_size;
            return BP_SUCCESS;    
        }
    }
    
    /* Check Need to Open New Retrieve File */
    if(file_id != curr_file_id)
    {
        if(fs->retrieve_fd)
        {
            fclose(fs->retrieve_fd);
            fs->retrieve_fd = NULL;
        }
    }
    
    /* Check Need to Open Retrieve File */
    if(fs->retrieve_fd == NULL)
    {        
        /* Open Retrieve File */
        char filename[FILE_MAX_FILENAME];
        snprintf(filename, FILE_MAX_FILENAME, "%u.dat", file_id);
        fs->retrieve_fd = fopen(filename, "rb");
        if(fs->retrieve_fd == NULL) 
        {
            return BP_FAILEDSTORE;
        }

    }
    else
    {
        /* Set Delta from Current Position in File */
        offset_delta = data_offset - curr_data_offset;
        if(offset_delta < 0)
        {
            /* Handling Seeking Backwards */
            offset_delta = data_offset;
            seek_status = fseek(fs->retrieve_fd, 0, SEEK_SET);
            if(seek_status < 0) return BP_FAILEDSTORE;        
        }
    }            
    
    /* Seek Forward */
    if(offset_delta > 0)
    {
        uint32_t current_size;
        int pos;

        for(pos = 0; pos < offset_delta; pos++)
        {
            /* Read Current Data Size */
            bytes_read = fread(&current_size, 1, sizeof(current_size), fs->retrieve_fd);
            if(bytes_read != sizeof(current_size)) return BP_FAILEDSTORE;

            /* Seek to End of Current Data */
            seek_status = fseek(fs->retrieve_fd, current_size, SEEK_CUR);
            if(seek_status < 0) return BP_FAILEDSTORE;
        }
    }
    
    /* Read Data */
    bytes_read = fread(&data_size, 1, sizeof(data_size), fs->retrieve_fd);
    if(bytes_read == sizeof(data_size))
    {
        data_ptr = (unsigned char*)malloc(data_size);
        bytes_read = fread(data_ptr, 1, data_size, fs->retrieve_fd);
        if(bytes_read == data_size)
        {
            fs->retrieve_data_id = data_id;
        }
        else
        {
            free(data_ptr);
            
            /* Close Read File */
            if(fs->read_fd)
            {
                fclose(fs->read_fd);
                fs->read_fd = NULL;
            }
            
            /* Return Failure */
            return BP_FAILEDSTORE;
        }
    }
    
    /* Set Data Cache */
    cache_index = data_id % FILE_DATA_CACHE_SIZE;
    if(fs->data_cache[cache_index].mem_ptr) free(fs->data_cache[cache_index].mem_ptr);
    fs->data_cache[cache_index].mem_ptr = data_ptr;
    fs->data_cache[cache_index].mem_size = data_size;
    fs->data_cache[cache_index].mem_data_id = data_id;

    /* Set Function Parameters and Return Success */
    *data = data_ptr;
    if(size) *size = data_size;
    return BP_SUCCESS;    
}

/*--------------------------------------------------------------------------------------
 * bplib_store_pfile_relinquish -
 *-------------------------------------------------------------------------------------*/
int bplib_store_pfile_relinquish (int handle, bp_sid_t sid)
{
    assert(handle >= 0 && handle < FILE_MAX_STORES);
    assert(file_stores[handle].in_use);

    /* Initialize Variables */
    file_store_t* fs = (file_store_t*)&file_stores[handle];
    uint32_t cache_index = 0;
    uint32_t bytes_read = 0;
    uint32_t bytes_written = 0;

    /* Get IDs */
    uint32_t data_id = (uint32_t)(unsigned long)sid;
    uint32_t file_id = GET_FILEID(data_id);
    uint32_t data_offset = GET_DATAOFFSET(data_id);
    uint32_t curr_file_id = GET_FILEID(fs->relinquish_data_id);

    /* Check Data Cache */
    cache_index = data_id;
    if(fs->data_cache[cache_index].mem_ptr)
    {
        if(fs->data_cache[cache_index].mem_data_id == data_id)
        {
            free(fs->data_cache[cache_index].mem_ptr);
            fs->data_cache[cache_index].mem_ptr = NULL;
        }
    }

    /* Check Need to Read New Relinquish Table */
    if(file_id != curr_file_id)
    {
        char filename[FILE_MAX_FILENAME];

        if(fs->relinquish_table.free_cnt > 0)
        {
            /* Open Previous Relinquish File */
            if(fs->relinquish_fd == NULL)
            {
                snprintf(filename, FILE_MAX_FILENAME, "%u.tbl", file_id);
                fs->relinquish_fd = fopen(filename, "wb");
                if(fs->relinquish_fd == NULL) return BP_FAILEDSTORE;
            }

            /* Write Previous Relinquish Table */
            bytes_written = fwrite(&fs->relinquish_table, 1, sizeof(fs->relinquish_table), fs->relinquish_fd);

            /* Close Previous Relinquish File */
            fclose(fs->relinquish_fd);
            fs->relinquish_fd = NULL;

            /* Check Status of Write */
            if(bytes_written != sizeof(fs->relinquish_table))
            {
                return BP_FAILEDSTORE;
            }
        }

        /* Open New Relinquish File */
        snprintf(filename, FILE_MAX_FILENAME, "%u.tbl", file_id);
        fs->relinquish_fd = fopen(filename, "rb");
        if(fs->relinquish_fd == NULL)
        {
            /* Initialize New Relinquish Table */
            memset(&fs->relinquish_table, 0, sizeof(fs->relinquish_table));
        }
        else
        {
            /* Read Relinquish Table */
            bytes_read = fread(&fs->relinquish_table, 1, sizeof(fs->relinquish_table), fs->relinquish_fd);
            if(bytes_read != sizeof(fs->relinquish_table))
            {
                fclose(fs->relinquish_fd);
                fs->relinquish_fd = NULL;
                return BP_FAILEDSTORE;
            }
        }
        
        /* Close New Relinquish File */
        fclose(fs->relinquish_fd);
        fs->relinquish_fd = NULL;

        /* Set Current Relinquish Table */
        fs->relinquish_data_id = data_id;

    }
    
    /* Mark Data as Relinquished */
    fs->relinquish_data_id = data_id;
    if(fs->relinquish_table.freed[data_offset] == 0)
    {
        fs->relinquish_table.freed[data_offset] = 1;
        fs->relinquish_table.free_cnt++;
        fs->active_cnt--;
        if(fs->relinquish_table.free_cnt == FILE_DATA_COUNT)
        {
            char filename[FILE_MAX_FILENAME];
            int status;

            /* Close Write File */
            if(fs->write_fd)
            {
                fclose(fs->write_fd);
                fs->write_fd = NULL;
            }

            /* Delete Write File */
            snprintf(filename, FILE_MAX_FILENAME, "%u.dat", file_id);
            status = remove(filename);
            if(status < 0) return BP_FAILEDSTORE;
            
            /* Delete Relinquish File */
            snprintf(filename, FILE_MAX_FILENAME, "%u.tbl", file_id);
            status = remove(filename);
            if(status < 0) return BP_FAILEDSTORE;
        }
    }

    /* Return Success */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_pfile_getcount -
 *-------------------------------------------------------------------------------------*/
int bplib_store_pfile_getcount (int handle)
{
    assert(handle >= 0 && handle < FILE_MAX_STORES);
    assert(file_stores[handle].in_use);

    file_store_t* fs = (file_store_t*)&file_stores[handle];

    return fs->active_cnt;
}
