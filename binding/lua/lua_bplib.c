/************************************************************************
 * File: lua_bplib.c
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

#include "lua_bplib.h"

#include "bplib.h"
#include "bplib_store_ram.h"
#include "bplib_store_file.h"
#include "bplib_store_flash.h"
#include "bplib_flash_sim.h"

#ifdef _WINDOWS_
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>


/******************************************************************************
 DEFINES
 ******************************************************************************/

#define lualog(m,...) log_message(__FILE__,__LINE__,m,##__VA_ARGS__)
#define LBPLIB_MAX_LOG_ENTRY 128

/******************************************************************************
 EXTERNS
 ******************************************************************************/

int bplib_unittest (void);

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    bp_desc_t channel;
} lbplib_user_data_t;

typedef struct {
    const char* name;
    bp_store_t  store;
} lbplib_store_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

/* Bundle Protocol Library */
int lbplib_open     (lua_State* L);
int lbplib_route    (lua_State* L);
int lbplib_display  (lua_State* L);
int lbplib_eid2ipn  (lua_State* L);
int lbplib_ipn2eid  (lua_State* L);
int lbplib_unittest (lua_State* L);
int lbplib_sleep    (lua_State* L);

/* Bundle Protocol Meta Functions */
int lbplib_delete   (lua_State* L);
int lbplib_getopt   (lua_State* L);
int lbplib_setopt   (lua_State* L);
int lbplib_stats    (lua_State* L);
int lbplib_store    (lua_State* L);
int lbplib_load     (lua_State* L);
int lbplib_process  (lua_State* L);
int lbplib_accept   (lua_State* L);
int lbplib_flush    (lua_State* L);

/******************************************************************************
 FILE DATA
 ******************************************************************************/

/* Lua Environment Variables */
static const char* LUA_BPLIBMETANAME = "Lua.bplib";
static const char* LUA_ERRNO = "errno";

/* Lua Bplib Library Functions */
static const struct luaL_Reg lbplib_functions [] = {
    {"open",        lbplib_open},
    {"route",       lbplib_route},
    {"display",     lbplib_display},
    {"eid2ipn",     lbplib_eid2ipn},
    {"ipn2eid",     lbplib_ipn2eid},
    {"unittest",    lbplib_unittest},
    {"sleep",       lbplib_sleep},
    {NULL, NULL}
};

/* Lua Bplib Channel Meta Data */
static const struct luaL_Reg lbplib_metadata [] = {
    {"getopt",      lbplib_getopt},
    {"setopt",      lbplib_setopt},
    {"stats",       lbplib_stats},
    {"store",       lbplib_store},
    {"load",        lbplib_load},
    {"process",     lbplib_process},
    {"accept",      lbplib_accept},    
    {"flush",       lbplib_flush},
    {"close",       lbplib_delete},
    {"__gc",        lbplib_delete},
    {NULL, NULL}
};

static const lbplib_store_t lbplib_stores[] = 
{
    {
        .name = "RAM",
        .store = 
        {
            .create     = bplib_store_ram_create,
            .destroy    = bplib_store_ram_destroy,
            .enqueue    = bplib_store_ram_enqueue,
            .dequeue    = bplib_store_ram_dequeue,
            .retrieve   = bplib_store_ram_retrieve,
            .release    = bplib_store_ram_release,
            .relinquish = bplib_store_ram_relinquish,
            .getcount   = bplib_store_ram_getcount,
        }
    },
    {
        .name = "FILE",
        .store = 
        {
            .create     = bplib_store_file_create,
            .destroy    = bplib_store_file_destroy,
            .enqueue    = bplib_store_file_enqueue,
            .dequeue    = bplib_store_file_dequeue,
            .retrieve   = bplib_store_file_retrieve,
            .release    = bplib_store_file_release,
            .relinquish = bplib_store_file_relinquish,
            .getcount   = bplib_store_file_getcount,
        }
    },
    {
        .name = "FLASH",
        .store = 
        {
            .create     = bplib_store_flash_create,
            .destroy    = bplib_store_flash_destroy,
            .enqueue    = bplib_store_flash_enqueue,
            .dequeue    = bplib_store_flash_dequeue,
            .retrieve   = bplib_store_flash_retrieve,
            .release    = bplib_store_flash_release,
            .relinquish = bplib_store_flash_relinquish,
            .getcount   = bplib_store_flash_getcount,
        }
    }
};

#define LBPLIB_NUM_STORES (sizeof(lbplib_stores) / sizeof(lbplib_store_t))

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

/*----------------------------------------------------------------------------
 * log_message
 *----------------------------------------------------------------------------*/
void log_message(const char* file_name, unsigned int line_number, const char* format_string, ...)
{
    char formatted_string[LBPLIB_MAX_LOG_ENTRY];
    char entry_log_msg[LBPLIB_MAX_LOG_ENTRY];

    /* Build Formatted Log Entry String */
    va_list args;
    va_start(args, format_string);
    int vlen = vsnprintf(formatted_string, LBPLIB_MAX_LOG_ENTRY - 1, format_string, args);
    int msglen = vlen < (LBPLIB_MAX_LOG_ENTRY - 1) ? vlen : (LBPLIB_MAX_LOG_ENTRY - 1);
    va_end(args);

    /* Terminate String */
    if (msglen < 0) return;
    formatted_string[msglen] = '\0';

    /* Adjust Filename to Exclude Path */
#ifdef _WINDOWS_
    const char* last_path_delimeter = strrchr(file_name, '\\');
#else
    const char* last_path_delimeter = strrchr(file_name, '/');
#endif
    const char* file_name_only = last_path_delimeter ? last_path_delimeter + 1 : file_name;
    
    /* Build Message */
    snprintf(entry_log_msg, LBPLIB_MAX_LOG_ENTRY, "%s:%u: %s", file_name_only, line_number, formatted_string);
    
    /* Print Message */
    printf("%s", entry_log_msg);
}

