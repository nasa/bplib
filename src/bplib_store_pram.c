/******************************************************************************
 Filename     : bplib_store_pram.c
 Purpose      : thread level message queues
*******************************************************************************/

/******************************************************************************
 * INCLUDES
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <limits.h>
#include <unistd.h>
#include <assert.h>

#include "bplib.h"

/******************************************************************************
 * DEFINES
 ******************************************************************************/

#define MSGQ_OKAY           (1)
#define MSGQ_TIMEOUT        (0)
#define MSGQ_ERROR          (-1)
#define MSGQ_FULL           (-2)
#define MSGQ_MEMORY_ERROR   (-3)
#define MSGQ_UNDERFLOW		(-4)
#define MSGQ_INVALID_HANDLE ((msgq_t)NULL)
#define MSGQ_MAX_NAME_CHARS 64
#define MSGQ_DEPTH_INFINITY 0
#define MSGQ_SIZE_INFINITY  0
#define MSGQ_STORE_STR      "bplibq"
#define MSGQ_STORE_STR_SIZE 32
#define MSGQ_MAX_STORES     64

/******************************************************************************
 * TYPEDEFS
 ******************************************************************************/

/* queue_node_t */
typedef struct queue_block_t {
  void*                   data;
  unsigned int            size;
  struct queue_block_t*   next;
} queue_node_t;

/* queue_t */
typedef struct queue_def_t {
  queue_node_t*   front;
  queue_node_t*   rear;
  unsigned int    depth;
  unsigned int    len;
  unsigned int    max_data_size;
} queue_t;

/* message_queue_t */
typedef struct {
  char            name[MSGQ_MAX_NAME_CHARS];
  queue_t*        queue;
  pthread_mutex_t mut;
  sem_t           ready;
  int             state;
} message_queue_t;

/* message queue handle */
typedef void* msgq_t;

/******************************************************************************
 * FILE DATA
 ******************************************************************************/

static msgq_t msgq_stores[MSGQ_MAX_STORES];
static unsigned long store_id = 0;

/******************************************************************************
 * LOCAL FUNCTIONS
 ******************************************************************************/

/*----------------------------------------------------------------------------
 * Function:        delay_time
 *----------------------------------------------------------------------------*/
static void delay_time(unsigned long msecs, struct timespec* tm)
{
    clock_gettime( CLOCK_REALTIME,  tm );
    tm->tv_sec  += (time_t) (msecs / 1000) ;
    tm->tv_nsec +=  (msecs % 1000) * 1000000L ;

    if(tm->tv_nsec  >= 1000000000L )
    {
        tm->tv_nsec -= 1000000000L ;
        tm->tv_sec ++ ;
    }
}

/*----------------------------------------------------------------------------
 * Function:        create_queue
 *----------------------------------------------------------------------------*/
static queue_t* create_queue(int depth, int data_size)
{
    queue_t* msgQ = (queue_t*)malloc(sizeof(queue_t));

    msgQ->front         = NULL;
    msgQ->rear          = NULL;
    msgQ->depth         = depth;
    msgQ->len           = 0;
    msgQ->max_data_size = data_size;

    return msgQ;
}

/*----------------------------------------------------------------------------
 * Function:        destroy_queue
 *----------------------------------------------------------------------------*/
static void destroy_queue(queue_t* q)
{
    queue_node_t* temp;

    while(q->front != NULL)
    {
        temp = q->front->next;
        free(q->front);
        q->front = temp;
    }
    q->rear = NULL;
}

/*----------------------------------------------------------------------------
 * Function:        isempty
 *----------------------------------------------------------------------------*/
