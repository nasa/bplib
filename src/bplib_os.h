/******************************************************************************
 * Filename:        bplib_os.h
 * Purpose:         OS abstraction for Bundle Protocol Library
 * Design Notes:
 ******************************************************************************/

#ifndef _BPLIB_OS_H_
#define _BPLIB_OS_H_

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdint.h>
#include "bplib.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

/* Logging */
#ifdef _GNU_
#define VARG_CHECK(f, a, b) __attribute__((format(f, a, b)))
#else
#define VARG_CHECK(f, a, b)
#endif
#define bplog(err,...)  bplib_log(__FILE__,__LINE__,err,__VA_ARGS__)
#define BP_INFO         0

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int     bplib_log       (const char* file, unsigned int line, int error, const char* fmt, ...) VARG_CHECK(printf, 4, 5);
void    bplib_os_memset (void* addr, int len, int val);
void    bplib_os_memcpy (void* dst, void* src, int len);
int     bplib_systime   (bp_time_t* tm);

#endif /* _BPLIB_CRC_H_ */