/*----------------------------------------------------------------------------
 * set_errno
 *----------------------------------------------------------------------------*/
static void set_errno (lua_State* L, int error_code)
{
    lua_pushnumber(L, error_code);
    lua_setglobal(L, LUA_ERRNO);    
}

/*----------------------------------------------------------------------------
 * push_flag_table
 *----------------------------------------------------------------------------*/
static void push_flag_table (lua_State* L, uint16_t flags)
{
    lua_newtable(L);

    lua_pushstring(L, "noncompliant");
    lua_pushboolean(L, (flags & BP_FLAG_NONCOMPLIANT) != 0);
    lua_settable(L, -3);
   
    lua_pushstring(L, "incomplete");
    lua_pushboolean(L, (flags & BP_FLAG_INCOMPLETE) != 0);
    lua_settable(L, -3);

    lua_pushstring(L, "unreliabletime");
    lua_pushboolean(L, (flags & BP_FLAG_UNRELIABLETIME) != 0);
    lua_settable(L, -3);

    lua_pushstring(L, "filloverflow");
    lua_pushboolean(L, (flags & BP_FLAG_FILLOVERFLOW) != 0);
    lua_settable(L, -3);

    lua_pushstring(L, "toomanyfills");
    lua_pushboolean(L, (flags & BP_FLAG_TOOMANYFILLS) != 0);
    lua_settable(L, -3);

    lua_pushstring(L, "cidwentbackwards");
    lua_pushboolean(L, (flags & BP_FLAG_CIDWENTBACKWARDS) != 0);
    lua_settable(L, -3);

    lua_pushstring(L, "routeneeded");
    lua_pushboolean(L, (flags & BP_FLAG_ROUTENEEDED) != 0);
    lua_settable(L, -3);

    lua_pushstring(L, "storefailure");
    lua_pushboolean(L, (flags & BP_FLAG_STOREFAILURE) != 0);
    lua_settable(L, -3);

    lua_pushstring(L, "unknowncid");
    lua_pushboolean(L, (flags & BP_FLAG_UNKNOWNCID) != 0);
    lua_settable(L, -3);

    lua_pushstring(L, "sdnvoverflow");
    lua_pushboolean(L, (flags & BP_FLAG_SDNVOVERFLOW) != 0);
    lua_settable(L, -3);

    lua_pushstring(L, "sdnincomplete");
    lua_pushboolean(L, (flags & BP_FLAG_SDNVINCOMPLETE) != 0);
    lua_settable(L, -3);

    lua_pushstring(L, "activetablewrap");
    lua_pushboolean(L, (flags & BP_FLAG_ACTIVETABLEWRAP) != 0);
    lua_settable(L, -3);

    lua_pushstring(L, "duplicates");
    lua_pushboolean(L, (flags & BP_FLAG_DUPLICATES) != 0);
    lua_settable(L, -3);

    lua_pushstring(L, "rbtreefull");
    lua_pushboolean(L, (flags & BP_FLAG_RBTREEFULL) != 0);
    lua_settable(L, -3);
}

/******************************************************************************
 LIBRARY FUNCTIONS
 ******************************************************************************/

/*----------------------------------------------------------------------------
 * luaopen_bplib
 *----------------------------------------------------------------------------*/
int luaopen_bplib (lua_State *L)
{
    /* Initialize Bundle Protocol Library */
    bplib_init();

    /* Initialize RAM Storage Services */
    bplib_store_ram_init();

    /* Initialize FILE Storage Services */
    bplib_store_file_init();

    /* Initialize FLASH Storage Services */
    bp_flash_driver_t flash_driver = {
        .num_blocks = FLASH_SIM_NUM_BLOCKS,
        .pages_per_block = FLASH_SIM_PAGES_PER_BLOCK,
        .data_size = FLASH_SIM_DATA_SIZE,
        .spare_size = FLASH_SIM_SPARE_SIZE,
        .read = bplib_flash_sim_page_read,
        .write = bplib_flash_sim_page_write,
        .erase = bplib_flash_sim_block_erase
    };
    bplib_flash_sim_initialize();
    bplib_store_flash_init(flash_driver);
    
    /* Initialize Errno */
    set_errno(L, 0);

    /* Create User Data */
    luaL_newmetatable(L, LUA_BPLIBMETANAME);    /* metatable.__index = metatable */
    lua_pushvalue(L, -1);                       /* duplicates the metatable */
    lua_setfield(L, -2, "__index");

    /* Associate Meta Data */
    luaL_setfuncs(L, lbplib_metadata, 0);

    /* Create Functions */
    luaL_newlib(L, lbplib_functions);

    return 1;
}

/******************************************************************************
 EXTENSION FUNCTIONS
 ******************************************************************************/

/*----------------------------------------------------------------------------
 * lbplib_open - bplib.open(<src node>, <src serv>, <dst node>, <dst serv>, <storage service>) --> channel
 *----------------------------------------------------------------------------*/
