/************************************************************************
 * File: file.c
 *
 *  Copyright 2019 United States Government as represented by the
 *  Administrator of the National Aeronautics and Space Administration.
 *  All Other Rights Reserved.
 *
 *  This software was created at NASA's Goddard Space Flight Center.
 *  This software is governed by the NASA Open Source Agreement and may be
 *  used, distributed and modified only pursuant to the terms of that
 *  agreement.
 *
 * Maintainer(s):
 *  Joe-Paul Swinski, Code 582 NASA GSFC
 *
 *************************************************************************/

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bplib_store_file.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define FILE_FLUSH_DEFAULT      true
#define FILE_MAX_FILENAME       256
#define FILE_DATA_COUNT         256 /* Cannot be changed without changing macros */
#define FILE_MEM_LOCKED         1
#define FILE_MEM_AVAIABLE       0

/* Dynamically Set Attributes */

#define FILE_DEFAULT_CACHE_SIZE 16384
#define FILE_DEFAULT_ROOT       ".pfile"

/* Configurable Options */

#ifndef FILE_MAX_STORES
#define FILE_MAX_STORES         60
#endif

/******************************************************************************
 MACROS
 ******************************************************************************/

#define GET_DATAID(sid)     (sid - 1)
#define GET_FILEID(did)     ((did) >> 8)
#define GET_DATAOFFSET(did) ((uint8_t)((did) & 0xFF))

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    void*           mem_ptr;
    bool            mem_locked;
    unsigned long   mem_data_id;
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
    unsigned long   write_data_id;
    bool            write_error;

    FILE*           read_fd;
    unsigned long   read_data_id;
    bool            read_error;

    FILE*           retrieve_fd;
    unsigned long  retrieve_data_id;

    FILE*           relinquish_fd;
    unsigned long   relinquish_data_id;
    free_table_t    relinquish_table;

    data_cache_t*   data_cache;
    int             cache_size;
    bool            flush_on_write;
} file_store_t;

/******************************************************************************
 FILE DATA
 ******************************************************************************/

static file_store_t file_stores[FILE_MAX_STORES];

static uint64_t file_service_id = 0;

static bp_file_driver_t file_driver = {
    .open   = fopen,
    .close  = fclose,
    .read   = fread,
    .write  = fwrite,
    .seek   = fseek,
    .flush  = fflush
};

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * open_dat_file -
 *-------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE FILE* open_dat_file (int service_id, char* file_root, uint32_t file_id, bool read_only)
{
    FILE* fd;

    char filename[FILE_MAX_FILENAME];
    bplib_os_format(filename, FILE_MAX_FILENAME, "%s/%d_%u.dat", file_root, service_id, file_id);

    if(read_only)
    {
        fd = file_driver.open(filename, "rb");
    }
    else
    {
        fd = file_driver.open(filename, "ab");
        if(fd == NULL)
        {
            bplog(BP_FAILEDSTORE, "failed to open %s data file %s: %s\n", read_only ? "read only" : "appended", filename, strerror(errno));
        }
    }

    return fd;
}

/*--------------------------------------------------------------------------------------
 * delete_dat_file -
 *-------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE int delete_dat_file (int service_id, char* file_root, uint32_t file_id)
{
    char filename[FILE_MAX_FILENAME];
    bplib_os_format(filename, FILE_MAX_FILENAME, "%s/%d_%u.dat", file_root, service_id, file_id);

    int status = remove(filename);

    if(status < 0)
    {
        return bplog(BP_FAILEDSTORE, "failed to remove %s data file: %s\n", filename, strerror(errno));
    }
    else
    {
        return status;
    }
}

/*--------------------------------------------------------------------------------------
 * open_tbl_file -
 *-------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE FILE* open_tbl_file (int service_id, char* file_root, uint32_t file_id, bool read_only)
{
    FILE* fd;

    char filename[FILE_MAX_FILENAME];
    bplib_os_format(filename, FILE_MAX_FILENAME, "%s/%d_%u.tbl", file_root, service_id, file_id);

    if(read_only)
    {
        fd = file_driver.open(filename, "rb");
    }
    else
    {
        fd = file_driver.open(filename, "wb");
        if(fd == NULL)
        {
            bplog(BP_FAILEDSTORE, "failed to open %s table file %s: %s\n", read_only ? "read only" : "appended", filename, strerror(errno));
        }
    }

    return fd;
}

/*--------------------------------------------------------------------------------------
 * delete_tbl_file -
 *-------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE int delete_tbl_file (int service_id, char* file_root, uint32_t file_id)
{
    char filename[FILE_MAX_FILENAME];
    bplib_os_format(filename, FILE_MAX_FILENAME, "%s/%d_%u.tbl", file_root, service_id, file_id);

    int status = remove(filename);

    if(status < 0 && errno != 2)
    {
        return bplog(BP_FAILEDSTORE, "failed to remove %s table file: %d %s\n", filename, errno, strerror(errno));
    }
    else
    {
        return 0;
    }
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_store_file_init -
 *-------------------------------------------------------------------------------------*/
