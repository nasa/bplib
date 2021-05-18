/************************************************************************
 * File: flash.c
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
#include "lrc.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define FLASH_OBJECT_SYNC_HI                0x42502046
#define FLASH_OBJECT_SYNC_LO                0x4C415348

/******************************************************************************
 MACROS
 ******************************************************************************/

#define FLASH_GET_SID(addr)                 ((((addr).block * FLASH_DRIVER.pages_per_block) + (addr).page) + 1)
#define FLASH_GET_BLOCK(sid)                (((sid)-1) / FLASH_DRIVER.pages_per_block)
#define FLASH_GET_PAGE(sid)                 (((sid)-1) % FLASH_DRIVER.pages_per_block)

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    uint32_t            synchi;
    uint32_t            synclo;
    bp_object_hdr_t     object_hdr;
} flash_object_hdr_t;

typedef struct {
    bp_flash_index_t    next_block;
    bp_flash_index_t    pages_in_use;
} flash_block_control_t;

typedef struct {
    bp_flash_index_t    out;
    bp_flash_index_t    in;
    int                 count;
} flash_block_list_t;

typedef struct {
    bool                in_use;
    bool                preserve;
    int                 type; /* bp store type */
    bp_ipn_t            node;
    bp_ipn_t            service;
    bp_flash_attr_t     attributes;
    bp_flash_addr_t     write_addr;
    bp_flash_addr_t     read_addr;
    bp_flash_index_t    active_block;
    uint8_t*            write_stage; /* holding buffer to construct data object before write */
    uint8_t*            read_stage; /* lockable buffer that holds data object for read */
    bool                stage_locked;
    int                 object_count;
    int                 inactive_count;
} flash_store_t;

/******************************************************************************
 LOCAL FILE DATA
 ******************************************************************************/

/* Constants - set only in initialization function */

static bp_flash_driver_t        FLASH_DRIVER;                           /* function pointers and meta data needed to use flash */
static int                      FLASH_PAGE_DATA_SIZE = 0;               /* size in bytes of data being written to page */
static int                      FLASH_ECC_CODE_SIZE = 0;                /* size in bytes of encoding */

/* Globals */

static flash_store_t            flash_stores[FLASH_MAX_STORES];         /* available set of storage service control structures */
static flash_block_list_t       flash_free_blocks;                      /* linked list of flash blocks available for use */
static flash_block_list_t       flash_fail_blocks;                      /* linked list of flash blocks that have failed during runtime use */

static int                      flash_device_lock = BP_INVALID_HANDLE;  /* mutex for accessing flash control structures */
static flash_block_control_t*   flash_blocks = NULL;                    /* memory array of per block meta data, one per flash block */
static int                      flash_error_count = 0;                  /* total number of flash errors encountered across all storage services */
static int                      flash_used_block_count = 0;             /* total number of flash blocks currently in use by all storage services */
static uint8_t*                 flash_page_buffer = NULL;               /* memory buffer used for ECC and for object deletes */

/******************************************************************************
 LOCAL FUNCTIONS - UTILITY
 ******************************************************************************/
 
/*--------------------------------------------------------------------------------------
 * type2str -
 *-------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE const char* type2str(int type)
{
    const char* type_str = "objects";
    if(type == BP_STORE_DATA_TYPE) type_str = "bundle(s)";
    else if(type == BP_STORE_PAYLOAD_TYPE) type_str = "payload(s)";
    else if(type == BP_STORE_DACS_TYPE) type_str = "dacs(s)";
    return type_str;
}

/******************************************************************************
 LOCAL FUNCTIONS - PAGE LEVEL
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * flash_page_write -
 *-------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE int flash_page_write (bp_flash_addr_t addr, uint8_t* data, int size)
{
    assert(size <= FLASH_PAGE_DATA_SIZE);

    if(FLASH_ECC_CODE_SIZE > 0)
    {
        memcpy(flash_page_buffer, data, size);
        lrc_encode(flash_page_buffer, FLASH_PAGE_DATA_SIZE);
        return FLASH_DRIVER.write(addr, flash_page_buffer);
    }
    else
    {
        return FLASH_DRIVER.write(addr, data);
    }
}

/*--------------------------------------------------------------------------------------
 * flash_page_read -
 *-------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE int flash_page_read (bp_flash_addr_t addr, uint8_t* data, int size)
{
    assert(size <= FLASH_PAGE_DATA_SIZE);

    int status;

    if(FLASH_ECC_CODE_SIZE > 0)
    {
        status = FLASH_DRIVER.read(addr, flash_page_buffer);
        if(status == BP_SUCCESS)
        {
            int decode_status = lrc_decode(flash_page_buffer, FLASH_PAGE_DATA_SIZE);
            if(decode_status == BP_ECC_NO_ERRORS)
            {
                status = BP_SUCCESS;
            }
            else if(decode_status == BP_ECC_COR_ERRORS)
            {
                bplog(NULL, BP_FLAG_STORE_FAILURE, "Single-bit error corrected at %d.%d\n", FLASH_DRIVER.phyblk(addr.block), addr.page);
                status = BP_SUCCESS;
            }
            else /* decode_status == BP_ECC_UNCOR_ERRORS */
            {
                bplog(NULL, BP_FLAG_STORE_FAILURE, "Multiple-bit error detected at %d.%d\n", FLASH_DRIVER.phyblk(addr.block), addr.page);
                status = BP_ERROR;
            }

            /*
             * Inside flash_object_delete, the flash_page_buffer is used to read the first
             * page of an object - this is a shortcut so that the code doesn't need to allocate
             * a separate buffer just to read the beginning of the object.  It is the only
             * time in the code when this is done, and it is hardcoded to only read one page
             * and have the offset into the flash_page_buffer be zero.
             * 
             * When this occurs, we not only don't need to copy the contents out of the flash_page_buffer
             * since the "data" pointer already points to it, but on many architectures, calling
             * memcpy with overlapping address ranges causes an exception.
             * 
             * Note that there is no case in the code where the data buffer is pointing to a location
             * in memory that overlaps with the flash_page_buffer; so the check for equivalence is 
             * sufficient.
             */ 
            if(data != flash_page_buffer) 
            {
                memcpy(data, flash_page_buffer, size);
            }
        }
    }
    else
    {
        status = FLASH_DRIVER.read(addr, data);
    }

    return status;
}