int lbplib_open (lua_State* L)
{
    unsigned int i;
    const bp_store_t* store = NULL;

    /* Check Number of Parameters */
    int minargs = 5;
    if(lua_gettop(L) < minargs)
    {
        lualog("incorrect number of parameters - expected %d\n", minargs);
        lua_pushnil(L);
        return 1;
    }
    
    /* Type Check Parameters */
    if(!lua_isnumber(L, 1) ||
       !lua_isnumber(L, 2) || 
       !lua_isnumber(L, 3) || 
       !lua_isnumber(L, 4) ||
       !lua_isstring(L, 5))
    {
        lualog("incorrect parameter types\n");
        lua_pushnil(L);
        return 1;
    }

    /* Get IPNs */  
    bp_route_t route;
    route.local_node            = lua_tonumber(L, 1);
    route.local_service         = lua_tonumber(L, 2);
    route.destination_node      = lua_tonumber(L, 3);
    route.destination_service   = lua_tonumber(L, 4);
    route.report_node           = 0;
    route.report_service        = 0;

    /* Get Storage Service */
    const char* storage_service = lua_tostring(L, 5);
    for(i = 0; i < LBPLIB_NUM_STORES; i++)
    {
        if(strcmp(storage_service, lbplib_stores[i].name) == 0)
        {
            store = &lbplib_stores[i].store;
            break;
        }
    }

    /* Check Storage Service */
    if(store == NULL)
    {
        lualog("invalid store provided: %s\n", storage_service ? storage_service : "nil");
        lua_pushnil(L);
        return 1;
    }        

    /* Initialize with Default Attributes */
    bp_attr_t attributes;
    bplib_attrinit(&attributes);

    /* Override if Attribute Table Provided */
    if(lua_type(L, 6) == LUA_TTABLE)
    {
        /* Set Attributes on Stack */    
        lua_getfield(L, 6, "lifetime");
        lua_getfield(L, 6, "request_custody");
        lua_getfield(L, 6, "admin_record");
        lua_getfield(L, 6, "integrity_check");
        lua_getfield(L, 6, "allow_fragmentation");
        lua_getfield(L, 6, "cipher_suite");
        lua_getfield(L, 6, "timeout");
        lua_getfield(L, 6, "max_length");
        lua_getfield(L, 6, "cid_reuse");
        lua_getfield(L, 6, "dacs_rate");
        lua_getfield(L, 6, "protocol_version");
        lua_getfield(L, 6, "retransmit_order");
        lua_getfield(L, 6, "active_table_size");
        lua_getfield(L, 6, "max_fills_per_dacs");
        lua_getfield(L, 6, "max_gaps_per_dacs");

        /* Get Attributes from Stack */
        attributes.lifetime             = luaL_optnumber(L, -15, attributes.lifetime);
        attributes.request_custody      = luaL_optnumber(L, -14, attributes.request_custody);
        attributes.admin_record         = luaL_optnumber(L, -13, attributes.admin_record);
        attributes.integrity_check      = luaL_optnumber(L, -12, attributes.integrity_check);
        attributes.allow_fragmentation  = luaL_optnumber(L, -11, attributes.allow_fragmentation);
        attributes.cipher_suite         = luaL_optnumber(L, -10, attributes.cipher_suite);
        attributes.timeout              = luaL_optnumber(L, -9,  attributes.timeout);
        attributes.max_length           = luaL_optnumber(L, -8,  attributes.max_length);
        attributes.cid_reuse            = luaL_optnumber(L, -7,  attributes.cid_reuse);
        attributes.dacs_rate            = luaL_optnumber(L, -6,  attributes.dacs_rate);
        attributes.protocol_version     = luaL_optnumber(L, -5,  attributes.protocol_version);
        attributes.retransmit_order     = luaL_optnumber(L, -4,  attributes.retransmit_order);
        attributes.active_table_size    = luaL_optnumber(L, -3,  attributes.active_table_size);
        attributes.max_fills_per_dacs   = luaL_optnumber(L, -2,  attributes.max_fills_per_dacs);
        attributes.max_gaps_per_dacs    = luaL_optnumber(L, -1,  attributes.max_gaps_per_dacs);
        attributes.storage_service_parm = NULL;      
    }
    
    /* Create Bplib Channel */
    bp_desc_t channel = bplib_open(route, *store, attributes);
    if(channel == BP_INVALID_DESCRIPTOR)
    {
        lua_pushnil(L);
        return 1;
    }

    /* Create Lua User Data Object */
    lbplib_user_data_t* bplib_data = (lbplib_user_data_t*)lua_newuserdata(L, sizeof(lbplib_user_data_t));
    bplib_data->channel = channel;

    /* Return lbplib_user_data_t to Lua */
    luaL_getmetatable(L, LUA_BPLIBMETANAME);
    lua_setmetatable(L, -2); /* associates the meta table with the user data */
    return 1;
}

/*----------------------------------------------------------------------------
 * lbplib_route - bplib.route(<bundle>) --> return code, dst node, dst serv
 *----------------------------------------------------------------------------*/
int lbplib_route (lua_State* L)
{
    /* Check Number of Parameters */
    int minargs = 1;
    if(lua_gettop(L) != minargs)
    {
        lualog("incorrect number of parameters - expected %d\n", minargs);
        lua_pushboolean(L, false); /* push result as fail */
        return 1;
    }
    
    /* Type Check Parameters */
    if(!lua_isstring(L, 1))
    {
        lualog("incorrect parameter type\n");
        lua_pushboolean(L, false); /* push result as fail */
        return 1;
    }

    /* Route Info */
    size_t size = 0;
    const char* bundle = lua_tolstring(L, 1, &size);
    bp_route_t route;
    int status = bplib_routeinfo((void*)bundle, size, &route);
    set_errno(L, status);
    
    /* Return */
    lua_pushboolean(L, status == BP_SUCCESS);
    lua_pushnumber(L, route.destination_node);
    lua_pushnumber(L, route.destination_service);
    return 3;
}

