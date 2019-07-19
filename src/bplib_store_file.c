/************************************************************************
 * File: bplib_store_file.c
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
#include "bplib_store_file.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define FILE_MAX_STORES         16
#define FILE_FLUSH_DEFAULT      true
#define FILE_MAX_FILENAME       256
#define FILE_DATA_COUNT         256
#define FILE_DATA_CACHE_SIZE    16384
#define FILE_DEFAULT_ROOT       ".pfile"

/******************************************************************************
 MACROS
 ******************************************************************************/

#define GET_FILEID(did)     ((uint32_t)((did) >> 8))
#define GET_DATAOFFSET(did) ((uint8_t)((did) & 0xFF))

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    void*           mem_ptr;
    uint32_t        mem_size;
    uint64_t        mem_data_id;
} data_cache_t;

typedef struct {
    bool            freed[FILE_DATA_COUNT];
    int             free_cnt;
} free_table_t;

typedef struct {
    bool            in_use;
    int             lock;
    uint64_t        service_id;
    char*           file_root;
    int             data_count;
    
    FILE*           write_fd;
    uint64_t        write_data_id;
    bool            write_error;
    
    FILE*           read_fd;
    uint64_t        read_data_id;
    bool            read_error;

    FILE*           retrieve_fd;
    uint64_t        retrieve_data_id;

    FILE*           relinquish_fd;
    uint64_t        relinquish_data_id;
    free_table_t    relinquish_table;         

    data_cache_t    data_cache[FILE_DATA_CACHE_SIZE];
} file_store_t;

/******************************************************************************
 FILE DATA
 ******************************************************************************/

static file_store_t file_stores[FILE_MAX_STORES];
static bool file_flush = FILE_FLUSH_DEFAULT;
static uint64_t file_service_id = 0;

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * open_dat_file -
 *-------------------------------------------------------------------------------------*/
static FILE* open_dat_file (int service_id, char* file_root, uint32_t file_id, bool read_only)
{
    FILE* fd;
    
    char filename[FILE_MAX_FILENAME];
    bplib_os_format(filename, FILE_MAX_FILENAME, "%s/%d_%u.dat", file_root, service_id, file_id);

    if(read_only)   fd = fopen(filename, "rb");
    else            fd = fopen(filename, "ab");

    if(fd == NULL) printf("pfile fatal error - failed to open %s data file %s: %s\n", read_only ? "read only" : "appended", filename, strerror(errno));
    
    return fd;
}

/*--------------------------------------------------------------------------------------
 * delete_dat_file -
 *-------------------------------------------------------------------------------------*/
static int delete_dat_file (int service_id, char* file_root, uint32_t file_id)
{
    char filename[FILE_MAX_FILENAME];
    bplib_os_format(filename, FILE_MAX_FILENAME, "%s/%d_%u.dat", file_root, service_id, file_id);

    return remove(filename);
}

/*--------------------------------------------------------------------------------------
 * open_tbl_file -
 *-------------------------------------------------------------------------------------*/
static FILE* open_tbl_file (int service_id, char* file_root, uint32_t file_id, bool read_only)
{
    FILE* fd;
    
    char filename[FILE_MAX_FILENAME];
    bplib_os_format(filename, FILE_MAX_FILENAME, "%s/%d_%u.tbl", file_root, service_id, file_id);

    if(read_only)   fd = fopen(filename, "rb");
    else            fd = fopen(filename, "wb");

    if(fd == NULL) printf("pfile fatal error - failed to open %s table file %s: %s\n", read_only ? "read only" : "appended", filename, strerror(errno));
    
    return fd;
}

/*--------------------------------------------------------------------------------------
 * delete_tbl_file -
 *-------------------------------------------------------------------------------------*/
static int delete_tbl_file (int service_id, char* file_root, uint32_t file_id)
{
    char filename[FILE_MAX_FILENAME];
    bplib_os_format(filename, FILE_MAX_FILENAME, "%s/%d_%u.tbl", file_root, service_id, file_id);

    return remove(filename);
}

/*--------------------------------------------------------------------------------------
 * set_root_path -
 *-------------------------------------------------------------------------------------*/
