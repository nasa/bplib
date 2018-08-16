/******************************************************************************
 * Filename:        bplib_os_posix.c
 * Purpose:         POSIX OS abstraction for Bundle Protocol Library
 * Design Notes:
 ******************************************************************************/

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include "bplib.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define MAX_LOG_ENTRY_SIZE  256

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_log - 
 * 
 * 	Returns - the error code passed in (for convenience)
 *-------------------------------------------------------------------------------------*/
int bplib_log(const char* file, unsigned int line, int error, const char* fmt, ...)
{
    char formatted_string[MAX_LOG_ENTRY_SIZE];
    va_list args;
	int vlen, msglen;

    /* Build Formatted String */
    va_start(args, fmt);
    vlen = vsnprintf(formatted_string, MAX_LOG_ENTRY_SIZE - 1, fmt, args);
    msglen = vlen < MAX_LOG_ENTRY_SIZE - 1 ? vlen : MAX_LOG_ENTRY_SIZE - 1;
    va_end(args);
    if (msglen < 0) return error; // nothing to do
    formatted_string[msglen] = '\0';

	/* Print Log Message */
    printf("%s:%d:%d:%s", file, line, error, formatted_string);

	/* Return Error Code */
	return error;
}   

/*--------------------------------------------------------------------------------------
 * bplib_memset - 
 *-------------------------------------------------------------------------------------*/
void bplib_os_memset(void* addr, int len, int val)
{
	memset(addr, len, val);
}

/*--------------------------------------------------------------------------------------
 * bplib_memcpy - 
 *-------------------------------------------------------------------------------------*/
void bplib_os_memcpy(void* dst, void* src, int len)
{
	memcpy(dst, src, len);
}

/*--------------------------------------------------------------------------------------
 * bplib_systime - 
 *-------------------------------------------------------------------------------------*/
int bplib_systime(bp_time_t* tm)
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    tm->s = now.tv_sec;
    tm->ns = now.tv_nsec;
    return BP_SUCCESS;
}