/*----------------------------------------------------------------------------
 * lbplib_display - bplib.lbplib_display(<bundle>,<ascii flag>) --> return code, flags
 *----------------------------------------------------------------------------*/
int lbplib_display (lua_State* L)
{
    /* Check Number of Parameters */
    int minargs = 1;
    if(lua_gettop(L) < minargs)
    {
        lualog("insufficient number of parameters - need at least %d\n", minargs);
        lua_pushboolean(L, false); /* push result as fail */
        return 1;
    }
    
    /* Type Check Parameters */
    if(!lua_isstring(L, 1))
    {
        lualog("incorrect parameter type\n");
        lua_pushboolean(L, false); /* push result as fail */
        return 1;
    }

    /* Get Bundle */
    size_t size = 0;
    const char* bundle = lua_tolstring(L, 1, &size);

    /* Check for Optional Parameter */
    bool ascii_bundle = false;
    if(lua_isboolean(L, 2))
    {
        ascii_bundle = lua_toboolean(L, 2);
    }

    /* Convert Bundle String */
    unsigned len = size;
    unsigned char* buffer = (unsigned char*)bundle;
    if(ascii_bundle == true)
    {
        /* Check Size of String */
        if(size % 2 != 0)
        {
            lualog("ASCII bundle must be even length: %d\n", size);
            lua_pushboolean(L, false); /* push result as fail */
            return 1;
        }

        /* Parse String */
        len = size / 2;
        buffer = (unsigned char*)malloc(size);    
        char b[5] = {'0', 'x', '\0', '\0', '\0'};
        unsigned int n = 0, s = 0, d = 0;
        while(s < size)
        {
            /* Read Next Character from Bundle */
            char ch = bundle[s++];

            /* Load Character into Character Array */
            b[2 + (n++ % 2)] = (char)ch;
            if(n % 2 == 0)
            {
                /* Convert Arrary into Binary Value */
                char *endptr;
                errno = 0;
                unsigned long result = strtoul(b, &endptr, 0);
                if( (endptr == b) ||
                    ((result == ULONG_MAX || result == 0) && errno == ERANGE) )
                {
                    free(buffer);
                    lualog("Invalid data in ASCII bundle: %s\n", b);
                    lua_pushboolean(L, false); /* push result as fail */
                    return 1;
                }

                /* Load Binary Value into Result Buffer */
                buffer[d++] = (unsigned char)result;
            }
        }            
    }

    /* Display */
    uint16_t flags = 0;
    int status = bplib_display((void*)buffer, len, &flags);
    set_errno(L, status);

    /* Clean Up from ASCII Bundle */
    if(ascii_bundle == true)
    {
        free(buffer);
    }
    
    /* Return */
    lua_pushboolean(L, status == BP_SUCCESS);
    lua_pushnumber(L, flags);
    return 2;
}

/*----------------------------------------------------------------------------
 * lbplib_eid2ipn - bplib.eid2ipn(<eid>) --> return code, node, service, 
 *----------------------------------------------------------------------------*/
int lbplib_eid2ipn (lua_State* L)
{
    /* Check Number of Parameters */
    int minargs = 1;
    if(lua_gettop(L) != minargs)
    {
        lualog("incorrect number of parameters - expected %d\n", minargs);
        lua_pushboolean(L, false); /* push result as fail */
        return 1;
    }
    
    /* Type Check Parameters */
    if(!lua_isstring(L, 1))
    {
        lualog("incorrect parameter type\n");
        lua_pushboolean(L, false); /* push result as fail */
        return 1;
    }

    /* EID to IPN */
    size_t size = 0;
    const char* eid = lua_tolstring(L, 1, &size);
    bp_ipn_t node = BP_IPN_NULL, service = BP_IPN_NULL;
    int status = bplib_eid2ipn(eid, size, &node, &service);
    set_errno(L, status);

    /* Return */
    lua_pushboolean(L, status == BP_SUCCESS);
    lua_pushnumber(L, node);
    lua_pushnumber(L, service);
    return 3;
}

/*----------------------------------------------------------------------------
 * lbplib_ipn2eid - bplib.ipn2eid(<node>, <service>) --> return code, eid
 *----------------------------------------------------------------------------*/
int lbplib_ipn2eid (lua_State* L)
{
    /* Check Number of Parameters */
    int minargs = 2;
    if(lua_gettop(L) != minargs)
    {
        lualog("incorrect number of parameters - expected %d\n", minargs);
        lua_pushboolean(L, false); /* push result as fail */
        return 1;
    }
    
    /* Type Check Parameters */
    if(!lua_isnumber(L, 1) ||
       !lua_isnumber(L, 2))
    {
        lualog("incorrect parameter types\n");
        lua_pushboolean(L, false); /* push result as fail */
        return 1;
    }

    /* IPN to EID */
    bp_ipn_t node = (bp_ipn_t)lua_tonumber(L, 1);
    bp_ipn_t service = (bp_ipn_t)lua_tonumber(L, 2);
    char eid[BP_MAX_EID_STRING];
    memset(eid, 0, BP_MAX_EID_STRING);
    int status = bplib_ipn2eid(eid, BP_MAX_EID_STRING, node, service);
    set_errno(L, status);

    /* Return */
    lua_pushboolean(L, status == BP_SUCCESS);
    lua_pushstring(L, eid);
    return 2;
}

/*----------------------------------------------------------------------------
 * lbplib_unittest - bplib.unittest() --> number of failures
 *----------------------------------------------------------------------------*/
