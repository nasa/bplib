/************************************************************************
 * File: posix.c
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

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

#include "bplib.h"
#include "bplib_os.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define UNIX_SECS_AT_2000       946684800
#define BP_MAX_LOG_ENTRY_SIZE   256
#define BP_MAX_LOCKS            128

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    pthread_cond_t  cond;
    pthread_mutex_t mutex;
} bplib_os_lock_t;

/******************************************************************************
 FILE DATA
 ******************************************************************************/

bplib_os_lock_t*    locks[BP_MAX_LOCKS] = {0};
pthread_mutex_t     lock_of_locks;

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_os_init -
 *-------------------------------------------------------------------------------------*/
void bplib_os_init()
{
    pthread_mutexattr_t locks_attr;
    pthread_mutexattr_init(&locks_attr);
    pthread_mutexattr_settype(&locks_attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&lock_of_locks, &locks_attr);
}

/*--------------------------------------------------------------------------------------
 * bplib_os_log -
 *
 * 	Returns - the error code passed in (for convenience)
 *-------------------------------------------------------------------------------------*/
int bplib_os_log(const char* file, unsigned int line, int error, const char* fmt, ...)
{
    char formatted_string[BP_MAX_LOG_ENTRY_SIZE];
    char log_message[BP_MAX_LOG_ENTRY_SIZE];
    va_list args;
    int vlen, msglen;
    char* pathptr;

    /* Build Formatted String */
    va_start(args, fmt);
    vlen = vsnprintf(formatted_string, BP_MAX_LOG_ENTRY_SIZE - 1, fmt, args);
    msglen = vlen < BP_MAX_LOG_ENTRY_SIZE - 1 ? vlen : BP_MAX_LOG_ENTRY_SIZE - 1;
    va_end(args);
    if (msglen < 0) return error; /* nothing to do */
    formatted_string[msglen] = '\0';

    /* Chop Path in Filename */
    pathptr = strrchr(file, '/');
    if(pathptr) pathptr++;
    else pathptr = (char*)file;

    /* Create Log Message */
    snprintf(log_message, BP_MAX_LOG_ENTRY_SIZE, "%s:%d:%d:%s", pathptr, line, error, formatted_string);
    
    /* Display Log Message */
    printf("%s", log_message);

    /* Return Error Code */
    return error;
}

/*--------------------------------------------------------------------------------------
 * bplib_os_systime - returns seconds
 *-------------------------------------------------------------------------------------*/
unsigned long bplib_os_systime(void)
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now.tv_sec - UNIX_SECS_AT_2000;
}

/*--------------------------------------------------------------------------------------
 * bplib_os_sleep
 *-------------------------------------------------------------------------------------*/
void bplib_os_sleep(int seconds)
{
    sleep(seconds);
}

/*--------------------------------------------------------------------------------------
 * bplib_os_createlock -
 *-------------------------------------------------------------------------------------*/
int bplib_os_createlock(void)
{
    int i;
    int handle = BP_INVALID_HANDLE;
    
    pthread_mutex_lock(&lock_of_locks);
    {
        for(i = 0; i < BP_MAX_LOCKS; i++)
        {
            if(locks[i] == NULL)
            {
                locks[i] = (bplib_os_lock_t*)malloc(sizeof(bplib_os_lock_t));
                pthread_mutexattr_t attr;
                pthread_mutexattr_init(&attr);
                pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
                pthread_mutex_init(&locks[i]->mutex, &attr);
                pthread_cond_init(&locks[i]->cond, NULL);
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
            pthread_mutex_destroy(&locks[handle]->mutex); 
            pthread_cond_destroy(&locks[handle]->cond);
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
    pthread_mutex_lock(&locks[handle]->mutex);
}

/*--------------------------------------------------------------------------------------
 * bplib_os_unlock -
 *-------------------------------------------------------------------------------------*/
void bplib_os_unlock(int handle)
{
    pthread_mutex_unlock(&locks[handle]->mutex);
}

/*--------------------------------------------------------------------------------------
 * bplib_os_signal -
 *-------------------------------------------------------------------------------------*/
void bplib_os_signal(int handle)
{
    pthread_cond_signal(&locks[handle]->cond);
}

/*--------------------------------------------------------------------------------------
 * bplib_os_waiton -
 *-------------------------------------------------------------------------------------*/
int bplib_os_waiton(int handle, int timeout_ms)
{
    int status;

    /* Perform Wait */
    if(timeout_ms == -1)
    {
        /* Block Forever until Success */
        status = pthread_cond_wait(&locks[handle]->cond, &locks[handle]->mutex);
        if(status != 0) status = BP_OS_ERROR;
        else            status = BP_SUCCESS;
    }
    else if(timeout_ms > 0)
    {
        /* Build Time Structure */
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec  += (time_t) (timeout_ms / 1000);
        ts.tv_nsec +=  (timeout_ms % 1000) * 1000000L;
        if(ts.tv_nsec  >= 1000000000L)
        {
            ts.tv_nsec -= 1000000000L;
            ts.tv_sec++;
        }

        /* Block on Timed Wait and Update Timeout */
        status = pthread_cond_timedwait(&locks[handle]->cond, &locks[handle]->mutex, &ts);
        if(status == ETIMEDOUT) status = BP_OS_TIMEOUT;
        else                    status = BP_SUCCESS;
    }
    else /* timeout_ms = 0 */
    {
        /* conditional does not support a non-blocking attempt
         * so treat it as an immediate timeout */
        status = BP_OS_TIMEOUT;
    }

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_os_format -
 *-------------------------------------------------------------------------------------*/
int bplib_os_format(char* dst, size_t len, const char* fmt, ...)
{
    va_list args;
    int vlen;

    /* Build Formatted String */
    va_start(args, fmt);
    vlen = vsnprintf(dst, len, fmt, args);
    va_end(args);

    /* Return Error Code */
    return vlen;
}

/*--------------------------------------------------------------------------------------
 * bplib_os_strnlen -
 *-------------------------------------------------------------------------------------*/
int bplib_os_strnlen(const char* str, int maxlen)
{
    int len;
    for(len = 0; len < maxlen; len++)
    {
        if(str[len] == '\0')
        {
            return len;
        }
    }
    return maxlen;
}