static int set_root_path (char** root_path_dst, const char* root_path_src)
{
    if(root_path_src == NULL) root_path_src = FILE_DEFAULT_ROOT;
    
    int root_path_len = bplib_os_strnlen(root_path_src, FILE_MAX_FILENAME - 1) + 1;
    if(root_path_len == (FILE_MAX_FILENAME - 1)) return BP_PARMERR;
    
    if(*root_path_dst) free(*root_path_dst);
    *root_path_dst = (char*)malloc(root_path_len);
    
    memcpy(*root_path_dst, root_path_src, root_path_len);

    return BP_SUCCESS;
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_store_file_init -
 *-------------------------------------------------------------------------------------*/
void bplib_store_file_init (void)
{
    memset(file_stores, 0, sizeof(file_stores));
}

/*--------------------------------------------------------------------------------------
 * bplib_store_file_create -
 *-------------------------------------------------------------------------------------*/
int bplib_store_file_create (void* parm)
{
    pfile_attr_t* attr = (pfile_attr_t*)parm;
    
    int s;
    for(s = 0; s < FILE_MAX_STORES; s++)
    {
        if(file_stores[s].in_use == false)
        {
            memset(&file_stores[s], 0, sizeof(file_stores[s]));
            file_stores[s].in_use = true;
            file_stores[s].service_id = file_service_id++;
            set_root_path(&file_stores[s].file_root, attr ? attr->root_path : NULL);
            file_stores[s].lock = bplib_os_createlock();
            file_stores[s].write_data_id = 1;
            file_stores[s].read_data_id = 1;
            file_stores[s].retrieve_data_id = 1;
            file_stores[s].relinquish_data_id = 1;
            return s;
        }
    }

    return BP_INVALID_HANDLE;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_file_destroy -
 *-------------------------------------------------------------------------------------*/
int bplib_store_file_destroy (int handle)
{
    assert(handle >= 0 && handle < FILE_MAX_STORES);
    assert(file_stores[handle].in_use);

    if(file_stores[handle].write_fd) fclose(file_stores[handle].write_fd);
    if(file_stores[handle].read_fd) fclose(file_stores[handle].read_fd);
    if(file_stores[handle].retrieve_fd) fclose(file_stores[handle].retrieve_fd);

    bplib_os_destroylock(file_stores[handle].lock);
    
    file_stores[handle].in_use = false;
    
    return 0;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_file_enqueue -
 *-------------------------------------------------------------------------------------*/
int bplib_store_file_enqueue (int handle, void* data1, int data1_size, void* data2, int data2_size, int timeout)
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
        fs->write_fd = open_dat_file(fs->service_id, fs->file_root, file_id, false);
        if(fs->write_fd == NULL) return BP_FAILEDSTORE;
        
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
        bplib_os_signal(fs->lock);

        /* Close Write File */
        if(fs->write_data_id % FILE_DATA_COUNT == 0)
        {
            fclose(fs->write_fd);
            fs->write_fd = NULL;
        }

        /* Return Success */
        fs->data_count++;
        return bytes_written;
    }
}

/*--------------------------------------------------------------------------------------
 * bplib_store_file_dequeue -
 *-------------------------------------------------------------------------------------*/
int bplib_store_file_dequeue (int handle, void** data, int* size, bp_sid_t* sid, int timeout)
{
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

    /* Check if Data Available */
    if(fs->read_data_id == fs->write_data_id)
    {
        int wait_status;
        bplib_os_lock(fs->lock);
        {
            wait_status = bplib_os_waiton(fs->lock, timeout);
        }
        bplib_os_unlock(fs->lock);
        if(wait_status == BP_OS_TIMEOUT) return BP_TIMEOUT;
        else if(wait_status == BP_OS_ERROR) return BP_FAILEDSTORE;
        else if(fs->read_data_id == fs->write_data_id) return BP_TIMEOUT;
    }
        
    /* Check Need to Open Read File */
    if(fs->read_fd == NULL)
    {
        /* Open Read File */
        fs->read_fd = open_dat_file(fs->service_id, fs->file_root, file_id, true);
        if(fs->read_fd == NULL) return BP_FAILEDSTORE;
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
    bplib_os_lock(fs->lock);
    {
        if(fs->data_cache[cache_index].mem_ptr) free(fs->data_cache[cache_index].mem_ptr);
        fs->data_cache[cache_index].mem_ptr = data_ptr;
        fs->data_cache[cache_index].mem_size = data_size;
        fs->data_cache[cache_index].mem_data_id = data_id;
    }
    bplib_os_unlock(fs->lock);
    
    /* Set Function Parameters and Return Success */
    *data = data_ptr;
    if(size) *size = data_size;
    *sid = (bp_sid_t)(unsigned long)data_id;
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_file_retrieve -
 *-------------------------------------------------------------------------------------*/
int bplib_store_file_retrieve (int handle, void** data, int* size, bp_sid_t sid, int timeout)
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
    bplib_os_lock(fs->lock);
    {
        if(fs->data_cache[cache_index].mem_ptr)
        {
            if(fs->data_cache[cache_index].mem_data_id == data_id)
            {
                /* Return Data Cache */
                *data = fs->data_cache[cache_index].mem_ptr;
                if(size) *size = fs->data_cache[cache_index].mem_size;
                bplib_os_unlock(fs->lock);
                return BP_SUCCESS;    
            }
        }
    }
    bplib_os_unlock(fs->lock);
    
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
        fs->retrieve_fd = open_dat_file(fs->service_id, fs->file_root, file_id, true);
        if(fs->retrieve_fd == NULL) return BP_FAILEDSTORE;
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
            if(fs->retrieve_fd)
            {
                fclose(fs->retrieve_fd);
                fs->retrieve_fd = NULL;
            }
            
            /* Return Failure */
            return BP_FAILEDSTORE;
        }
    }
    
    /* Set Data Cache */
    cache_index = data_id % FILE_DATA_CACHE_SIZE;
    bplib_os_lock(fs->lock);
    {
        if(fs->data_cache[cache_index].mem_ptr) free(fs->data_cache[cache_index].mem_ptr);
        fs->data_cache[cache_index].mem_ptr = data_ptr;
        fs->data_cache[cache_index].mem_size = data_size;
        fs->data_cache[cache_index].mem_data_id = data_id;
    }
    bplib_os_unlock(fs->lock);

    /* Set Function Parameters and Return Success */
    *data = data_ptr;
    if(size) *size = data_size;
    return BP_SUCCESS;    
}