int lbplib_unittest (lua_State* L)
{
    int failures = bplib_unittest();

    lua_pushnumber(L, failures);

    return 1;
}

/*----------------------------------------------------------------------------
 * lbplib_sleep - bplib.sleep(s) --> sleeps for 's' number of seconds
 *----------------------------------------------------------------------------*/
int lbplib_sleep (lua_State* L)
{
    if(lua_isnumber(L, 1))
    {
        int wait_time = lua_tonumber(L, 1); /* seconds */
#ifdef _WINDOWS_
        Sleep(wait_time * 1000); /* milliseconds */
#else
        usleep(wait_time * 1000000.0); /* seconds */
#endif
    }
    else
    {
        lualog("did not provide seconds to sleep");
    }

    return 0;
}

/******************************************************************************
 BUNDLE PROTOCOL META FUNCTIONS
 ******************************************************************************/

/*----------------------------------------------------------------------------
 * lbplib_delete
 *----------------------------------------------------------------------------*/
int lbplib_delete (lua_State* L)
{
    /* Get User Data */
    lbplib_user_data_t* bplib_data = (lbplib_user_data_t*)luaL_checkudata(L, 1, LUA_BPLIBMETANAME);
    if(!bplib_data) 
    {
        lualog("unable to retrieve user data object: %s\n", LUA_BPLIBMETANAME);
    }
    else
    {
        /* Close Channel */        
        bplib_close(bplib_data->channel);
        bplib_data->channel = BP_INVALID_DESCRIPTOR;
    }

    return 0;
}

/*----------------------------------------------------------------------------
 * lbplib_config - channel:getopt(<option>) --> return code, value
 *----------------------------------------------------------------------------*/
int lbplib_getopt (lua_State* L)
{
    /* Get User Data */
    lbplib_user_data_t* bplib_data = (lbplib_user_data_t*)luaL_checkudata(L, 1, LUA_BPLIBMETANAME);
    if(!bplib_data) 
    {
        lualog("unable to retrieve user data object: %s\n", LUA_BPLIBMETANAME);
        lua_pushboolean(L, false); /* push result as fail */
        return 1;
    }

    /* Check Number of Parameters */
    int minargs = 2;
    if(lua_gettop(L) != minargs)
    {
        lualog("incorrect number of parameters - expected %d\n", minargs);
        lua_pushboolean(L, false); /* push result as fail */
        return 1;
    }
    
    /* Type Check Parameters */
    if(!lua_isstring(L, 2))
    {
        lualog("incorrect parameters types\n");
        lua_pushboolean(L, false); /* push result as fail */
        return 1;
    }

    /* Get Option */
    const char* optstr = lua_tostring(L, 2);
    if(strcmp(optstr, "LIFETIME") == 0)
    {
        int lifetime;
        int status = bplib_config(bplib_data->channel, BP_OPT_MODE_READ, BP_OPT_LIFETIME, &lifetime);
        set_errno(L, status);
        lua_pushboolean(L, status == BP_SUCCESS);
        double lua_lifetime = (double)lifetime;         
        lua_pushnumber(L, lua_lifetime);
        return 2;
    }
    else if(strcmp(optstr, "REQUEST_CUSTODY") == 0)
    {
        int cstrqst;
        int status = bplib_config(bplib_data->channel, BP_OPT_MODE_READ, BP_OPT_REQUEST_CUSTODY, &cstrqst);
        set_errno(L, status);
        lua_pushboolean(L, status == BP_SUCCESS);
        bool lua_cstrqst = cstrqst == 1;         
        lua_pushboolean(L, lua_cstrqst);
        return 2;
    }
    else if(strcmp(optstr, "ADMIN_RECORD") == 0)
    {
        int admin;
        int status = bplib_config(bplib_data->channel, BP_OPT_MODE_READ, BP_OPT_ADMIN_RECORD, &admin);
        set_errno(L, status);
        lua_pushboolean(L, status == BP_SUCCESS);
        bool lua_admin = admin == 1;         
        lua_pushboolean(L, lua_admin);
        return 2;
    }
    else if(strcmp(optstr, "INTEGRITY_CHECK") == 0)
    {
        int icheck;
        int status = bplib_config(bplib_data->channel, BP_OPT_MODE_READ, BP_OPT_INTEGRITY_CHECK, &icheck);
        set_errno(L, status);
        lua_pushboolean(L, status == BP_SUCCESS);
        bool lua_icheck = icheck == 1;         
        lua_pushboolean(L, lua_icheck);
        return 2;
    }
    else if(strcmp(optstr, "ALLOW_FRAGMENTATION") == 0)
    {
        int frag;
        int status = bplib_config(bplib_data->channel, BP_OPT_MODE_READ, BP_OPT_ALLOW_FRAGMENTATION, &frag);
        set_errno(L, status);
        lua_pushboolean(L, status == BP_SUCCESS);
        bool lua_frag = frag == 1;         
        lua_pushboolean(L, lua_frag);
        return 2;
    }
    else if(strcmp(optstr, "CIPHER_SUITE") == 0)
    {
        int paycrc;
        int status = bplib_config(bplib_data->channel, BP_OPT_MODE_READ, BP_OPT_CIPHER_SUITE, &paycrc);
        lua_pushboolean(L, status == BP_SUCCESS);
        set_errno(L, status);
        double lua_paycrc = (double)paycrc;         
        lua_pushnumber(L, lua_paycrc);
        return 2;
    }
    else if(strcmp(optstr, "TIMEOUT") == 0)
    {
        int timeout;
        int status = bplib_config(bplib_data->channel, BP_OPT_MODE_READ, BP_OPT_TIMEOUT, &timeout);
        lua_pushboolean(L, status == BP_SUCCESS);
        set_errno(L, status);
        double lua_timeout = (double)timeout;         
        lua_pushnumber(L, lua_timeout);
        return 2;
    }
    else if(strcmp(optstr, "MAX_LENGTH") == 0)
    {
        int len;
        int status = bplib_config(bplib_data->channel, BP_OPT_MODE_READ, BP_OPT_MAX_LENGTH, &len);
        set_errno(L, status);
        lua_pushboolean(L, status == BP_SUCCESS);
        double lua_len = (double)len;         
        lua_pushnumber(L, lua_len);
        return 2;
    }
    else if(strcmp(optstr, "CID_REUSE") == 0)
    {
        int reuse;
        int status = bplib_config(bplib_data->channel, BP_OPT_MODE_READ, BP_OPT_CID_REUSE, &reuse);
        set_errno(L, status);
        lua_pushboolean(L, status == BP_SUCCESS);
        bool lua_reuse = reuse == 1;         
        lua_pushboolean(L, lua_reuse);
        return 2;
    }
    else if(strcmp(optstr, "DACS_RATE") == 0)
    {
        int rate;
        int status = bplib_config(bplib_data->channel, BP_OPT_MODE_READ, BP_OPT_DACS_RATE, &rate);
        set_errno(L, status);
        lua_pushboolean(L, status == BP_SUCCESS);
        double lua_rate = (double)rate;         
        lua_pushnumber(L, lua_rate);
        return 2;
    }

    /* Unrecognized Option */
    lualog("unrecognized option: %s\n", optstr);
    lua_pushboolean(L, false);
    return 1;
}