void bplib_store_file_init (bp_file_driver_t* driver)
{
    if(driver) file_driver = *driver;

    memset(file_stores, 0, sizeof(file_stores));
}

/*--------------------------------------------------------------------------------------
 * bplib_store_file_create -
 *-------------------------------------------------------------------------------------*/
int bplib_store_file_create (void* parm)
{
    bp_file_attr_t* attr = (bp_file_attr_t*)parm;

    int s;
    for(s = 0; s < FILE_MAX_STORES; s++)
    {
        if(file_stores[s].in_use == false)
        {
            /* Clear File Store (pointers set to NULL) */
            memset(&file_stores[s], 0, sizeof(file_stores[s]));
            file_stores[s].write_fd = NULL;
            file_stores[s].read_fd = NULL;
            file_stores[s].retrieve_fd = NULL;
            file_stores[s].relinquish_fd = NULL;

            /* Set In Use (necessary to be able to destroy later) */
            file_stores[s].in_use = true;

            /* Initialize Parameters */
            file_stores[s].service_id = file_service_id++;
            file_stores[s].lock = BP_INVALID_HANDLE;
            file_stores[s].write_data_id = 1;
            file_stores[s].read_data_id = 1;
            file_stores[s].retrieve_data_id = 1;
            file_stores[s].relinquish_data_id = 1;

            /* Setup and Check Lock */
            file_stores[s].lock = bplib_os_createlock();
            if(file_stores[s].lock < 0)
            {
                bplog(BP_FAILEDOS, "Failed (%d) to create FSS lock\n", file_stores[s].lock);
                bplib_store_file_destroy(s);
                break;
            }

            /* Setup Root Path */
            const char* root_path = FILE_DEFAULT_ROOT;
            if(attr && attr->root_path) root_path = attr->root_path;
            int root_path_len = bplib_os_strnlen(root_path, FILE_MAX_FILENAME) + 1;
            if(root_path_len <= FILE_MAX_FILENAME)
            {
                file_stores[s].file_root = (char*)malloc(root_path_len);
                if(file_stores[s].file_root)
                {
                    memcpy(file_stores[s].file_root, root_path, root_path_len);
                }
            }

            /* Check File Root Setup */
            if(file_stores[s].file_root == NULL)
            {
                bplog(BP_FAILEDMEM, "Failed to set FSS root path\n");
                bplib_store_file_destroy(s);
                break;
            }

            /* Setup Data Cache */
            int cache_size = FILE_DEFAULT_CACHE_SIZE;
            if(attr && attr->cache_size) cache_size = attr->cache_size;
            file_stores[s].cache_size = cache_size;
            file_stores[s].data_cache = (data_cache_t*)malloc(cache_size * sizeof(data_cache_t));
            if(file_stores[s].data_cache)
            {
                memset(file_stores[s].data_cache, 0, cache_size * sizeof(data_cache_t));
            }

            /* Set Flush Attribute */
            if(attr)    file_stores[s].flush_on_write = attr->flush_on_write;
            else        file_stores[s].flush_on_write = FILE_FLUSH_DEFAULT;

            /* Check Data Cache Setup */
            if(file_stores[s].data_cache == NULL)
            {
                bplog(BP_FAILEDMEM, "Failed to allocate FSS data cache\n");
                bplib_store_file_destroy(s);
                break;
            }

            /* Return Handle */
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

    if(file_stores[handle].write_fd != NULL)            file_driver.close(file_stores[handle].write_fd);
    if(file_stores[handle].read_fd != NULL)             file_driver.close(file_stores[handle].read_fd);
    if(file_stores[handle].retrieve_fd != NULL)         file_driver.close(file_stores[handle].retrieve_fd);
    if(file_stores[handle].file_root != NULL)           free(file_stores[handle].file_root);
    if(file_stores[handle].lock != BP_INVALID_HANDLE)   bplib_os_destroylock(file_stores[handle].lock);
    if(file_stores[handle].data_cache != NULL)          free(file_stores[handle].data_cache);

    file_stores[handle].in_use = false;

    return BP_SUCCESS;
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
    unsigned long data_size = data1_size + data2_size;
    unsigned long object_size = sizeof(bp_object_hdr_t) + data_size;
    unsigned long bytes_written = 0;
    bool flush_error = false;

    /* Get IDs */
    unsigned long data_id = GET_DATAID(fs->write_data_id);
    unsigned long file_id = GET_FILEID(data_id);
    unsigned long data_offset = GET_DATAOFFSET(data_id);

    /* Check Need to Open Write File */
    if(fs->write_fd == NULL)
    {
        /* Open Write File */
        fs->write_fd = open_dat_file(fs->service_id, fs->file_root, file_id, false);
        if(fs->write_fd == NULL) return bplog(BP_FAILEDSTORE, "Failed to enqueue data\n");;

        /* Seek to Current Position */
        if(fs->write_error)
        {
            /* Start at Beginning of File */
            int seek_status = file_driver.seek(fs->write_fd, 0, SEEK_SET);
            if(seek_status < 0) return bplog(BP_FAILEDSTORE, "Failed (%d) to set write position after error to start of file\n", seek_status);

            /* Read/Seek Through File */
            unsigned long pos;
            for(pos = 0; pos < data_offset; pos++)
            {
                unsigned long current_size;

                /* Read Current Data Size */
                unsigned long bytes_read = file_driver.read(&current_size, 1, sizeof(current_size), fs->write_fd);
                if(bytes_read != sizeof(current_size)) return bplog(BP_FAILEDSTORE, "Failed to read data size for write after error (%d != %d)\n", bytes_read, sizeof(current_size));

                /* Seek to End of Current Data */
                seek_status = file_driver.seek(fs->write_fd, current_size, SEEK_CUR);
                if(seek_status < 0) return bplog(BP_FAILEDSTORE, "Failed (%d) to jump over data for write after error\n", seek_status);
            }
        }
    }

    /* Create Object */
    bp_object_hdr_t object_header = {
        .handle = handle,
        .sid = BP_SID_VACANT,
        .size = data_size
    };

    /* Write Object Size */
    bytes_written += file_driver.write(&object_size, 1, sizeof(object_size), fs->write_fd);

    /* Write Object */
    bytes_written += file_driver.write(&object_header, 1, sizeof(bp_object_hdr_t), fs->write_fd);

    /* Write Data Buffer 1 */
    bytes_written += file_driver.write(data1, 1, data1_size, fs->write_fd);

    /* Write Data Buffer 2 */
    bytes_written += file_driver.write(data2, 1, data2_size, fs->write_fd);

    /* Flush File Data */
    if(fs->flush_on_write)
    {
        int flush_status = file_driver.flush(fs->write_fd);
        if(flush_status < 0)
        {
            bplog(BP_FAILEDSTORE, "Failed (%d) to flush data write\n", flush_status);
            flush_error = true;
        }
    }

    /* Check Errors and Return Status */
    if((bytes_written != (object_size + sizeof(object_size))) || flush_error)
    {
        fs->write_error = true;
        if(fs->write_fd)
        {
            file_driver.close(fs->write_fd);
            fs->write_fd = NULL;
        }

        /* Return Failure */
        return bplog(BP_FAILEDSTORE, "Failed to write data to file (%d ?= %d)\n", bytes_written, object_size + sizeof(object_size));
    }
    else
    {
        /* Close Write File
         *  this needs to be performed here prior to the write_data_id being
         *  incremented because the x_data_id variables are one based instead
         *  of zero based */
        if(fs->write_data_id % FILE_DATA_COUNT == 0)
        {
            file_driver.close(fs->write_fd);
            fs->write_fd = NULL;
        }

        /* Set Write State */
        fs->write_error = false;
        fs->write_data_id++;
        fs->data_count++;
        bplib_os_signal(fs->lock);

        /* Return Success */
        return BP_SUCCESS;
    }
}

/*--------------------------------------------------------------------------------------
 * bplib_store_file_dequeue -
 *-------------------------------------------------------------------------------------*/
int bplib_store_file_dequeue (int handle, bp_object_t** object, int timeout)
{
    assert(handle >= 0 && handle < FILE_MAX_STORES);
    assert(file_stores[handle].in_use);
    assert(object);

    file_store_t* fs = (file_store_t*)&file_stores[handle];
    bplib_os_lock(fs->lock);
    {
        /* Initialize Variables */
        unsigned long bytes_read = 0;
        unsigned long object_size = 0;
        unsigned char* object_ptr = NULL;
        unsigned long cache_index = 0;
        bool read_success = false;

        /* Get IDs */
        unsigned long data_id = GET_DATAID(fs->read_data_id);
        unsigned long file_id = GET_FILEID(data_id);
        unsigned long data_offset = GET_DATAOFFSET(data_id);

        /* Check if Data Available */
        if(fs->read_data_id == fs->write_data_id)
        {
            int wait_status = bplib_os_waiton(fs->lock, timeout);
            if(wait_status == BP_ERROR)
            {
                bplib_os_unlock(fs->lock);
                return bplog(BP_FAILEDSTORE, "Failed (%d) to wait for FSS lock\n", wait_status);
            }
            else if((wait_status == BP_TIMEOUT) ||
                    (fs->read_data_id == fs->write_data_id))
            {
                bplib_os_unlock(fs->lock);
                return BP_TIMEOUT;
            }
        }

        /* Check Need to Open Read File */
        if(fs->read_fd == NULL)
        {
            /* Open Read File */
            fs->read_fd = open_dat_file(fs->service_id, fs->file_root, file_id, true);
            if(fs->read_fd == NULL)
            {
                bplib_os_unlock(fs->lock);
                return bplog(BP_FAILEDSTORE, "Failed to dequeue data\n");
            }
        }

        /* Seek to Current Position */
        if(fs->read_error)
        {
            /* Start at Beginning of File */
            int seek_status = file_driver.seek(fs->read_fd, 0, SEEK_SET);
            if(seek_status < 0)
            {
                bplib_os_unlock(fs->lock);
                return bplog(BP_FAILEDSTORE, "Failed (%d) to set read position after error to start of file\n", seek_status);
            }

            /* Read/Seek Through File */
            unsigned int pos;
            for(pos = 0; pos < data_offset; pos++)
            {
                unsigned long current_size;

                /* Read Current Object Size */
                bytes_read = file_driver.read(&current_size, 1, sizeof(current_size), fs->read_fd);
                if(bytes_read != sizeof(current_size))
                {
                    bplib_os_unlock(fs->lock);
                    return bplog(BP_FAILEDSTORE, "Failed to read data size for read after error (%d != %d)\n", bytes_read, sizeof(current_size));

                }

                /* Seek to End of Current Data */
                seek_status = file_driver.seek(fs->read_fd, current_size, SEEK_CUR);
                if(seek_status < 0)
                {
                    bplib_os_unlock(fs->lock);
                    return bplog(BP_FAILEDSTORE, "Failed (%d) to jump over data for read after error\n", seek_status);
                }
            }
        }

        /* Read Data */
        bytes_read = file_driver.read(&object_size, 1, sizeof(object_size), fs->read_fd);
        if(bytes_read == sizeof(object_size))
        {
            object_ptr = (unsigned char*)malloc(object_size);
            bytes_read = file_driver.read(object_ptr, 1, object_size, fs->read_fd);
            if(bytes_read == object_size)
            {
                /* Update SID */
                bp_object_hdr_t* dequeued_object_header = (bp_object_hdr_t*)object_ptr;
                dequeued_object_header->sid = (bp_sid_t)fs->read_data_id;
                read_success = true;
            }
        }

        /* Check for Read Errors */
        if(!read_success)
        {
            /* Set Error State */
            fs->read_error = true;

            /* Close Read File */
            if(fs->read_fd)
            {
                file_driver.close(fs->read_fd);
                fs->read_fd = NULL;
            }

            /* Free Object Pointer */
            if(object_ptr) free(object_ptr);

            /* Return Failure */
            bplib_os_unlock(fs->lock);
            return bplog(BP_FAILEDSTORE, "Failed (%d) to read data from file\n", bytes_read);
        }

        /* Check State of Data Cache */
        cache_index = data_id % fs->cache_size;
        if(fs->data_cache[cache_index].mem_locked)
        {
            int wait_status = bplib_os_waiton(fs->lock, timeout);
            if(wait_status == BP_ERROR)
            {
                free(object_ptr);
                bplib_os_unlock(fs->lock);
                return bplog(BP_FAILEDSTORE, "Failed (%d) to get lock to update cache\n", wait_status);
            }
            else if((wait_status == BP_TIMEOUT) ||
                    (fs->data_cache[cache_index].mem_locked))
            {
                free(object_ptr);
                bplib_os_unlock(fs->lock);
                return BP_TIMEOUT;
            }
        }

        /* Free Previous Entry in Data Cache */
        if(fs->data_cache[cache_index].mem_ptr)
        {
            free(fs->data_cache[cache_index].mem_ptr);
        }

        /* Update Data Cache */
        fs->data_cache[cache_index].mem_locked = FILE_MEM_LOCKED;
        fs->data_cache[cache_index].mem_ptr = object_ptr;
        fs->data_cache[cache_index].mem_data_id = data_id;

        /* Check Need to Open New File
         *  this needs to be performed here prior to the read_data_id being
         *  incremented because the x_data_id variables are one based instead
         *  of zero based */
        if(fs->read_data_id % FILE_DATA_COUNT == 0)
        {
            file_driver.close(fs->read_fd);
            fs->read_fd = NULL;
        }

        /* Set Read State */
        fs->read_error = false;
        fs->read_data_id++;

        /* Return Object */
        *object = (bp_object_t*)object_ptr;
    }
    bplib_os_unlock(fs->lock);

    /* Return Success */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_file_retrieve -
 *-------------------------------------------------------------------------------------*/
int bplib_store_file_retrieve (int handle, bp_sid_t sid, bp_object_t** object, int timeout)
{
    (void)timeout;

    assert(handle >= 0 && handle < FILE_MAX_STORES);
    assert(file_stores[handle].in_use);
    assert(object);

    file_store_t* fs = (file_store_t*)&file_stores[handle];
    bplib_os_lock(fs->lock);
    {
        /* Initialize Variables */
        unsigned long bytes_read = 0;
        unsigned long object_size = 0;
        unsigned char* object_ptr = NULL;
        unsigned long cache_index = 0;
        long offset_delta = 0;
        bool retrieve_success = false;

        /* Get IDs */
        unsigned long data_id = GET_DATAID(sid);
        unsigned long file_id = GET_FILEID(data_id);
        unsigned long data_offset = GET_DATAOFFSET(data_id);
        unsigned long prev_data_id = GET_DATAID(fs->retrieve_data_id);
        unsigned long prev_file_id = GET_FILEID(prev_data_id);
        unsigned long prev_data_offset = GET_DATAOFFSET(prev_data_id);

        /* Check Data Cache */
        cache_index = data_id % fs->cache_size;
        if(fs->data_cache[cache_index].mem_ptr)
        {
            if(fs->data_cache[cache_index].mem_data_id == data_id)
            {
                /* Return Data Cache */
                *object = fs->data_cache[cache_index].mem_ptr;
                bplib_os_unlock(fs->lock);
                return BP_SUCCESS;
            }
        }

        /* Check Need to Open New Retrieve File */
        if(file_id != prev_file_id)
        {
            if(fs->retrieve_fd)
            {
                file_driver.close(fs->retrieve_fd);
                fs->retrieve_fd = NULL;
            }
        }

        /* Check Need to Open Retrieve File */
        if(fs->retrieve_fd == NULL)
        {
            /* Open Retrieve File */
            fs->retrieve_fd = open_dat_file(fs->service_id, fs->file_root, file_id, true);
            if(fs->retrieve_fd == NULL)
            {
                bplib_os_unlock(fs->lock);
                return bplog(BP_FAILEDSTORE, "Failed to retrieve data\n");
            }
        }
        else
        {
            /* Set Delta from Current Position in File */
            offset_delta = data_offset - prev_data_offset;
            if(offset_delta < 0)
            {
                /* Handling Seeking Backwards */
                offset_delta = data_offset;
                int seek_status = file_driver.seek(fs->retrieve_fd, 0, SEEK_SET);
                if(seek_status < 0)
                {
                    bplib_os_unlock(fs->lock);
                    return bplog(BP_FAILEDSTORE, "Failed (%d) to set retrieve position to start of file\n", seek_status);
                }
            }
        }

        /* Seek Forward */
        if(offset_delta > 0)
        {
            unsigned long current_size;
            long pos;

            for(pos = 0; pos < offset_delta; pos++)
            {
                /* Read Current Data Size */
                bytes_read = file_driver.read(&current_size, 1, sizeof(current_size), fs->retrieve_fd);
                if(bytes_read != sizeof(current_size))
                {
                    bplib_os_unlock(fs->lock);
                    return bplog(BP_FAILEDSTORE, "Failed to read data size on retrieval (%d != %d)\n", bytes_read, sizeof(current_size));
                }

                /* Seek to End of Current Data */
                int seek_status = file_driver.seek(fs->retrieve_fd, current_size, SEEK_CUR);
                if(seek_status < 0)
                {
                    bplib_os_unlock(fs->lock);
                    return bplog(BP_FAILEDSTORE, "Failed (%d) to jump to end of data on retrieval\n", seek_status);
                }
            }
        }

        /* Read Data */
        bytes_read = file_driver.read(&object_size, 1, sizeof(object_size), fs->retrieve_fd);
        if(bytes_read == sizeof(object_size))
        {
            object_ptr = (unsigned char*)malloc(object_size);
            bytes_read = file_driver.read(object_ptr, 1, object_size, fs->retrieve_fd);
            if(bytes_read == object_size)
            {
                bp_object_hdr_t* retrieved_object_header = (bp_object_hdr_t*)object_ptr;
                retrieved_object_header->sid = sid;
                fs->retrieve_data_id = (unsigned long)sid;
                retrieve_success = true;
            }
        }

        /* Check Success */
        if(!retrieve_success)
        {
            free(object_ptr);

            /* Close Read File */
            if(fs->retrieve_fd)
            {
                file_driver.close(fs->retrieve_fd);
                fs->retrieve_fd = NULL;
            }

            /* Return Failure */
            bplib_os_unlock(fs->lock);
            return bplog(BP_FAILEDSTORE, "Failed (%d) to retrieve data from file\n", bytes_read);
        }

        /* Check State of Data Cache */
        cache_index = data_id % fs->cache_size;
        if(fs->data_cache[cache_index].mem_locked)
        {
            int wait_status = bplib_os_waiton(fs->lock, timeout);
            if(wait_status == BP_ERROR)
            {
                free(object_ptr);
                bplib_os_unlock(fs->lock);
                return bplog(BP_FAILEDSTORE, "Failed (%d) to update data cache on retrieval\n", wait_status);
            }
            else if((wait_status == BP_TIMEOUT) ||
                    (fs->data_cache[cache_index].mem_locked))
            {
                free(object_ptr);
                bplib_os_unlock(fs->lock);
                return BP_TIMEOUT;
            }
        }

        /* Free Previous Entry in Data Cache */
        if(fs->data_cache[cache_index].mem_ptr)
        {
            free(fs->data_cache[cache_index].mem_ptr);
        }

        /* Update Data Cache */
        fs->data_cache[cache_index].mem_locked = FILE_MEM_LOCKED;
        fs->data_cache[cache_index].mem_ptr = object_ptr;
        fs->data_cache[cache_index].mem_data_id = data_id;

        /* Return Object */
        *object = (bp_object_t*)object_ptr;
    }
    bplib_os_unlock(fs->lock);

    /* Return Success */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_file_release -
 *-------------------------------------------------------------------------------------*/
int bplib_store_file_release (int handle, bp_sid_t sid)
{
    assert(handle >= 0 && handle < FILE_MAX_STORES);
    assert(file_stores[handle].in_use);

    file_store_t* fs = (file_store_t*)&file_stores[handle];

    bplib_os_lock(fs->lock);
    {
        /* Get Cache Index */
        unsigned long data_id = GET_DATAID(sid);
        unsigned long cache_index = data_id % fs->cache_size;

        /* Check Data Cache */
        if((fs->data_cache[cache_index].mem_ptr == NULL) ||
           (fs->data_cache[cache_index].mem_data_id != data_id))
        {
            /* Releasing Invalid Resource */
            bplib_os_unlock(fs->lock);
            return bplog(BP_FAILEDSTORE, "Failed to release invalid resource: %lu\n", (unsigned long)sid);
        }

        /* Unlock Cache Entry */
        fs->data_cache[cache_index].mem_locked = FILE_MEM_AVAIABLE;
        bplib_os_signal(fs->lock);
    }
    bplib_os_unlock(fs->lock);

    /* Return Success */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_file_relinquish -
 *-------------------------------------------------------------------------------------*/
int bplib_store_file_relinquish (int handle, bp_sid_t sid)
{
    assert(handle >= 0 && handle < FILE_MAX_STORES);
    assert(file_stores[handle].in_use);

    file_store_t* fs = (file_store_t*)&file_stores[handle];
    bplib_os_lock(fs->lock);
    {
        /* Get IDs */
        unsigned long data_id = GET_DATAID(sid);
        unsigned long file_id = GET_FILEID(data_id);
        unsigned long data_offset = GET_DATAOFFSET(data_id);
        unsigned long prev_data_id = GET_DATAID(fs->relinquish_data_id);
        unsigned long prev_file_id = GET_FILEID(prev_data_id);

        /* Clear Data Cache */
        unsigned long cache_index = data_id % fs->cache_size;
        if(fs->data_cache[cache_index].mem_ptr)
        {
            if(fs->data_cache[cache_index].mem_data_id == data_id)
            {
                free(fs->data_cache[cache_index].mem_ptr);
                fs->data_cache[cache_index].mem_ptr = NULL;
                fs->data_cache[cache_index].mem_data_id = BP_SID_VACANT;
                fs->data_cache[cache_index].mem_locked = FILE_MEM_AVAIABLE;
            }
        }

        /* Check Need to Read New Relinquish Table */
        if(file_id != prev_file_id)
        {
            /* Set Current Relinquish Table */
            fs->relinquish_data_id = (unsigned long)sid;

            /* Check Need to Save Off Previous Relinquish Table */
            if(fs->relinquish_table.free_cnt > 0)
            {
                /* Open Previous Relinquish File */
                if(fs->relinquish_fd == NULL)
                {
                    fs->relinquish_fd = open_tbl_file(fs->service_id, fs->file_root, prev_file_id, false);
                    if(fs->relinquish_fd == NULL)
                    {
                        bplib_os_unlock(fs->lock);
                        return bplog(BP_FAILEDSTORE, "Failed to relinquish data\n");
                    }
                }

                /* Write Previous Relinquish Table */
                unsigned long bytes_written = file_driver.write(&fs->relinquish_table, 1, sizeof(fs->relinquish_table), fs->relinquish_fd);

                /* Close Previous Relinquish File */
                file_driver.close(fs->relinquish_fd);
                fs->relinquish_fd = NULL;

                /* Check Status of Write */
                if(bytes_written != sizeof(fs->relinquish_table))
                {
                    bplib_os_unlock(fs->lock);
                    return bplog(BP_FAILEDSTORE, "Failed to update relinquish table (%d != %d)\n", bytes_written, sizeof(fs->relinquish_table));
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
                unsigned long bytes_read = file_driver.read(&fs->relinquish_table, 1, sizeof(fs->relinquish_table), fs->relinquish_fd);

                /* Close New Relinquish File */
                file_driver.close(fs->relinquish_fd);
                fs->relinquish_fd = NULL;

                /* Check for Error */
                if(bytes_read != sizeof(fs->relinquish_table))
                {
                    bplib_os_unlock(fs->lock);
                    return bplog(BP_FAILEDSTORE, "Failed to read new relinquish table (%d != %d)\n", bytes_read, sizeof(fs->relinquish_table));
                }
            }
        }

        /* Check if Still Present */
        if(fs->relinquish_table.freed[data_offset] == 0)
        {
            /* Mark Data as Relinquished */
            fs->relinquish_table.freed[data_offset] = 1;
            fs->data_count--;

            /* Relinquish Resources */
            fs->relinquish_table.free_cnt++;
            if(fs->relinquish_table.free_cnt == FILE_DATA_COUNT)
            {
                /* Delete Associated Files
                 *  only check the status of the data file deletion as it is
                 *  possible (and often the case) that the table file is never
                 *  created because the state of which bundles are freed does
                 *  not need to be saved off */
                delete_tbl_file(fs->service_id, fs->file_root, file_id);
                int dat_status = delete_dat_file(fs->service_id, fs->file_root, file_id);
                if(dat_status < 0)
                {
                    bplib_os_unlock(fs->lock);
                    return bplog(BP_FAILEDSTORE, "Failed (%d) to relinquish file\n", dat_status);
                }
            }
        }
    }
    bplib_os_unlock(fs->lock);

    /* Return Success */
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