/******************************************************************************
 LOCAL FUNCTIONS - BLOCK LEVEL
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * flash_block_list_add -
 *-------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE void flash_block_list_add (flash_block_list_t* list, bp_flash_index_t block)
{
    if(list->out == BP_FLASH_INVALID_INDEX)
    {
        /* First Block Successfully Added */
        list->out = block;
    }
    else
    {
        /* Link from Previous Block  */
        flash_blocks[list->in].next_block = block;
    }

    /* Add Block to List */
    list->in = block;
    list->count++;
}

/*--------------------------------------------------------------------------------------
 * flash_free_reclaim -
 *-------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE int flash_free_reclaim (bp_flash_index_t block)
{
    /* Clear Block Control Entry */
    flash_blocks[block].next_block = BP_FLASH_INVALID_INDEX;
    flash_blocks[block].pages_in_use = FLASH_DRIVER.pages_per_block;

    /* Block No Longer In Use */
    flash_used_block_count--;

    /* Add to Free or Failed List */
    if(!FLASH_DRIVER.isbad(block))
    {
        flash_block_list_add(&flash_free_blocks, block);
        return BP_SUCCESS;
    }
    else
    {
        flash_block_list_add(&flash_fail_blocks, block);
        return BP_ERROR;
    }
}

/*--------------------------------------------------------------------------------------
 * flash_free_allocate -
 *-------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE int flash_free_allocate (bp_flash_index_t* block)
{
    int status = BP_ERROR;

    /* Erase Block */
    while(status != BP_SUCCESS && flash_free_blocks.out != BP_FLASH_INVALID_INDEX)
    {
        bp_flash_index_t block_out = flash_free_blocks.out;
        status = FLASH_DRIVER.erase(block_out);
        if(status == BP_SUCCESS)
        {
            /* Return Block */
            *block = block_out;
            flash_used_block_count++;
        }
        else
        {
            /* Failed to Erase - Add to Failed Block List */
            flash_error_count++;
            flash_block_list_add(&flash_fail_blocks, block_out);
            bplog(NULL, BP_FLAG_STORE_FAILURE, "Failed to erase block %d when allocating it... adding as failed block\n", 
                                                FLASH_DRIVER.phyblk(block_out));
        }

        /* Move to Next Free Block */
        flash_free_blocks.out = flash_blocks[block_out].next_block;
        flash_free_blocks.count--;

        /* Mark Next Block Invalid */
        flash_blocks[block_out].next_block = BP_FLASH_INVALID_INDEX;
    }

    /* Log Error */
    if(status != BP_SUCCESS)
    {
        bplog(NULL, BP_FLAG_STORE_FAILURE, "No free blocks available\n");
    }

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * flash_data_write -
 *-------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE int flash_data_write (bp_flash_addr_t* addr, uint8_t* data, int size)
{
    int data_index = 0;
    int bytes_left = size;

    /* Check for Valid Address */
    if(addr->block >= FLASH_DRIVER.num_blocks || addr->page >= FLASH_DRIVER.pages_per_block)
    {
        return bplog(NULL, BP_FLAG_STORE_FAILURE, "Invalid address provided to write function: %d.%d\n", 
                                                    FLASH_DRIVER.phyblk(addr->block), addr->page);
    }

    /* Copy Into and Write Pages */
    while(bytes_left > 0)
    {
        /* Write Data into Page */
        int bytes_to_copy = bytes_left < FLASH_PAGE_DATA_SIZE ? bytes_left : FLASH_PAGE_DATA_SIZE;
        int flash_status = flash_page_write(*addr, &data[data_index], bytes_to_copy);

        /* Check if Write Failed 
         *  don't set return status of function to failure
         *  but instead count and log the error and keep going */
        if(flash_status != BP_SUCCESS)
        {
            flash_error_count++;
            bplog(NULL, BP_FLAG_STORE_FAILURE, "Error encountered writing data to flash address: %d.%d\n", 
                                                FLASH_DRIVER.phyblk(addr->block), addr->page);
        }

        /* Always Continue with Write */
        data_index += bytes_to_copy;
        bytes_left -= bytes_to_copy;
        addr->page++;

        /* Check Need to go to Next Block */
        if(addr->page >= FLASH_DRIVER.pages_per_block)
        {
            bp_flash_index_t next_write_block;
            flash_status = flash_free_allocate(&next_write_block);
            if(flash_status == BP_SUCCESS)
            {
                flash_blocks[addr->block].next_block = next_write_block;
                addr->block = next_write_block;
                addr->page = 0;
            }
            else
            {
                return bplog(NULL, BP_FLAG_STORE_FAILURE, "Failed to retrieve next free block in middle of flash write at block: %ld\n", 
                                                            FLASH_DRIVER.phyblk(addr->block));
            }
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

    /* Check for Valid Address */
    if(addr->block >= FLASH_DRIVER.num_blocks || addr->page >= FLASH_DRIVER.pages_per_block)
    {
        return bplog(NULL, BP_FLAG_STORE_FAILURE, "Invalid address provided to read function: %d.%d\n", 
                                                    FLASH_DRIVER.phyblk(addr->block), addr->page);
    }

    /* Copy Into and Write Pages */
    while(bytes_left > 0)
    {
        /* Read Data from Page */
        int bytes_to_copy = bytes_left < FLASH_PAGE_DATA_SIZE ? bytes_left : FLASH_PAGE_DATA_SIZE;
        int flash_status = flash_page_read(*addr, &data[data_index], bytes_to_copy);

        /* Check if Read Failed
         *  don't set return status of function to failure
         *  but instead count and log the error and keep going */
        if(flash_status != BP_SUCCESS)
        {
            flash_error_count++;
            bplog(NULL, BP_FLAG_STORE_FAILURE, "Failed to read data from flash address: %d.%d\n", 
                                                FLASH_DRIVER.phyblk(addr->block), addr->page);
        }

        /* Always Continue with Write */
        data_index += bytes_to_copy;
        bytes_left -= bytes_to_copy;
        addr->page++;

        /* Check Need to go to Next Block */
        if(addr->page >= FLASH_DRIVER.pages_per_block)
        {
            bp_flash_index_t next_read_block = flash_blocks[addr->block].next_block;
            if(next_read_block == BP_FLASH_INVALID_INDEX)
            {
                return bplog(NULL, BP_FLAG_STORE_FAILURE, "Failed to retrieve next block in middle of flash read at block: %ld\n", 
                                                            FLASH_DRIVER.phyblk(addr->block));
            }

            /* Goto Next Read Block */
            addr->block = next_read_block;
            addr->page = 0;
        }
    }

    return BP_SUCCESS;
}

/******************************************************************************
 LOCAL FUNCTIONS - OBJECT LEVEL
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * flash_object_write -
 *-------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE int flash_object_write (flash_store_t* fs, int handle, uint8_t* data1, int data1_size, uint8_t* data2, int data2_size)
{
    int status = BP_SUCCESS;

    /* Check if Room Available */
    uint64_t bytes_available = ((uint64_t)flash_free_blocks.count * (uint64_t)FLASH_DRIVER.pages_per_block * (uint64_t)FLASH_PAGE_DATA_SIZE) + /* full blocks remaining */
                               (((uint64_t)FLASH_DRIVER.pages_per_block - fs->write_addr.page) * (uint64_t)FLASH_PAGE_DATA_SIZE); /* current block remaining */
    int bytes_needed = sizeof(flash_object_hdr_t) + data1_size + data2_size;
    if(bytes_available >= (uint64_t)bytes_needed && fs->attributes.max_data_size >= bytes_needed)
    {
        /* Calculate Object Information */
        unsigned long sid = FLASH_GET_SID(fs->write_addr);
        flash_object_hdr_t flash_object_hdr = {
            .synchi = FLASH_OBJECT_SYNC_HI,
            .synclo = FLASH_OBJECT_SYNC_LO,
            .object_hdr = {
                .handle = handle,
                .size = data1_size + data2_size,
                .sid = (bp_sid_t)sid
            }
        };

        /* Copy into Write Stage */
        memcpy(&fs->write_stage[0], &flash_object_hdr, sizeof(flash_object_hdr_t));
        if(data1) memcpy(&fs->write_stage[sizeof(flash_object_hdr_t)], data1, data1_size);
        if(data2) memcpy(&fs->write_stage[sizeof(flash_object_hdr_t) + data1_size], data2, data2_size);

        /* Write Data into Flash */
        status = flash_data_write(&fs->write_addr, fs->write_stage, bytes_needed);
    }
    else
    {
        status = bplog(NULL, BP_FLAG_STORE_FAILURE, "Insufficient room in flash storage, max: %llu, available: %llu, needed: %llu\n", 
                                                    (long long unsigned)fs->attributes.max_data_size, (long long unsigned)bytes_available, 
                                                    (long long unsigned)bytes_needed);
    }

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * flash_object_read -
 *-------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE int flash_object_read (flash_store_t* fs, int handle, bp_flash_addr_t* addr, bp_object_t** object)
{
    int status;

    /* Check Stage Lock */
    if(fs->stage_locked == false)
    {
        flash_object_hdr_t* flash_object_hdr = (flash_object_hdr_t*)fs->read_stage;

        /* Read Object Header from Flash */
        status = flash_data_read(addr, fs->read_stage, FLASH_PAGE_DATA_SIZE);
        if(status == BP_SUCCESS)
        {
            if( (flash_object_hdr->object_hdr.size <= fs->attributes.max_data_size) &&
                (flash_object_hdr->object_hdr.handle == handle) &&
                (flash_object_hdr->synchi == FLASH_OBJECT_SYNC_HI) &&
                (flash_object_hdr->synclo == FLASH_OBJECT_SYNC_LO) )
            {
                int bytes_read = FLASH_PAGE_DATA_SIZE - sizeof(flash_object_hdr_t);
                int remaining_bytes = flash_object_hdr->object_hdr.size - bytes_read;
                if(remaining_bytes > 0)
                {
                    status = flash_data_read(addr, &fs->read_stage[FLASH_PAGE_DATA_SIZE], remaining_bytes);
                }
            }
            else
            {
                status = bplog(NULL, BP_FLAG_STORE_FAILURE, "Object read from flash fails validation, size (%d, %d), handle (%d, %d), sync (%08Xl%08Xl, %018Xl%08Xl)\n",
                                                            flash_object_hdr->object_hdr.size, fs->attributes.max_data_size,
                                                            flash_object_hdr->object_hdr.handle, handle,
                                                            flash_object_hdr->synchi, flash_object_hdr->synclo, FLASH_OBJECT_SYNC_HI, FLASH_OBJECT_SYNC_LO);
            }
        }

        /* Object Successfully Dequeued */
        if(status == BP_SUCCESS)
        {
            /* Return Locked Object */
            *object = (bp_object_t*)&flash_object_hdr->object_hdr;
            fs->stage_locked = true;
        }
    }
    else
    {
        status = bplog(NULL, BP_FLAG_STORE_FAILURE, "Object read cannot proceed when object stage is locked\n");
    }

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * flash_object_delete -
 *-------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE int flash_object_delete (flash_store_t* fs, bp_sid_t sid)
{
    int status;

    /* Get Address from SID */
    bp_flash_addr_t addr = {FLASH_GET_BLOCK((unsigned long)sid), FLASH_GET_PAGE((unsigned long)sid)};
    if(addr.block >= FLASH_DRIVER.num_blocks || addr.page >= FLASH_DRIVER.pages_per_block)
    {
        return bplog(NULL, BP_FLAG_STORE_FAILURE, "Invalid address provided to delete function: %d.%d\n", 
                                                    FLASH_DRIVER.phyblk(addr.block), addr.page);
    }

    /* Retrieve Object Header */
    flash_object_hdr_t* flash_object_hdr = (flash_object_hdr_t*)flash_page_buffer;
    flash_object_hdr->object_hdr.sid = BP_SID_VACANT;
    bp_flash_addr_t hdr_addr = addr;
    status = flash_data_read(&hdr_addr, (uint8_t*)flash_object_hdr, sizeof(flash_object_hdr_t));
    if(status != BP_SUCCESS)
    {
        return bplog(NULL, BP_FLAG_STORE_FAILURE, "Unable to read object header at %d.%d in delete function\n", 
                                                    FLASH_DRIVER.phyblk(addr.block), addr.page);
    }
    else if(flash_object_hdr->object_hdr.sid != sid)
    {
        return bplog(NULL, BP_FLAG_STORE_FAILURE, "Attempting to delete object with invalid SID: %lu != %lu\n", 
                                                    (unsigned long)flash_object_hdr->object_hdr.sid, (unsigned long)sid);
    }

    /* Setup Loop Parameters */
    bp_flash_index_t current_block = BP_FLASH_INVALID_INDEX;
    int bytes_left = flash_object_hdr->object_hdr.size;

    /* Delete Each Page of Data */
    while(bytes_left > 0)
    {
        /* Set Current Block */
        if(current_block != addr.block)
        {
            current_block = addr.block;
        }

        /* Mark Data on Page as Deleted */
        flash_blocks[current_block].pages_in_use--;

        /* Update Bytes Left and Address */
        int bytes_to_delete = bytes_left < FLASH_PAGE_DATA_SIZE ? bytes_left : FLASH_PAGE_DATA_SIZE;
        bytes_left -= bytes_to_delete;
        addr.page++;

        /* Check if Address Needs to go to Next Block */
        if(addr.page >= FLASH_DRIVER.pages_per_block)
        {
            bp_flash_index_t next_delete_block = flash_blocks[addr.block].next_block;
            if((next_delete_block == BP_FLASH_INVALID_INDEX) && (bytes_left > 0))
            {
                return bplog(NULL, BP_FLAG_STORE_FAILURE, "Failed to retrieve next block in middle of flash delete at block: %d\n", 
                                                            FLASH_DRIVER.phyblk(addr.block));
            }

            /* Goto Next Block to Delete */
            addr.block = next_delete_block;
            addr.page = 0;
        }

        /* Check if Active Block can be Updated */
        if(current_block == fs->active_block)
        {
            /* Check if Block can be Reclaimed */
            while(fs->active_block != BP_FLASH_INVALID_INDEX && flash_blocks[fs->active_block].pages_in_use == 0)
            {
                /* Get Next Block */
                bp_flash_index_t next_active_block = flash_blocks[fs->active_block].next_block;

                /* Reclaim Block as Free */
                status = flash_free_reclaim(fs->active_block);
                if(status != BP_SUCCESS)
                {
                    bplog(NULL, BP_FLAG_STORE_FAILURE, "Failed (%d) to reclaim block %d as a free block\n", 
                                                        status, FLASH_DRIVER.phyblk(fs->active_block));
                }

                /* Update Active Block */
                fs->active_block = next_active_block;
            }
        }
    }

    /* Return Success */
    return BP_SUCCESS;
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_store_flash_init -
 *-------------------------------------------------------------------------------------*/
int bplib_store_flash_init (bp_flash_driver_t driver, bool sw_edac)
{
    assert(driver.num_blocks > 0);
    assert(driver.pages_per_block > 0);
    assert(driver.page_size > 0);
    assert(driver.read);
    assert(driver.write);
    assert(driver.erase);
    assert(driver.isbad);
    assert(driver.phyblk);

    int reclaimed_blocks = 0;
    unsigned int start_block = bplib_os_random();

    /* Initialize Flash Driver */
    FLASH_DRIVER = driver;

    /* Default Constants */
    FLASH_PAGE_DATA_SIZE = FLASH_DRIVER.page_size;
    FLASH_ECC_CODE_SIZE = 0;

    /* Default Variables  */
    flash_device_lock = BP_INVALID_HANDLE;
    flash_blocks = NULL;
    flash_error_count = 0;
    flash_used_block_count = 0;
    flash_page_buffer = NULL;

    /* Zero Out Flash Stores */
    memset(flash_stores, 0, sizeof(flash_stores));

    /* Initialize Free Blocks List */
    flash_free_blocks.out = BP_FLASH_INVALID_INDEX;
    flash_free_blocks.in = BP_FLASH_INVALID_INDEX;
    flash_free_blocks.count = 0;

    /* Initialize Failed Blocks List */
    flash_fail_blocks.out = BP_FLASH_INVALID_INDEX;
    flash_fail_blocks.in = BP_FLASH_INVALID_INDEX;
    flash_fail_blocks.count = 0;

    do
    {
        /* Initialize Flash Device Lock */
        flash_device_lock = bplib_os_createlock();
        if(flash_device_lock == BP_INVALID_HANDLE)
        {
            bplog(NULL, BP_FLAG_DIAGNOSTIC, "Failed (%d) to create flash device lock\n", flash_device_lock);
            break; /* Skip rest of initialization */
        }

        /* Allocate Memory for Block Control */
        flash_blocks = (flash_block_control_t*)bplib_os_calloc(sizeof(flash_block_control_t) * FLASH_DRIVER.num_blocks);
        if(flash_blocks == NULL)
        {
            bplog(NULL, BP_FLAG_DIAGNOSTIC, "Unable to allocate memory for flash block control information\n");
            break; /* Skip rest of initialization */
        }

        /* Configure for EDAC */
        if(sw_edac)
        {
            /* Calculate Segment Sizes */
            FLASH_PAGE_DATA_SIZE = lrc_init(FLASH_DRIVER.page_size);
            FLASH_ECC_CODE_SIZE = FLASH_DRIVER.page_size - FLASH_PAGE_DATA_SIZE;
            if(FLASH_ECC_CODE_SIZE == 0)
            {
                bplog(NULL, BP_FLAG_DIAGNOSTIC, "Unable to allocate memory for software ECC\n");
                break; /* Skip rest of initialization */
            }
        }

        /* Allocate Page Buffer */
        flash_page_buffer = (uint8_t*)bplib_os_calloc(FLASH_DRIVER.page_size);
        if(!flash_page_buffer)
        {
            bplog(NULL, BP_FLAG_DIAGNOSTIC, "Unable to allocate memory for flash page buffer\n");
            break; /* Skip rest of initialization */
        }

        /* Build Free Block List */
        unsigned int block;
        for(block = 0; block < FLASH_DRIVER.num_blocks; block++)
        {
            bp_flash_index_t block_to_reclaim = (block + start_block) % FLASH_DRIVER.num_blocks;
            if(flash_free_reclaim(block_to_reclaim) == BP_SUCCESS)
            {
                reclaimed_blocks++;
            }

            /* Log Status */
            if(block % 2000 == 0)
            {
                bplog(NULL, BP_FLAG_DIAGNOSTIC, "Initializing flash... reclaiming block %d of %d\n", block, FLASH_DRIVER.num_blocks);
            }
        }

        /* Zero Out Used Flash Blocks */
        flash_used_block_count = 0;
    } while(false);

    /* Check for Success */
    if(reclaimed_blocks > 0)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Flash storage service reclaimed %d blocks, starting at block %d\n", 
                                        reclaimed_blocks, FLASH_DRIVER.phyblk(start_block % driver.num_blocks));
    }
    else
    {
        bplog(NULL, BP_FLAG_STORE_FAILURE, "Failed to initialize flash storage service\n");
        bplib_store_flash_uninit();
    }

    /* Return Number of Reclaimed Blocks */
    return reclaimed_blocks;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_flash_uninit -
 *-------------------------------------------------------------------------------------*/
void bplib_store_flash_uninit (void)
{
    int s;

    for(s = 0; s < FLASH_MAX_STORES; s++)
    {
        if(flash_stores[s].preserve || flash_stores[s].in_use)
        {
            flash_stores[s].in_use = true;
            flash_stores[s].preserve = false;
            bplib_store_flash_destroy(s);
        }
    }

    if(flash_device_lock != BP_INVALID_HANDLE)
    {
        bplib_os_destroylock(flash_device_lock);
        flash_device_lock = BP_INVALID_HANDLE;
    }

    if(flash_blocks)
    {
        bplib_os_free(flash_blocks);
        flash_blocks = NULL;
    }

    if(FLASH_ECC_CODE_SIZE > 0)
    {
        lrc_uninit();
    }

    if(flash_page_buffer)
    {
        bplib_os_free(flash_page_buffer);
        flash_page_buffer = NULL;
    }
}

/*--------------------------------------------------------------------------------------
 * bplib_store_flash_reclaim_used_blocks -
 *-------------------------------------------------------------------------------------*/
void bplib_store_flash_reclaim_used_blocks (bp_ipn_t node, bp_ipn_t service)
{
    int s;

    for(s = 0; s < FLASH_MAX_STORES; s++)
    {
        if( (flash_stores[s].in_use == false) && 
            (flash_stores[s].preserve == true) && 
            (flash_stores[s].node == node) && 
            (flash_stores[s].service == service) )
        {
            flash_stores[s].in_use = true;
            flash_stores[s].preserve = false;
            bplib_store_flash_destroy(s);
        }
    }
}

/*--------------------------------------------------------------------------------------
 * bplib_store_flash_restore_failed_blocks -
 *-------------------------------------------------------------------------------------*/
void bplib_store_flash_restore_failed_blocks (void)
{
    flash_block_list_t new_fail_blocks = {
        .out = BP_FLASH_INVALID_INDEX,
        .in = BP_FLASH_INVALID_INDEX,
        .count = 0
    };

    /* Loop Through Failed Blocks */
    int block = flash_fail_blocks.out;
    while(block != BP_FLASH_INVALID_INDEX)
    {
        bp_flash_index_t next_block = flash_blocks[block].next_block;

        /* Clear Block Control Entry */
        flash_blocks[block].next_block = BP_FLASH_INVALID_INDEX;
        flash_blocks[block].pages_in_use = FLASH_DRIVER.pages_per_block;

        /* Add to Free or Failed List */
        if(!FLASH_DRIVER.isbad(block))
        {
            flash_block_list_add(&flash_free_blocks, block);
        }
        else
        {
            flash_block_list_add(&new_fail_blocks, block);
        }

        /* Go to Next Failed Block */
        block = next_block;
    }

    /* Update Failed Blocks List */
    flash_fail_blocks = new_fail_blocks;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_flash_stats -
 *-------------------------------------------------------------------------------------*/
void bplib_store_flash_stats (bp_flash_stats_t* stats, bool log_stats, bool reset_stats)
{
    /* Copy Stats */
    if(stats)
    {
        stats->num_free_blocks = flash_free_blocks.count;
        stats->num_used_blocks = flash_used_block_count;
        stats->num_fail_blocks = flash_fail_blocks.count;
        stats->error_count = flash_error_count;
    }

    /* Log Stats */
    if(log_stats)
    {
        bplog(NULL, BP_FLAG_STORE_FAILURE, "Number of free blocks: %d\n", flash_free_blocks.count);
        bplog(NULL, BP_FLAG_STORE_FAILURE, "Number of used blocks: %d\n", flash_used_block_count);
        bplog(NULL, BP_FLAG_STORE_FAILURE, "Number of failed blocks: %d\n", flash_fail_blocks.count);
        bplog(NULL, BP_FLAG_STORE_FAILURE, "Number of flash errors: %d\n", flash_error_count);

        int block = flash_fail_blocks.out;
        while(block != BP_FLASH_INVALID_INDEX)
        {
            bplog(NULL, BP_FLAG_STORE_FAILURE, "Block <%d> failed\n", FLASH_DRIVER.phyblk(block));
            block = flash_blocks[block].next_block;
        }
    }

    /* Reset Stats */
    if(reset_stats)
    {
        flash_error_count = 0;
    }
}

/*--------------------------------------------------------------------------------------
 * bplib_store_flash_create -
 *-------------------------------------------------------------------------------------*/
int bplib_store_flash_create (int type, bp_ipn_t node, bp_ipn_t service, bool recover, void* parm)
{
    bp_flash_attr_t* attr = (bp_flash_attr_t*)parm;
    bool in_error = false;
    int handle = BP_INVALID_HANDLE;
    int s;    

    if(recover)
    {
        /* Search for Existing Store */
        for(s = 0; s < FLASH_MAX_STORES; s++)
        {
            if( (flash_stores[s].preserve == true) && 
                (flash_stores[s].type == type) &&
                (flash_stores[s].node == node) && 
                (flash_stores[s].service == service) )
            {
                if(!flash_stores[s].in_use)
                {
                    /* Recover Bundles */
                    bplog(NULL, BP_FLAG_DIAGNOSTIC, "Recovered %d %s from ipn:%d.%d in flash store\n", 
                                                    flash_stores[s].object_count, type2str(type), node, service);
                    handle = s;
                }
                else
                {
                    /* Node.Service Already In Use */
                    bplog(NULL, BP_FLAG_DIAGNOSTIC, "Store of %s for ipn:%d.%d already in use!\n", 
                                                    type2str(type), node, service);
                    in_error = true;
                }

                break;
            }
        }
    }

    if( (handle == BP_INVALID_HANDLE) && (in_error == false) )
    {
        for(s = 0; s < FLASH_MAX_STORES; s++)
        {
            if( (!flash_stores[s].in_use) && (!flash_stores[s].preserve) )
            {
                /* Initialize Attributes */
                if(attr)
                {
                    /* Check User Provided Attributes */
                    if(attr->max_data_size < FLASH_PAGE_DATA_SIZE)
                    {
                        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Invalid attributes - must supply sufficient sizes\n");
                        break;
                    }

                    /* Copy User Provided Attributes */
                    flash_stores[s].attributes = *attr;
                }
                else
                {
                    /* Use Default Attributes */
                    flash_stores[s].attributes.max_data_size = FLASH_PAGE_DATA_SIZE;
                }

                /* Account for Object Header Overhead */
                flash_stores[s].attributes.max_data_size += sizeof(flash_object_hdr_t);

                /* Round Up to Next Full Page (to optimize driver operations on page boundaries) */
                int num_pages_in_stage = (flash_stores[s].attributes.max_data_size + FLASH_DRIVER.page_size - 1) / FLASH_DRIVER.page_size;
                flash_stores[s].attributes.max_data_size = FLASH_DRIVER.page_size * num_pages_in_stage;

                /* Initialize Store Identifiers */
                flash_stores[s].type                = type;
                flash_stores[s].node                = node;
                flash_stores[s].service             = service;

                /* Initialize Read/Write Pointers */
                flash_stores[s].write_addr.block    = BP_FLASH_INVALID_INDEX;
                flash_stores[s].write_addr.page     = 0;
                flash_stores[s].read_addr.block     = BP_FLASH_INVALID_INDEX;
                flash_stores[s].read_addr.page      = 0;
                flash_stores[s].active_block        = BP_FLASH_INVALID_INDEX;

                /* Set Counts to Zero */
                flash_stores[s].object_count = 0;
                flash_stores[s].inactive_count = 0;

                /* Update Store Index and Break Out of Loop */
                handle = s;
                break;
            }
        }
    }

    /* Allocate Stage */
    if(handle != BP_INVALID_HANDLE)
    {
        flash_stores[s].stage_locked = false;
        flash_stores[s].write_stage = (uint8_t*)bplib_os_calloc(flash_stores[s].attributes.max_data_size);
        flash_stores[s].read_stage = (uint8_t*)bplib_os_calloc(flash_stores[s].attributes.max_data_size);
        if((flash_stores[s].write_stage == NULL) ||
        (flash_stores[s].read_stage == NULL) )
        {
            bplog(NULL, BP_FLAG_DIAGNOSTIC, "Unable to allocate data stages\n");
            if(flash_stores[s].write_stage) bplib_os_free(flash_stores[s].write_stage);
            if(flash_stores[s].read_stage) bplib_os_free(flash_stores[s].read_stage);
            handle = BP_INVALID_HANDLE;
        }
    }

    /* Set Store Properties */
    if(handle != BP_INVALID_HANDLE)
    {
        flash_stores[handle].in_use = true;
        flash_stores[handle].preserve = recover;
    }

    return handle;
}

/*--------------------------------------------------------------------------------------
 * bplib_store_flash_destroy -
 *-------------------------------------------------------------------------------------*/
int bplib_store_flash_destroy (int handle)
{
    assert(handle >= 0 && handle < FLASH_MAX_STORES);
    assert(flash_stores[handle].in_use);

    /* If Preserving:
     *  Reset Active Block to Read Address - this will cause any bundles that are 
     *  active to be lost.  Since this store doesn't keep track of individual free pages, 
     *  there is no way to reset the active address to the exact page (the active address 
     *  only keeps track of the block for this reason). 
     * 
     * If Not Preserving:
     *  Reclaim all blocks from the active block all the way to the end.  This drains all
     *  the blocks associated with this store from flash. */
    while( (flash_stores[handle].active_block != BP_FLASH_INVALID_INDEX) && 
           ( (!flash_stores[handle].preserve) ||
             (flash_stores[handle].active_block != flash_stores[handle].read_addr.block) ) )
    {
        /* Get Next Block */
        bp_flash_index_t next_active_block = flash_blocks[flash_stores[handle].active_block].next_block;

        /* Reclaim Block as Free */
        int status = flash_free_reclaim(flash_stores[handle].active_block);
        if(status != BP_SUCCESS)
        {
            bplog(NULL, BP_FLAG_STORE_FAILURE, "Failed (%d) to reclaim block %d as a free block\n", 
                                                status, FLASH_DRIVER.phyblk(flash_stores[handle].active_block));
        }

        /* Update Active Block */
        flash_stores[handle].active_block = next_active_block;
    }

    /* Cleanup Write Stage */
    if(flash_stores[handle].write_stage)
    {
        bplib_os_free(flash_stores[handle].write_stage);
        flash_stores[handle].write_stage = NULL;
    }

    /* Cleanup Read Stage */
    if(flash_stores[handle].read_stage)
    {
        bplib_os_free(flash_stores[handle].read_stage);
        flash_stores[handle].read_stage = NULL;
        flash_stores[handle].stage_locked = false;
    }

    /* Generate Status Message */
    if(!flash_stores[handle].preserve)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Deleting %d unpreserved %s from ipn:%d.%d in flash store\n", 
                                        flash_stores[handle].object_count, type2str(flash_stores[handle].type), 
                                        flash_stores[handle].node, flash_stores[handle].service);
    }
    else
    {
        int active_bundles = flash_stores[handle].object_count - flash_stores[handle].inactive_count;
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Deleting %d abandoned %s from ipn:%d.%d in flash store\n", 
                                        active_bundles, type2str(flash_stores[handle].type), 
                                        flash_stores[handle].node, flash_stores[handle].service);
    }


    /* Set Store Properties */
    flash_stores[handle].object_count = flash_stores[handle].inactive_count;
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
            if(status != BP_SUCCESS)
            {
                bplog(NULL, BP_FLAG_STORE_FAILURE, "Failed (%d) to allocate write block first time\n");
            }
        }

        if(status == BP_SUCCESS)
        {
            /* Check if Read Address Needs to be Set */
            if(fs->read_addr.block == BP_FLASH_INVALID_INDEX)
            {
                fs->read_addr = fs->write_addr;
            }

            /* Check if Active Address Needs to be Set */
            if(fs->active_block == BP_FLASH_INVALID_INDEX)
            {
                fs->active_block = fs->write_addr.block;
            }

            /* Write Object into Flash */
            status = flash_object_write(fs, handle, data1, data1_size, data2, data2_size);
            if(status == BP_SUCCESS)
            {
                fs->object_count++;
                fs->inactive_count++;
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
        /* Check if Data Objects Available */
        if((fs->read_addr.block != fs->write_addr.block) || (fs->read_addr.page != fs->write_addr.page))
        {
            status = flash_object_read(fs, handle, &fs->read_addr, object);
            if(status == BP_SUCCESS)
            {
                fs->inactive_count--;
            }
        }
        else
        {
            /* No Data to Return */
            status = BP_TIMEOUT;
        }
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
        bp_flash_addr_t page_addr = {FLASH_GET_BLOCK((unsigned long)sid), FLASH_GET_PAGE((unsigned long)sid)};
        status = flash_object_read(fs, handle, &page_addr, object);
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
    flash_object_hdr_t* flash_object_hdr = (flash_object_hdr_t*)fs->read_stage;
    if(flash_object_hdr->object_hdr.sid != sid)
    {
        return bplog(NULL, BP_FLAG_STORE_FAILURE, "Object being released does not have correct SID, requested: %lu, actual: %lu\n", 
                                                    (unsigned long)sid, (unsigned long)flash_object_hdr->object_hdr.sid);
    }
    else
    {
        /* Unlock Stage */
        fs->stage_locked = false;

        /* Return Success */
        return BP_SUCCESS;
    }
}

/*--------------------------------------------------------------------------------------
 * bplib_store_flash_relinquish -
 *
 *  there is no need to check the blocks being deleted against the read and write pointers
 *  because a block should only be deleted after it is dequeued and therefore no longer apart
 *  of the queue of blocks in storage
 *-------------------------------------------------------------------------------------*/
int bplib_store_flash_relinquish (int handle, bp_sid_t sid)
{
    assert(handle >= 0 && handle < FLASH_MAX_STORES);
    assert(flash_stores[handle].in_use);

    flash_store_t* fs = (flash_store_t*)&flash_stores[handle];
    int status = BP_SUCCESS;

    bplib_os_lock(flash_device_lock);
    {
        /* Delete Pages Containing Object */
        status = flash_object_delete(fs, sid);
        if(status == BP_SUCCESS)
        {
            fs->object_count--;
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
