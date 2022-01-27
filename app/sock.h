/************************************************************************
 * File: bpsock.h
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

#ifndef _bpsock_
#define _bpsock_

/******************************************************************************
 * Defines
 ******************************************************************************/

#define SOCK_PEND    (-1)
#define SOCK_CHECK   0
#define SOCK_TIMEOUT 1000 // milliseconds
#define SOCK_INVALID (-1)

/******************************************************************************
 * Exported Functions
 ******************************************************************************/

int  sockcreate(int type, const char *ip_addr, int port, int is_server, int *block);
int  sockstream(const char *ip_addr, int port, int is_server, int *block);
int  sockdatagram(const char *ip_addr, int port, int is_server, int *block);
int  socksend(int fd, const void *buf, int size, int timeout);
int  sockrecv(int fd, void *buf, int size, int timeout);
void sockclose(int fd);

#endif /* _bpsock_ */