/*--------------------------------------------------------------------------------------
 * bplib_store_file_relinquish -
 *-------------------------------------------------------------------------------------*/
int bplib_store_file_relinquish (int handle, bp_sid_t sid)
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
    bplib_os_lock(fs->lock);
    {
        if(fs->data_cache[cache_index].mem_ptr)
        {
            if(fs->data_cache[cache_index].mem_data_id == data_id)
            {
                free(fs->data_cache[cache_index].mem_ptr);
                fs->data_cache[cache_index].mem_ptr = NULL;
            }
        }
    }
    bplib_os_unlock(fs->lock);

    /* Check Need to Read New Relinquish Table */
    if(file_id != curr_file_id)
    {
        if(fs->relinquish_table.free_cnt > 0)
        {
            /* Open Previous Relinquish File */
            if(fs->relinquish_fd == NULL)
            {
                fs->relinquish_fd = open_tbl_file(fs->service_id, fs->file_root, file_id, false);
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
        fs->relinquish_fd = open_tbl_file(fs->service_id, fs->file_root, file_id, true);
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

        /* Relinquish Resources */
        fs->relinquish_table.free_cnt++;
        if(fs->relinquish_table.free_cnt == FILE_DATA_COUNT)
        {
            int status;

            /* Delete Write File */
            status = delete_dat_file(fs->service_id, fs->file_root, file_id);
            if(status < 0) return BP_FAILEDSTORE;
            
            /* Delete Relinquish File */
            status = delete_tbl_file(fs->service_id, fs->file_root, file_id);
            if(status < 0) return BP_FAILEDSTORE;
        }
    }

    /* Return Success */
    fs->data_count--;
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_file_getcount -
 *-------------------------------------------------------------------------------------*/
int bplib_store_file_getcount (int handle)
{
    assert(handle >= 0 && handle < FILE_MAX_STORES);
    assert(file_stores[handle].in_use);

    file_store_t* fs = (file_store_t*)&file_stores[handle];

    return fs->data_count;
}
