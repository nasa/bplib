/************************************************************************
 * File: lua_bplib.h
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
#ifndef _lua_bplib_h_
#define _lua_bplib_h_

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <lua.h>

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

int luaopen_bplib(lua_State *L);

#endif /* _lua_bplib_h_ */