static int isempty(queue_t* q)
{
    if(q->front == NULL)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*----------------------------------------------------------------------------
 * Function:        enqueue
 *----------------------------------------------------------------------------*/
static int enqueue(queue_t* q, void* data, int size)
{
    queue_node_t* temp;

    /* check if queue is full */
    if((q->depth != MSGQ_DEPTH_INFINITY) && (q->len >= q->depth))
    {
        return MSGQ_FULL;
    }

    /* check size */
    if((size <= 0) ||
       ((q->max_data_size != MSGQ_SIZE_INFINITY) &&
        ((unsigned)size > q->max_data_size)))
    {
        return MSGQ_ERROR;
    }

    /* create temp node */
    temp = (queue_node_t*)malloc((int)sizeof(queue_node_t));
    if(!temp) return MSGQ_MEMORY_ERROR;

    /* construct node to be added */
    temp->data      = data;
    temp->size      = size;
    temp->next      = NULL;

    /* place temp node into queue */
    if(q->rear == NULL)
    {
        q->rear = temp;
        q->front = temp;
    }
    else if(q->rear != NULL)
    {
        q->rear->next = temp;
        q->rear = q->rear->next;
    }

    q->len++;

    return MSGQ_OKAY;
}

/*----------------------------------------------------------------------------
 * Function:        dequeue
 *----------------------------------------------------------------------------*/
static void* dequeue(queue_t* q, int* size)
{
    queue_node_t* tmp;
    void *data;

    if(q->front != NULL)
    {
        /* extract */
        data = q->front->data;
        if(size) *size = q->front->size;

        /* remove */
        tmp = q->front;
        if(q->front == q->rear)
        {
            q->front = q->rear = NULL;
        }
        else
        {
            q->front = q->front->next;
        }
        free(tmp);

        q->len--;
    }
    else
    {
        if(size) *size = 0;
        data = NULL;
    }

    return data;
}

/*----------------------------------------------------------------------------
 * Function:        msgq_create
 *
 * Notes: 1. Returns a handle to the message queue created
 *        2. The depth specifies the maximum number of items that are
 *           allowed to be queued up.  If the depth is zero, the queue
 *           is allowed to infinitely grow until all the memory in the
 *           system is consumed.
 *----------------------------------------------------------------------------*/
static msgq_t msgq_create(const char* name, int depth, int data_size)
{
    message_queue_t* msgQ;

    /* Check Parameters */
    if(name == NULL)
    {
        return MSGQ_INVALID_HANDLE;
    }

    /* Allocate MSG Q */
    msgQ = (message_queue_t*)malloc(sizeof(message_queue_t));

    /* Initialize MSG Q */
    strncpy(msgQ->name, name, MSGQ_MAX_NAME_CHARS);
    msgQ->queue = create_queue(depth, data_size);
    msgQ->state = MSGQ_OKAY;

    /* Create Semaphore */
    int mutstatus = pthread_mutex_init(&(msgQ->mut), NULL);
    if(mutstatus == -1)
    {
        printf("ERROR[%d]: Unable to create mutex: %s\n",
                mutstatus, name);
        free(msgQ);
        msgQ = NULL;
    }
    else
    {
        /* Create Semaphore */
        int rdystatus = sem_init(&(msgQ->ready), 0, 0);
        if(rdystatus == -1)
        {
            printf("ERROR[%d]: Unable to create ready sem: %s\n",
                    rdystatus, name);
            free(msgQ);
            msgQ = NULL;
        }
    }

    /* Check Final Status */
    if( (msgQ != NULL) && (msgQ->state != MSGQ_OKAY) )
    {
        printf("ERROR, Unable to register: %s\n",
                name);
        free(msgQ);
        msgQ = NULL;
    }

    return (msgq_t)msgQ;
}

/*----------------------------------------------------------------------------
 * Function:        msgq_delete
 *
 * Notes: 1. deallocates memory assocaited with message queue
 *        2. removes queue from system list
 *----------------------------------------------------------------------------*/
static void msgq_delete(msgq_t queue_handle)
{
    message_queue_t* msgQ = (message_queue_t*)queue_handle;
    if(msgQ != NULL)
    {
        destroy_queue(msgQ->queue);
        free(msgQ);
    }
}

/*----------------------------------------------------------------------------
 * Function:        msgq_post
 *----------------------------------------------------------------------------*/
static int msgq_post(msgq_t queue_handle, void* data, int size)
{
    int post_state;

    message_queue_t* msgQ = (message_queue_t*)queue_handle;
    if(msgQ == NULL) return MSGQ_ERROR;

    /* Post Data */
    pthread_mutex_lock(&(msgQ->mut));
    {
        post_state = enqueue(msgQ->queue, data, size);

#ifdef MSGQ_VERBOSE
        /* Print Error */
        if(msgQ->state == MSGQ_OKAY && post_state != MSGQ_OKAY)
        {
            printf("ERROR: %s, msgq post failed (state:%d sz:%d) \n",
                    msgQ->name, msgQ->state, size);
        }
#endif

        msgQ->state = post_state;
    }
    pthread_mutex_unlock(&(msgQ->mut));

    /* Trigger if Ready */
    if(post_state == MSGQ_OKAY)
    {
        sem_post(&(msgQ->ready));
    }

    /* Return Status */
    return post_state;
}

/*----------------------------------------------------------------------------
 * Function:        msgq_receive
 *
 * Notes:           returns a pointer to the data and the size of the data by
 *                  populating the size parameter passed in by pointer
 *----------------------------------------------------------------------------*/
static int msgq_receive(msgq_t queue_handle, void** data, int* size, int block)
{
    message_queue_t* msgQ = (message_queue_t*)queue_handle;
    if(msgQ == NULL) return MSGQ_ERROR;

	int recv_state = MSGQ_OKAY;

    /* Wait for a message to be posted */
    if(block == BP_PEND)
    {
        int ret = -1;
        while(isempty(msgQ->queue) || (ret == -1 && errno == EINTR))
        {
            ret = sem_wait(&(msgQ->ready));
        }
    }
    else if(block == BP_CHECK)
    {
    }
    else /* Timed Wait */
    {
        struct timespec  ts;
        int waitstatus = 1;
        delay_time(block, &ts);

        while((waitstatus != -1 && isempty(msgQ->queue)) ||
              (waitstatus == -1 && errno == EINTR))
        {
            waitstatus = sem_timedwait(&(msgQ->ready), &ts);
        }

        if(waitstatus == -1 && errno == ETIMEDOUT)
        {
            recv_state = MSGQ_TIMEOUT;
        }
        else if(waitstatus == -1)
        {
            recv_state = MSGQ_ERROR;
        }
    }

    /* Get data from queue */
    pthread_mutex_lock(&(msgQ->mut));
    {
#ifdef MSGQ_VERBOSE
        /* Print Error */
        if(msgQ->state == MSGQ_OKAY && recv_state != MSGQ_OKAY)
        {
            printf("ERROR: %s, receive failed (%d)\n", msgQ->name, recv_state);
        }
#endif

        msgQ->state = recv_state;
        if(msgQ->state == MSGQ_OKAY)
        {
            *data = dequeue(msgQ->queue, size);
            if(*data == NULL) recv_state = MSGQ_UNDERFLOW;
        }
    }
    pthread_mutex_unlock(&(msgQ->mut));

    /* Return Status */
    return recv_state;
}

/******************************************************************************
 * EXPORTED FUNCTIONS
 ******************************************************************************/

/*----------------------------------------------------------------------------
 * bplib_store_pram_create -
 *----------------------------------------------------------------------------*/
int bplib_store_pram_create (void)
{
    int slot, i;

    /* Build Queue Name */
    char qname[MSGQ_STORE_STR_SIZE];
    snprintf(qname, MSGQ_STORE_STR_SIZE, "%s%ld", MSGQ_STORE_STR, store_id++);

    /* Look for Empty Slots */
    slot = BP_INVALID_HANDLE;
    for(i = 0; i < MSGQ_MAX_STORES; i++)
    {
        if(msgq_stores[i] == MSGQ_INVALID_HANDLE)
        {
            msgq_stores[i] = msgq_create(qname,
                                         MSGQ_DEPTH_INFINITY,
                                         MSGQ_SIZE_INFINITY);
            slot = i;
            break;
        }
    }

    /* Return Index into List */
    return slot;
}

/*----------------------------------------------------------------------------
 * bplib_store_pram_destroy -
 *----------------------------------------------------------------------------*/
int bplib_store_pram_destroy (int handle)
{
    assert(handle >= 0 && handle < MSGQ_MAX_STORES);
    assert(msgq_stores[handle] != MSGQ_INVALID_HANDLE);

    msgq_delete(msgq_stores[handle]);
    msgq_stores[handle] = MSGQ_INVALID_HANDLE;

    return BP_SUCCESS;
}

/*----------------------------------------------------------------------------
 * bplib_store_pram_enqueue -
 *----------------------------------------------------------------------------*/
int bplib_store_pram_enqueue(int handle, void* data1, int data1_size,
                             void* data2, int data2_size, int timeout)
{
    assert(handle >= 0 && handle < MSGQ_MAX_STORES);
    assert(msgq_stores[handle]);
    assert((data1_size >= 0) && (data2_size >= 0));
    assert((data1_size + data2_size) > 0);

    int status;
    int data_size = data1_size + data2_size;
    unsigned char* data = (unsigned char*)malloc(data_size);

    /* Check memory allocation */
    if(!data) return BP_FAILEDSTORE;

    /* Copy data segments */
    memcpy(data, data1, data1_size);
    memcpy(&data[data1_size], data2, data2_size);

    /* Post data */
    status = msgq_post(msgq_stores[handle], data, data_size);

    /* Return status */
    if(status > 0)
    {
        return status;
    }
    else if(status == MSGQ_FULL)
    {
        free(data);
        usleep(timeout * 1000);
        return BP_TIMEOUT;
    }
    else
    {
        free(data);
        return BP_FAILEDSTORE;
    }
}

/*----------------------------------------------------------------------------
 * bplib_store_pram_dequeue -
 *----------------------------------------------------------------------------*/
int bplib_store_pram_dequeue(int handle, void** data, int* size,
                             bp_sid_t* sid, int timeout)
{
    assert(sid);
    assert(handle >= 0 && handle < MSGQ_MAX_STORES);
    assert(msgq_stores[handle]);

    int status = msgq_receive(msgq_stores[handle], data, size, timeout);
    if(status == MSGQ_OKAY)
    {
        *sid = *data;
        return BP_SUCCESS;
    }
    else if(status == MSGQ_TIMEOUT)
    {
        return BP_TIMEOUT;
    }
    else
    {
        return BP_FAILEDSTORE;
    }
}

/*----------------------------------------------------------------------------
 * bplib_store_pram_retrieve -
 *----------------------------------------------------------------------------*/
int bplib_store_pram_retrieve(int handle, void** data, int* size,
                              bp_sid_t sid, int timeout)
{
    (void)timeout;

    assert(data);
    assert(handle >= 0 && handle < MSGQ_MAX_STORES);
    assert(msgq_stores[handle]);

    if(data) *data = sid;
    if(size) *size = 0; // unsupported

    return BP_SUCCESS;
}

/*----------------------------------------------------------------------------
 * bplib_store_pram_refresh -
 *----------------------------------------------------------------------------*/
int bplib_store_pram_refresh(int handle, void* data, int size, int offset,
                             bp_sid_t sid, int timeout)
{
   (void)timeout;

    assert(data);
    assert(handle >= 0 && handle < MSGQ_MAX_STORES);
    assert(msgq_stores[handle]);

    uint8_t* app_data = (uint8_t*)data;
    uint8_t* store_data = (uint8_t*)sid;

    memmove(&store_data[offset], app_data, size);
    return BP_SUCCESS;
}

/*----------------------------------------------------------------------------
 * bplib_store_pram_relinquish -
 *----------------------------------------------------------------------------*/
int bplib_store_pram_relinquish (int handle, bp_sid_t sid)
{
    assert(handle >= 0 && handle < MSGQ_MAX_STORES);
    assert(msgq_stores[handle]);

    void* data = (void*)sid;
    free(data);

    return BP_SUCCESS;
}
