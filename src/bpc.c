/******************************************************************************
 Filename     : bpc.c
 Purpose      : Bundle Protocol Console
 Design Notes : Targets Linux OS exclusively
 ******************************************************************************/

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <dlfcn.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>

#include "bp/bplib.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define CLI_BUF_SIZE 1024

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/* 
 * __stack_chk_fail - called with -fstack-protector-all
 */
void __stack_chk_fail(void)
{
    assert(0);
}

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

/*
 * console_quick_exit - Signal handler for Control-C
 */
void console_quick_exit(int parm)
{
    (void)parm;
    printf("\n...Shutting down command line!\n");
    exit(0);
}

/*
 * clithread - command line
 */
void* clithread (void* parm)
{
    (void)parm;
    
    char buf[CLI_BUF_SIZE];
    
    fputs("> ", stdout);
    fflush(stdout);

    while(1)
    {
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        fd_set fdset;
        FD_ZERO(&fdset);
        FD_SET(fileno(stdin), &fdset);

        select(1, &fdset, NULL, NULL, &tv);
        if(FD_ISSET(fileno(stdin), &fdset))    
        {
            fputs("> ", stdout);
            fflush(stdout);
            if(fgets(&buf[0], CLI_BUF_SIZE, stdin))
            {
                if(buf[0] == 'q')
                {
                    exit(0);
                }
            }
        }
    }
    
    return NULL;
}


/******************************************************************************
 MAIN
 ******************************************************************************/

int main (int argc, char* argv[])
{            
    (void)argc;
    (void)argv;

    pthread_t thread_id;
    pthread_attr_t pthread_attr;

    /* Reassign the Signal Handler */
    signal(SIGINT, console_quick_exit);

    /* Create Command Line Interface Thread */
    pthread_attr_init(&pthread_attr);
    pthread_attr_setdetachstate(&pthread_attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&thread_id, &pthread_attr, clithread, NULL);

    /* Loop Forever */
    while(1) sleep(1);
    return 0;
}