/*----------------------------------------------------------------------------
 * lbplib_config - channel:setopt(<option>, <value>) --> return code
 *----------------------------------------------------------------------------*/
int lbplib_setopt (lua_State* L)
{
    /* Get User Data */
    lbplib_user_data_t* bplib_data = (lbplib_user_data_t*)luaL_checkudata(L, 1, LUA_BPLIBMETANAME);
    if(!bplib_data) 
    {
        lualog("unable to retrieve user data object: %s\n", LUA_BPLIBMETANAME);
        lua_pushboolean(L, false); /* push result as fail */
        return 1;
    }

    /* Check Number of Parameters */
    int minargs = 3;
    if(lua_gettop(L) != minargs)
    {
        lualog("incorrect number of parameters - expected %d\n", minargs);
        lua_pushboolean(L, false); /* push result as fail */
        return 1;
    }
    
    /* Type Check Parameters */
    if(!lua_isstring(L, 2))
    {
        lualog("incorrect parameter type\n");
        lua_pushboolean(L, false); /* push result as fail */
        return 1;
    }

    /* Get Option */
    int status = BP_PARMERR;
    const char* optstr = lua_tostring(L, 2);
    if((strcmp(optstr, "LIFETIME") == 0) && lua_isnumber(L, 3))
    {
        int lifetime = (int)lua_tonumber(L, 3);
        status = bplib_config(bplib_data->channel, BP_OPT_MODE_WRITE, BP_OPT_LIFETIME, &lifetime);
    }
    else if((strcmp(optstr, "REQUEST_CUSTODY") == 0) && lua_isboolean(L, 3))
    {
        int cstrqst = (int)lua_toboolean(L, 3);
        status = bplib_config(bplib_data->channel, BP_OPT_MODE_WRITE, BP_OPT_REQUEST_CUSTODY, &cstrqst);
    }
    else if((strcmp(optstr, "ADMIN_RECORD") == 0) && lua_isboolean(L, 3))
    {
        int admin = (int)lua_toboolean(L, 3);
        status = bplib_config(bplib_data->channel, BP_OPT_MODE_WRITE, BP_OPT_ADMIN_RECORD, &admin);
    }
    else if((strcmp(optstr, "INTEGRITY_CHECK") == 0) && lua_isboolean(L, 3))
    {
        int icheck = (int)lua_toboolean(L, 3);
        status = bplib_config(bplib_data->channel, BP_OPT_MODE_WRITE, BP_OPT_INTEGRITY_CHECK, &icheck);
    }
    else if((strcmp(optstr, "ALLOW_FRAGMENTATION") == 0) && lua_isboolean(L, 3))
    {
        int frag = (int)lua_toboolean(L, 3);
        status = bplib_config(bplib_data->channel, BP_OPT_MODE_WRITE, BP_OPT_ALLOW_FRAGMENTATION, &frag);
    }
    else if((strcmp(optstr, "CIPHER_SUITE") == 0) && lua_isnumber(L, 3))
    {
        int paycrc = (int)lua_tonumber(L, 3);
        status = bplib_config(bplib_data->channel, BP_OPT_MODE_WRITE, BP_OPT_CIPHER_SUITE, &paycrc);
    }
    else if((strcmp(optstr, "TIMEOUT") == 0) && lua_isnumber(L, 3))
    {
        int timeout = (int)lua_tonumber(L, 3);
        status = bplib_config(bplib_data->channel, BP_OPT_MODE_WRITE, BP_OPT_TIMEOUT, &timeout);
    }
    else if((strcmp(optstr, "MAX_LENGTH") == 0) && lua_isnumber(L, 3))
    {
        int len = (int)lua_tonumber(L, 3);
        status = bplib_config(bplib_data->channel, BP_OPT_MODE_WRITE, BP_OPT_MAX_LENGTH, &len);
    }
    else if((strcmp(optstr, "CID_REUSE") == 0) && lua_isboolean(L, 3))
    {
        int reuse = (int)lua_toboolean(L, 3);
        status = bplib_config(bplib_data->channel, BP_OPT_MODE_WRITE, BP_OPT_CID_REUSE, &reuse);
    }
    else if((strcmp(optstr, "DACS_RATE") == 0) && lua_isnumber(L, 3))
    {
        int rate = (int)lua_tonumber(L, 3);
        status = bplib_config(bplib_data->channel, BP_OPT_MODE_WRITE, BP_OPT_DACS_RATE, &rate);
    }

    /* Return Status */
    set_errno(L, status);
    lua_pushboolean(L, status == BP_SUCCESS);
    return 1;
}

