/************************************************************************
 * File: flash_store.c
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
#include "bplib_store_flash.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

/* Configurable Options */

#ifndef FLASH_MAX_STORES
#define FLASH_MAX_STORES            24
#endif

#ifndef FLASH_MAX_PAGES_PER_BLOCK
#define FLASH_MAX_PAGES_PER_BLOCK   128
#endif

/* Local Definitions */

#define FLASH_BUFFER                1
#define FLASH_USE_EDAC              0
#define FLASH_PAGE_USE_BYTES        (FLASH_MAX_PAGES_PER_BLOCK / 8)

/******************************************************************************
 MACROS
 ******************************************************************************/

#define FLASH_GET_SID(addr)                 ((((addr).block * FLASH_DRIVER.pages_per_block) + (addr).page) + 1)
#define FLASH_GET_CACHE_INDEX(sid, attr)    ((((sid)-1) % (attr).cache_size) * (attr).max_data_size)
#define FLASH_GET_BLOCK(sid)                (((sid)-1) / FLASH_DRIVER.pages_per_block)
#define FLASH_GET_PAGE(sid)                 (((sid)-1) % FLASH_DRIVER.pages_per_block)

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    bp_flash_index_t    out;
    bp_flash_index_t    in;
    int                 count;
} flash_free_blocks_t;

typedef struct {
    bp_flash_index_t    next_block;
    uint8_t             page_use[FLASH_PAGE_USE_BYTES];
} flash_free_control_t;

typedef struct {
    bp_flash_index_t    next_block;
} flash_data_control_t;

typedef struct {
    bool                in_use;
    bp_flash_attr_t     attributes;
    bp_flash_addr_t     write_addr;
    bp_flash_addr_t     read_addr;
    uint8_t*            data_cache; /* [((block * pages_per_block) + page) % cache_size] * max_data_size */
    uint8_t*            object_stage; /* lockable buffer that returns data object */
    bool                stage_locked;
    int                 object_count;
} flash_store_t;

/******************************************************************************
 LOCAL FILE DATA
 ******************************************************************************/

/* Constants - set only in initialization function */

static bp_flash_driver_t    FLASH_DRIVER;
static bp_flash_index_t     FLASH_MASTER_FREE_PAGE;
static bp_flash_index_t     FLASH_MASTER_DATA_PAGE;

/* Globals */

static flash_store_t        flash_stores[FLASH_MAX_STORES];
static int                  flash_device_lock;
static flash_free_blocks_t  flash_free_blocks;

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * flash_free_reclaim -
 *-------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE int flash_free_reclaim (bp_flash_index_t block)
{
    int status;
    
    /* Erase Block */
    status = FLASH_DRIVER.erase(block);
    if(status == BP_SUCCESS)
    {
        if(flash_free_blocks.out == BP_FLASH_INVALID_INDEX)
        {
            /* First Block Successfully Reclaimed */
            flash_free_blocks.out = block;
            flash_free_blocks.in = block;
            flash_free_blocks.count++;
        }
        else
        {
            /* Insert In Free Block List */
            flash_free_control_t control = {block, {0}};
            bp_flash_addr_t page_addr = {flash_free_blocks.in, FLASH_MASTER_FREE_PAGE, BP_FLASH_SPARE_TYPE};
            status = FLASH_DRIVER.write(page_addr, &control, sizeof(control));
            if(status == BP_SUCCESS)
            {
                flash_free_blocks.in = block;
                flash_free_blocks.count++;
            }
        }
    }
    
    return status;
}

