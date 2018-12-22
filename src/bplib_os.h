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

/******************************************************************************
 DEFINES
 ******************************************************************************/

/* Logging */
#ifdef _GNU_
#define VARG_CHECK(f, a, b) __attribute__((format(f, a, b)))
#else
#define VARG_CHECK(f, a, b)
#endif
#define bplog(err,...)  bplib_os_log(__FILE__,__LINE__,err,__VA_ARGS__)

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

void        bplib_os_init           (void);
int         bplib_os_log            (const char* file, unsigned int line, int error, const char* fmt, ...) VARG_CHECK(printf, 4, 5);
void        bplib_os_memset         (void* addr, int val, int len);
void        bplib_os_memcpy         (void* dst, void* src, int len);
size_t      bplib_os_strnlen        (const char *str, size_t maxlen);
uint32_t    bplib_os_systime        (void);
int         bplib_os_createlock     (void);
void        bplib_os_destroylock    (int handle);
void        bplib_os_lock           (int handle);
void        bplib_os_unlock         (int handle);

#endif /* _BPLIB_OS_H_ */
