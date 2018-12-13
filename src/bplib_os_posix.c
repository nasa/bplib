/******************************************************************************
 * Filename:        bplib_os_posix.c
 * Purpose:         POSIX OS abstraction for Bundle Protocol Library
 * Design Notes:
 ******************************************************************************/

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include "bplib.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define MAX_LOG_ENTRY_SIZE  256
#define UNIX_SECS_AT_2000   946684800
#define MAX_MUTEXES         32

/******************************************************************************
 FILE DATA
 ******************************************************************************/

pthread_mutex_t* locks[MAX_MUTEXES] = {0};
pthread_mutex_t lock_of_locks;

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_os_init -
 *-------------------------------------------------------------------------------------*/
void bplib_os_init(void)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&lock_of_locks, &attr);
}

/*--------------------------------------------------------------------------------------
 * bplib_os_log -
 *
 * 	Returns - the error code passed in (for convenience)
 *-------------------------------------------------------------------------------------*/
int bplib_os_log(const char* file, unsigned int line, int error, const char* fmt, ...)
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
 * bplib_os_memset -
 *-------------------------------------------------------------------------------------*/
void bplib_os_memset(void* addr, int val, int len)
{
	memset(addr, val, len);
}

/*--------------------------------------------------------------------------------------
 * bplib_os_memcpy -
 *-------------------------------------------------------------------------------------*/
void bplib_os_memcpy(void* dst, void* src, int len)
{
	memcpy(dst, src, len);
}

/*--------------------------------------------------------------------------------------
 * bplib_os_strnlen -
 *-------------------------------------------------------------------------------------*/
size_t bplib_os_strnlen(const char *str, size_t maxlen)
{
    size_t len;
    for(len = 0; len < maxlen; len++)
    {
        if(str[len] == '\0')
        {
            return len;
        }
    }
    return maxlen;
}

/*--------------------------------------------------------------------------------------
 * bplib_os_systime -
 *-------------------------------------------------------------------------------------*/
uint32_t bplib_os_systime(void)
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now.tv_sec - UNIX_SECS_AT_2000;
}

/*--------------------------------------------------------------------------------------
 * bplib_os_createlock -
 *-------------------------------------------------------------------------------------*/
int bplib_os_createlock(void)
{
    int i;
    int handle = -1;
    pthread_mutex_lock(&lock_of_locks);
    {
        for(i = 0; i < MAX_MUTEXES; i++)
        {
            if(locks[i] == NULL)
            {
                locks[i] = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
                pthread_mutexattr_t attr;
                pthread_mutexattr_init(&attr);
                pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
                pthread_mutex_init(locks[i], &attr);
                handle = i;
                break;
            }
        }
    }
    pthread_mutex_unlock(&lock_of_locks);
    return handle;
}

/*--------------------------------------------------------------------------------------
 * bplib_os_destroylock -
 *-------------------------------------------------------------------------------------*/
void bplib_os_destroylock(int handle)
{
    pthread_mutex_lock(&lock_of_locks);
    {
        if(locks[handle] != NULL)
        {
            free(locks[handle]);
            locks[handle] = NULL;
        }
    }
    pthread_mutex_unlock(&lock_of_locks);
}

/*--------------------------------------------------------------------------------------
 * bplib_os_lock -
 *-------------------------------------------------------------------------------------*/
void bplib_os_lock(int handle)
{
    pthread_mutex_lock(locks[handle]);
}

/*--------------------------------------------------------------------------------------
 * bplib_os_unlock -
 *-------------------------------------------------------------------------------------*/
void bplib_os_unlock(int handle)
{
    pthread_mutex_unlock(locks[handle]);
}