/*----------------------------------------------------------------------------
 * lbplib_stats - channel:stats() --> return code, channel statistics
 *----------------------------------------------------------------------------*/
int lbplib_stats (lua_State* L)
{
    /* Get User Data */
    lbplib_user_data_t* bplib_data = (lbplib_user_data_t*)luaL_checkudata(L, 1, LUA_BPLIBMETANAME);
    if(!bplib_data) 
    {
        lualog("unable to retrieve user data object: %s\n", LUA_BPLIBMETANAME);
        lua_pushboolean(L, false); /* push result as fail */
        return 1;
    }

    /* Get Statistics */
    bp_stats_t stats;
    int status = bplib_latchstats(bplib_data->channel, &stats);
    set_errno(L, status);
    lua_pushboolean(L, status == BP_SUCCESS);
    
    /* Create Statistics Table */
    lua_newtable(L);

    lua_pushstring(L, "lost");
    lua_pushnumber(L, stats.lost);
    lua_settable(L, -3);

    lua_pushstring(L, "expired");
    lua_pushnumber(L, stats.expired);
    lua_settable(L, -3);

    lua_pushstring(L, "unrecognized");
    lua_pushnumber(L, stats.unrecognized);
    lua_settable(L, -3);

    lua_pushstring(L, "transmitted_bundles");
    lua_pushnumber(L, stats.transmitted_bundles);
    lua_settable(L, -3);

    lua_pushstring(L, "transmitted_dacs");
    lua_pushnumber(L, stats.transmitted_dacs);
    lua_settable(L, -3);

    lua_pushstring(L, "retransmitted_bundles");
    lua_pushnumber(L, stats.retransmitted_bundles);
    lua_settable(L, -3);

    lua_pushstring(L, "delivered_payloads");
    lua_pushnumber(L, stats.delivered_payloads);
    lua_settable(L, -3);

    lua_pushstring(L, "received_bundles");
    lua_pushnumber(L, stats.received_bundles);
    lua_settable(L, -3);

    lua_pushstring(L, "forwarded_bundles");
    lua_pushnumber(L, stats.forwarded_bundles);
    lua_settable(L, -3);

    lua_pushstring(L, "received_dacs");
    lua_pushnumber(L, stats.received_dacs);
    lua_settable(L, -3);

    lua_pushstring(L, "stored_bundles");
    lua_pushnumber(L, stats.stored_bundles);
    lua_settable(L, -3);

    lua_pushstring(L, "stored_payloads");
    lua_pushnumber(L, stats.stored_payloads);
    lua_settable(L, -3);

    lua_pushstring(L, "stored_dacs");
    lua_pushnumber(L, stats.stored_dacs);
    lua_settable(L, -3);

    lua_pushstring(L, "acknowledged_bundles");
    lua_pushnumber(L, stats.acknowledged_bundles);
    lua_settable(L, -3);

    lua_pushstring(L, "active_bundles");
    lua_pushnumber(L, stats.active_bundles);
    lua_settable(L, -3);

    return 2;
}

/*----------------------------------------------------------------------------
 * lbplib_store - channel:store(<data>, <timeout>) --> return code, flags
 *----------------------------------------------------------------------------*/
int lbplib_store (lua_State* L)
{
    /* Get User Data */
    lbplib_user_data_t* bplib_data = (lbplib_user_data_t*)luaL_checkudata(L, 1, LUA_BPLIBMETANAME);
    if(!bplib_data) 
    {
        lualog("unable to retrieve user data object: %s\n", LUA_BPLIBMETANAME);
        lua_pushboolean(L, false); /* push result as fail */
        return 1;
    }

    /* Check Number of Parameters */
    int minargs = 3;
    if(lua_gettop(L) != minargs)
    {
        lualog("incorrect number of parameters - expected %d\n", minargs);
        lua_pushboolean(L, false); /* push result as fail */
        return 1;
    }
    
    /* Type Check Parameters */
    if(!lua_isstring(L, 2) ||
       !lua_isnumber(L, 3))
    {
        lualog("incorrect parameter types\n");
        lua_pushboolean(L, false); /* push result as fail */
        return 1;
    }

    /* Store Data */
    uint16_t storflags = 0;
    size_t size = 0;
    const char* payload = lua_tolstring(L, 2, &size);
    int timeout = (int)lua_tonumber(L, 3);
    int status = bplib_store(bplib_data->channel, (void*)payload, (int)size, timeout, &storflags);
    set_errno(L, status);
    
    /* Return Status */
    lua_pushboolean(L, status == BP_SUCCESS);
    push_flag_table(L, storflags);
    return 2;    
}

/*----------------------------------------------------------------------------
 * lbplib_load - channel:load(<timeout>) --> return code, bundle, flags
 *----------------------------------------------------------------------------*/