/*--------------------------------------------------------------------------------------
 * flash_free_allocate -
 *-------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE int flash_free_allocate (bp_flash_index_t* block)
{
    /* Check if Block Available */
    if(flash_free_blocks.out == BP_FLASH_INVALID_INDEX)
    {
        return bplog(BP_FAILEDSTORE, "No more free blocks available\n");
    }

    /* Read Control Information from End of Free Block List */
    flash_free_control_t control;
    bp_flash_addr_t page_addr = {flash_free_blocks.out, FLASH_MASTER_FREE_PAGE, BP_FLASH_SPARE_TYPE};
    int status = FLASH_DRIVER.read(page_addr, &control, sizeof(control));
    if(status != BP_SUCCESS) return bplog(BP_FAILEDSTORE, "Failed to read free block information: %d\n", status);
    
    /* Return Block and Update List */
    *block = flash_free_blocks.out;
    flash_free_blocks.out = control.next_block;
    flash_free_blocks.count--;
    
    /* Check for Empty List */
    if(flash_free_blocks.count <= 0)
    {
        flash_free_blocks.out = BP_FLASH_INVALID_INDEX;
        flash_free_blocks.in = BP_FLASH_INVALID_INDEX;        
    }

    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * flash_data_write -
 *-------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE int flash_data_write (bp_flash_addr_t* addr, uint8_t* data, int size)
{
    int status;
    int data_index = 0;
    int bytes_left = size;
    
    /* Copy Into and Write Pages */
    while(bytes_left > 0)
    {
        /* Check if Page Available to Write */
        if(addr->page == FLASH_DRIVER.pages_per_block)
        {
            bp_flash_index_t next_write_block = addr->block;
            status = flash_free_allocate(&next_write_block);
            if(status == BP_SUCCESS)
            {
                flash_data_control_t control = {addr->block};
                bp_flash_addr_t page_addr = {addr->block, FLASH_MASTER_DATA_PAGE, BP_FLASH_SPARE_TYPE};
                status = FLASH_DRIVER.write(page_addr, &control, sizeof(control));
                if(status == BP_SUCCESS)
                {
                    addr->block = next_write_block;
                    addr->page = 0;
                }
                else
                {
                    /* Next Block Write Failed */
                    return bplog(status, "Failed to write master data page in middle of flash write at block: %ld\n", addr->block);
                }
            }
            else
            {
                /* Free Block Retrieval Failed */
                return bplog(status, "Failed to retrieve next free block in middle of flash write at block: %ld\n", addr->block);
            }
        }            

        /* Write Data into Page */
        int bytes_to_copy = bytes_left < FLASH_DRIVER.data_size ? bytes_left : FLASH_DRIVER.data_size;
        status = FLASH_DRIVER.write(*addr, &data[data_index], bytes_to_copy);
        if(status == BP_SUCCESS)
        {
            data_index += bytes_to_copy;
            bytes_left -= bytes_to_copy;
            addr->page++;
        }
        else
        {
            /* Write Failed */
            return bplog(status, "Failed to write data to flash address: %d.%d\n", addr->block, addr->page);
        }
    }
    
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * flash_data_read -
 *-------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE int flash_data_read (bp_flash_addr_t* addr, uint8_t* data, int size)
{
    int data_index = 0;
    int bytes_left = size;

    /* Copy Into and Write Pages */
    while(bytes_left > 0)
    {
        /* Check if Page Available to Read */
        if(addr->page == FLASH_DRIVER.pages_per_block)
        {
            flash_data_control_t control;
            bp_flash_addr_t page_addr = {addr->block, FLASH_MASTER_DATA_PAGE, BP_FLASH_SPARE_TYPE};
            int status = FLASH_DRIVER.read(page_addr, &control, sizeof(control));
            if(status == BP_SUCCESS)
            {
                addr->block = control.next_block;
                addr->page = 0;
            }
            else
            {
                /* Next Block Retrieval Failed */
                return bplog(status, "Failed in middle of flash read at block: %ld\n", addr->block);
            }
        }            

        /* Read Data from Page */
        int bytes_to_copy = bytes_left < FLASH_DRIVER.data_size ? bytes_left : FLASH_DRIVER.data_size;
        int status = FLASH_DRIVER.read(*addr, &data[data_index], bytes_to_copy);
        if(status == BP_SUCCESS)
        {
            data_index += bytes_to_copy;
            bytes_left -= bytes_to_copy;
            addr->page++;
        }
        else
        {
            /* Read Failed */
            return bplog(status, "Failed to read data to flash address: %d.%d\n", addr->block, addr->page);
        }
    }
    
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * flash_data_delete -
 *-------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE int flash_data_delete (bp_flash_addr_t* addr, int size)
{
    int status;
    
    flash_free_control_t control = {BP_FLASH_INVALID_INDEX, {0}};
    bp_flash_index_t current_block = BP_FLASH_INVALID_INDEX;
    unsigned int current_block_free_pages = 0;
    int bytes_left = size;
    
    while(bytes_left > 0)
    {
        /* Check if Page Available to Delete */
        if(addr->page == FLASH_DRIVER.pages_per_block)
        {
            bp_flash_addr_t page_addr = {addr->block, FLASH_MASTER_DATA_PAGE, BP_FLASH_SPARE_TYPE};
            status = FLASH_DRIVER.read(page_addr, &control, sizeof(control));
            if(status == BP_SUCCESS)
            {
                addr->block = control.next_block;
                addr->page = 0;
            }
            else
            {
                /* Next Block Retrieval Failed */
                return bplog(status, "Failed in middle of flash delete at block: %ld\n", addr->block);
            }
        }            

        /* Page Control Information */
        if(current_block != addr->block)
        {
            bp_flash_addr_t page_addr = {BP_FLASH_INVALID_INDEX, FLASH_MASTER_FREE_PAGE, BP_FLASH_SPARE_TYPE};

            /* Write Out Previous Blocks Control Information */
            if(control.next_block == BP_FLASH_INVALID_INDEX)
            {
                page_addr.block = current_block;
                status = FLASH_DRIVER.write(page_addr, &control, sizeof(control));
                if(status != BP_SUCCESS)
                {
                    return bplog(BP_FAILEDSTORE, "Failed to write free page information at block: %ld\n", current_block);
                }
            }
            
            /* Get Next Block's Control Information */
            current_block = addr->block;
            page_addr.block = current_block;
            status = FLASH_DRIVER.read(page_addr, &control, sizeof(control));
            if(status != BP_SUCCESS)
            {
                return bplog(BP_FAILEDSTORE, "Failed to read free page information at block: %ld\n", current_block);
            }
            
            /* Count Number of Free Pages */
            unsigned int byte_index, bit_index;
            current_block_free_pages = 0;
            for(byte_index = 0; byte_index < (FLASH_DRIVER.pages_per_block / 8); byte_index++)
            {
                for(bit_index = 0; bit_index < 8; bit_index++)
                {
                    if(control.page_use[byte_index] & (0x80 >> bit_index))
                    {
                        current_block_free_pages++;
                    }
                }
            }
        }
                
        /* Mark Data on Page as Deleted */
        int byte_offset = addr->page / 8;
        int bit_offset = addr->page % 8;
        control.page_use[byte_offset] &= ~(0x80 >> bit_offset);
        current_block_free_pages++;
                        
        /* Update Bytes Left and Address */
        int bytes_to_delete = bytes_left < FLASH_DRIVER.data_size ? bytes_left : FLASH_DRIVER.data_size;
        bytes_left -= bytes_to_delete;
        addr->page++;

        /* Check if Block can be Erased */
        if(current_block_free_pages >= FLASH_DRIVER.pages_per_block)
        {
            /* Check Page Consistency */
            if(bytes_left != 0 && addr->page != FLASH_DRIVER.pages_per_block)
            {
                bplog(BP_FAILEDSTORE, "Inconsistent page %ld.%ld - block is erasable, yet page contains data\n", addr->block, addr->page);
            }
            
            /* Reclaim Block as Free */
            status = flash_free_reclaim (current_block);
            if(status != BP_SUCCESS)
            {
                bplog(BP_FAILEDSTORE, "Failed (%d) to reclaim block %ld as a free block\n", status, addr->block);                
            }
        }
    }

    /* Write Last Block's Control Information */
    bp_flash_addr_t page_addr = {current_block, FLASH_MASTER_FREE_PAGE, BP_FLASH_SPARE_TYPE};
    status = FLASH_DRIVER.write(page_addr, &control, sizeof(control));
    if(status != BP_SUCCESS)
    {
        return bplog(BP_FAILEDSTORE, "Failed (%d) to write free page information at block: %ld\n", status, current_block);
    }
    
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * flash_object_read -
 *-------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE int flash_object_read (flash_store_t* fs, bp_flash_addr_t* addr, bp_object_t** object)
{
    int status = BP_SUCCESS;

    /* Check Stage Lock */
    if(fs->stage_locked == false)
    {
        /* Check if Data Objects Available */
        if(fs->object_count > 0)
        {
            /* Check Cache */
            uint64_t sid = FLASH_GET_SID(*addr);       
            int cache_index = FLASH_GET_CACHE_INDEX(sid, fs->attributes);
            bp_object_t* object_hdr = (bp_object_t*)&fs->data_cache[cache_index];
            if((uint64_t)object_hdr->sid == sid)
            {
                /* Cache Hit */
                memcpy(fs->object_stage, object_hdr, object_hdr->size + sizeof(bp_object_t));
            }
            else
            {
                /* Read Object Header from Flash */
                status = flash_data_read(addr, fs->object_stage, FLASH_DRIVER.data_size);
                if(status == BP_SUCCESS)
                {
                    object_hdr = (bp_object_t*)fs->object_stage;
                    int bytes_read = FLASH_DRIVER.data_size - sizeof(bp_object_t);
                    int remaining_bytes = object_hdr->size - bytes_read;
                    if(remaining_bytes > 0)
                    {
                        status = flash_data_read(addr, &fs->object_stage[FLASH_DRIVER.data_size], remaining_bytes);
                    }
                }
            }

            /* Object Successfully Dequeued */
            if(status == BP_SUCCESS)
            {
                /* Return Locked Object */
                *object = (bp_object_t*)fs->object_stage;
                fs->stage_locked = true;
            }
        }
        else
        {
            /* No Data to Return */
            status = BP_TIMEOUT;
        }
    }
    else
    {
        status = bplog(BP_ERROR, "Object read cannot proceed when object stage is locked\n");
    }
    
    /* Return Status */
    return status;
}    

/******************************************************************************
 EXPORTED APPLICATION FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_store_flash_init -
 *-------------------------------------------------------------------------------------*/
int bplib_store_flash_init (bp_flash_driver_t driver)
{
    /* Initialize Flash Stores to Zero */
    memset(flash_stores, 0, sizeof(flash_stores));

    /* Initialize Flash Driver */
    FLASH_DRIVER = driver;

    /* Initialize Flash Device Lock */
    flash_device_lock = bplib_os_createlock();
    if(flash_device_lock < 0)
    {
        return bplog(BP_FAILEDOS, "Failed (%d) to create flash device lock\n", flash_device_lock);
    }
    
    /* Initialize Free Blocks List */
    flash_free_blocks.out = BP_FLASH_INVALID_INDEX;
    flash_free_blocks.in = BP_FLASH_INVALID_INDEX;
    flash_free_blocks.count = 0;

    /* Initialize Flash Master Pages */
    FLASH_MASTER_FREE_PAGE = driver.pages_per_block - 2;
    FLASH_MASTER_DATA_PAGE = driver.pages_per_block - 1;
    
    /* Format Flash - Iterate over all Blocks */
    int reclaimed_blocks = 0;
    bp_flash_index_t block;
    for(block = 0; block < FLASH_DRIVER.num_blocks; block++)
    {
        int status = flash_free_reclaim(block);
        if(status == BP_SUCCESS) reclaimed_blocks++;
    }
    
    /* Return Number of Reclaimed Blocks */
    return reclaimed_blocks;
}

/******************************************************************************
 EXPORTED STORAGE SERVICE FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_store_flash_create -
 *-------------------------------------------------------------------------------------*/
int bplib_store_flash_create (void* parm)
{
    bp_flash_attr_t* attr = (bp_flash_attr_t*)parm;
    
    int s;
    for(s = 0; s < FLASH_MAX_STORES; s++)
    {
        if(flash_stores[s].in_use == false)
        {
            /* Initialize Attributes */
            if(attr)
            {
                /* Check User Provided Attributes */
                if((attr->max_data_size < FLASH_DRIVER.data_size) || 
                   (attr->cache_size < 1))
                {
                   return bplog(BP_INVALID_HANDLE, "Invalid attributes - must supply sufficient sizes\n");
                }

                /* Copy User Provided Attributes */
                flash_stores[s].attributes = *attr;
            }
            else
            {
                /* Use Default Attributes */
                flash_stores[s].attributes.cache_size = 1;
                flash_stores[s].attributes.max_data_size = FLASH_DRIVER.data_size;
            }
            
            /* Account for Overhead */
            flash_stores[s].attributes.max_data_size += sizeof(bp_object_t);
                        
            /* Initialize Read/Write Pointers */
            flash_stores[s].write_addr.block    = BP_FLASH_INVALID_INDEX;
            flash_stores[s].write_addr.page     = 0;
            flash_stores[s].write_addr.type     = BP_FLASH_DATA_TYPE;            
            flash_stores[s].read_addr.block     = BP_FLASH_INVALID_INDEX;
            flash_stores[s].read_addr.page      = 0;
            flash_stores[s].read_addr.type      = BP_FLASH_DATA_TYPE;

            /* Allocate Cache */   
            int data_cache_size = flash_stores[s].attributes.cache_size * flash_stores[s].attributes.max_data_size;
            flash_stores[s].data_cache = (uint8_t*)malloc(data_cache_size);
            if(flash_stores[s].data_cache)
            {
                memset(flash_stores[s].data_cache, 0, data_cache_size);
            }
            else
            {
                return bplog(BP_INVALID_HANDLE, "Unable to allocate data cache\n");
            }
            
            /* Allocate Stage */
            flash_stores[s].stage_locked = false;
            flash_stores[s].object_stage = (uint8_t*)malloc(flash_stores[s].attributes.max_data_size);
            if(flash_stores[s].object_stage == NULL)
            {
                free(flash_stores[s].data_cache);
                return bplog(BP_INVALID_HANDLE, "Unable to allocate data stage\n");
            }
            
            /* Set Object Count to Zero */
            flash_stores[s].object_count = 0;
            
            /* Set In Use (necessary to be able to destroy later) */
            flash_stores[s].in_use = true;

            /* Return Handle */
            return s;
        }
    }

    return BP_INVALID_HANDLE;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_flash_destroy -
 *-------------------------------------------------------------------------------------*/
int bplib_store_flash_destroy (int handle)
{
    assert(handle >= 0 && handle < FLASH_MAX_STORES);
    assert(flash_stores[handle].in_use);
    
    free(flash_stores[handle].data_cache);
    free(flash_stores[handle].object_stage);
    flash_stores[handle].in_use = false;
    
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_flash_enqueue -
 *-------------------------------------------------------------------------------------*/
int bplib_store_flash_enqueue (int handle, void* data1, int data1_size, void* data2, int data2_size, int timeout)
{
    (void)timeout;

    assert(handle >= 0 && handle < FLASH_MAX_STORES);
    assert(flash_stores[handle].in_use);

    flash_store_t* fs = (flash_store_t*)&flash_stores[handle];
    int status = BP_SUCCESS;
    
    bplib_os_lock(flash_device_lock);
    {            
        /* Check if First Write Block Available */
        if(fs->write_addr.block == BP_FLASH_INVALID_INDEX)
        {
            status = flash_free_allocate(&fs->write_addr.block);
            if(status == BP_SUCCESS)
            {
                fs->read_addr = fs->write_addr;
            }
        }

        if(status == BP_SUCCESS)
        {
            /* Check if Room Available */
            int bytes_available = flash_free_blocks.count * FLASH_DRIVER.pages_per_block * FLASH_DRIVER.data_size;
            int bytes_needed = sizeof(bp_object_t) + data1_size + data2_size;
            if(bytes_available >= bytes_needed && fs->attributes.max_data_size >= bytes_needed)
            {            
                /* Calculate Object Information */
                uint64_t sid = FLASH_GET_SID(fs->write_addr);                
                int cache_index = FLASH_GET_CACHE_INDEX(sid, fs->attributes);
                bp_object_t object_hdr = {handle, (bp_sid_t)sid, data1_size + data2_size};
                
                /* Copy into Cache */
                memcpy(&fs->data_cache[cache_index], &object_hdr, sizeof(object_hdr));
                if(data1) memcpy(&fs->data_cache[cache_index + sizeof(object_hdr)], data1, data1_size);
                if(data2) memcpy(&fs->data_cache[cache_index + sizeof(object_hdr) + data1_size], data2, data2_size);

                /* Write Data into Flash */
                status = flash_data_write(&fs->write_addr, &fs->data_cache[cache_index], bytes_needed);
                if(status == BP_SUCCESS)
                {
                    fs->object_count++;
                }
            }
            else
            {
                status = bplog(BP_FAILEDSTORE, "Insufficient room in flash storage, available: %llu, needed: %ll\n", (long long unsigned)bytes_available, (long long unsigned)bytes_needed);
            }        
        }
    }
    bplib_os_unlock(flash_device_lock);

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_flash_dequeue -
 *-------------------------------------------------------------------------------------*/
int bplib_store_flash_dequeue (int handle, bp_object_t** object, int timeout)
{
    (void)timeout;
    
    assert(handle >= 0 && handle < FLASH_MAX_STORES);
    assert(flash_stores[handle].in_use);
    assert(object);
    
    flash_store_t* fs = (flash_store_t*)&flash_stores[handle];
    int status = BP_SUCCESS;
    
    bplib_os_lock(flash_device_lock);
    {          
        status = flash_object_read(fs, &fs->read_addr, object);
    }
    bplib_os_unlock(flash_device_lock);
    
    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_flash_retrieve -
 *-------------------------------------------------------------------------------------*/
int bplib_store_flash_retrieve (int handle, bp_sid_t sid, bp_object_t** object, int timeout)
{
    (void)timeout;

    assert(handle >= 0 && handle < FLASH_MAX_STORES);
    assert(flash_stores[handle].in_use);
    assert(object);
    
    flash_store_t* fs = (flash_store_t*)&flash_stores[handle];
    int status = BP_SUCCESS;
    
    bplib_os_lock(flash_device_lock);
    {          
        bp_flash_addr_t page_addr = {FLASH_GET_BLOCK((uint64_t)sid), FLASH_GET_PAGE((uint64_t)sid), BP_FLASH_DATA_TYPE};
        status = flash_object_read(fs, &page_addr, object);
    }
    bplib_os_unlock(flash_device_lock);
    
    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_flash_release -
 *-------------------------------------------------------------------------------------*/
int bplib_store_flash_release (int handle, bp_sid_t sid)
{
    assert(handle >= 0 && handle < FLASH_MAX_STORES);
    assert(flash_stores[handle].in_use);
    
    flash_store_t* fs = (flash_store_t*)&flash_stores[handle];    

    /* Check SID Matches */
    bp_object_t* object = (bp_object_t*)fs->object_stage;
    if(object->sid != sid)
    {
        return bplog(BP_FAILEDSTORE, "Object being released does not have correct SID, requested: %llu, actual: %llu\n", (uint64_t)sid, (uint64_t)object->sid);
    }

    /* Unlock Stage */
    fs->stage_locked = false;
    
    /* Return Success */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_flash_relinquish -
 *-------------------------------------------------------------------------------------*/
int bplib_store_flash_relinquish (int handle, bp_sid_t sid)
{
    assert(handle >= 0 && handle < FLASH_MAX_STORES);
    assert(flash_stores[handle].in_use);

    flash_store_t* fs = (flash_store_t*)&flash_stores[handle];
    int status = BP_SUCCESS;
    
    bplib_os_lock(flash_device_lock);
    {
        /* Check and Clear Cache */
        int cache_index = FLASH_GET_CACHE_INDEX((uint64_t)sid, fs->attributes);
        bp_object_t* cache_object_hdr = (bp_object_t*)&fs->data_cache[cache_index];
        if(cache_object_hdr->sid == sid) cache_object_hdr->sid = BP_SID_VACANT;
        
        /* Retrieve Object Header */
        bp_object_t flash_object_hdr;
        bp_flash_addr_t page_addr = {FLASH_GET_BLOCK((uint64_t)sid), FLASH_GET_PAGE((uint64_t)sid), BP_FLASH_DATA_TYPE};
        status = flash_data_read(&page_addr, (uint8_t*)&flash_object_hdr, sizeof(flash_object_hdr));
        if(status == BP_SUCCESS)
        {
            /* Delete Pages Containing Object */
            status = flash_data_delete(&page_addr, flash_object_hdr.size);
        }        
    }
    bplib_os_unlock(flash_device_lock);

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_flash_getcount -
 *-------------------------------------------------------------------------------------*/
int bplib_store_flash_getcount (int handle)
{
    assert(handle >= 0 && handle < FLASH_MAX_STORES);
    assert(flash_stores[handle].in_use);

    flash_store_t* fs = (flash_store_t*)&flash_stores[handle];
    
    return fs->object_count;
}