int lbplib_load (lua_State* L)
{
    /* Get User Data */
    lbplib_user_data_t* bplib_data = (lbplib_user_data_t*)luaL_checkudata(L, 1, LUA_BPLIBMETANAME);
    if(!bplib_data) 
    {
        lualog("unable to retrieve user data object: %s\n", LUA_BPLIBMETANAME);
        lua_pushboolean(L, false); /* push result as fail */
        return 1;
    }

    /* Check Number of Parameters */
    int minargs = 2;
    if(lua_gettop(L) != minargs)
    {
        lualog("incorrect number of parameters - expected %d\n", minargs);
        lua_pushboolean(L, false); /* push result as fail */
        return 1;
    }
    
    /* Type Check Parameters */
    if(!lua_isnumber(L, 2))
    {
        lualog("incorrect parameter type\n");
        lua_pushboolean(L, false); /* push result as fail */
        return 1;
    }
    
    /* Load Bundle */
    uint16_t loadflags = 0;
    char* bundle = NULL;
    int size = 0;
    int timeout = (int)lua_tonumber(L, 2);
    int status = bplib_load(bplib_data->channel, (void**)&bundle, &size, timeout, &loadflags);
    set_errno(L, status);
    
    /* Return Status */
    lua_pushboolean(L, status == BP_SUCCESS);

    /* Return Bundle */
    if(status == BP_SUCCESS)
    {
        lua_pushlstring(L, bundle, size);        
        bplib_ackbundle(bplib_data->channel, bundle);
    }
    else
    {
        lua_pushnil(L);                
    }

    /* Return Flags */
    push_flag_table(L, loadflags);
    
    /* Return Number of Results */
    return 3;
}

/*----------------------------------------------------------------------------
 * lbplib_process - channel:process(<bundle>, <timeout>) --> return code, flags
 *----------------------------------------------------------------------------*/
int lbplib_process (lua_State* L)
{
    /* Get User Data */
    lbplib_user_data_t* bplib_data = (lbplib_user_data_t*)luaL_checkudata(L, 1, LUA_BPLIBMETANAME);
    if(!bplib_data) 
    {
        lualog("unable to retrieve user data object: %s\n", LUA_BPLIBMETANAME);
        lua_pushboolean(L, false); /* push result as fail */
        return 1;
    }

    /* Check Number of Parameters */
    int minargs = 3;
    if(lua_gettop(L) != minargs)
    {
        lualog("incorrect number of parameters - expected %d\n", minargs);
        lua_pushboolean(L, false); /* push result as fail */
        return 1;
    }
    
    /* Type Check Parameters */
    if(!lua_isstring(L, 2) ||   /* bundle */
       !lua_isnumber(L, 3))     /* timeout */
    {
        lualog("incorrect parameter types\n");
        lua_pushboolean(L, false); /* push result as fail */
        return 1;
    }

    /* Store Data */
    uint16_t procflags = 0;
    size_t size = 0;
    const char* bundle = lua_tolstring(L, 2, &size);
    int timeout = (int)lua_tonumber(L, 3);
    int status = bplib_process(bplib_data->channel, (void*)bundle, (int)size, timeout, &procflags);
    set_errno(L, status);

    /* Return Status */
    lua_pushboolean(L, status == BP_SUCCESS);
    push_flag_table(L, procflags);
    return 2;    
}

/*----------------------------------------------------------------------------
 * lbplib_accept - channel:accept(<timeout>) --> return code, data, flags
 *----------------------------------------------------------------------------*/
int lbplib_accept (lua_State* L)
{
    /* Get User Data */
    lbplib_user_data_t* bplib_data = (lbplib_user_data_t*)luaL_checkudata(L, 1, LUA_BPLIBMETANAME);
    if(!bplib_data) 
    {
        lualog("unable to retrieve user data object: %s\n", LUA_BPLIBMETANAME);
        lua_pushboolean(L, false); /* push result as fail */
        return 1;
    }

    /* Check Number of Parameters */
    int minargs = 2;
    if(lua_gettop(L) != minargs)
    {
        lualog("incorrect number of parameters - expected %d\n", minargs);
        lua_pushboolean(L, false); /* push result as fail */
        return 1;
    }
    
    /* Type Check Parameters */
    if(!lua_isnumber(L, 2))
    {
        lualog("incorrect parameter type\n");
        lua_pushboolean(L, false); /* push result as fail */
        return 1;
    }
    
    /* Accept Payload */
    uint16_t acptflags = 0;
    char* payload = NULL;
    int size = 0;
    int timeout = (int)lua_tonumber(L, 2);
    int status = bplib_accept(bplib_data->channel, (void**)&payload, &size, timeout, &acptflags);
    set_errno(L, status);

    /* Return Status */
    lua_pushboolean(L, status == BP_SUCCESS);

    /* Return Payload */
    if(status == BP_SUCCESS)
    {
        lua_pushlstring(L, payload, size);        
        bplib_ackpayload(bplib_data->channel, payload);
    }
    else
    {
        lua_pushnil(L);                
    }

    /* Return Flags */
    push_flag_table(L, acptflags);
        
    /* Return Number of Results */
    return 3;
}

/*----------------------------------------------------------------------------
 * lbplib_flush
 *----------------------------------------------------------------------------*/
int lbplib_flush (lua_State* L)
{
    /* Get User Data */
    lbplib_user_data_t* bplib_data = (lbplib_user_data_t*)luaL_checkudata(L, 1, LUA_BPLIBMETANAME);
    if(!bplib_data) 
    {
        lualog("unable to retrieve user data object: %s\n", LUA_BPLIBMETANAME);
    }
    else
    {
        /* Flush Channel */
        bplib_flush(bplib_data->channel);
    }

    return 0;
}
