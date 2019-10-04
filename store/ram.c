/************************************************************************
 * File: ram.c
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
 * INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bplib_store_ram.h"

/******************************************************************************
 * DEFINES
 ******************************************************************************/

#define MSGQ_OKAY               (1)
#define MSGQ_TIMEOUT            (0)
#define MSGQ_ERROR              (-1)
#define MSGQ_FULL               (-2)
#define MSGQ_MEMORY_ERROR       (-3)
#define MSGQ_UNDERFLOW          (-4)
#define MSGQ_INVALID_HANDLE     ((msgq_t)NULL)
#define MSGQ_MAX_NAME_CHARS     64
#define MSGQ_DEPTH_INFINITY     0
#define MSGQ_SIZE_INFINITY      0
#define MSGQ_STORE_STR          "bplibq"
#define MSGQ_STORE_STR_SIZE     32

/* Configurable Options */

#ifndef MSGQ_MAX_STORES
#define MSGQ_MAX_STORES         60
#endif

#ifndef MSGQ_MAX_DEPTH
#define MSGQ_MAX_DEPTH          65536
#endif

#ifndef MSGQ_MAX_SIZE
#define MSGQ_MAX_SIZE           MSGQ_SIZE_INFINITY
#endif

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
    queue_node_t*           front;  /* oldest node removed (read pointer) */
    queue_node_t*           rear;   /* newest node added (write pointer) */
    unsigned int            depth;  /* maximum length of linked list */
    unsigned int            len;    /* current length of linked list */
    unsigned int            max_data_size;
} queue_t;

/* message_queue_t */
typedef struct {
    char                    name[MSGQ_MAX_NAME_CHARS];
    queue_t                 queue;
    int                     ready;
    int                     state;
} message_queue_t;

/* message queue handle */
typedef void* msgq_t;

/******************************************************************************
 * FILE DATA
 ******************************************************************************/

static msgq_t msgq_stores[MSGQ_MAX_STORES];
static int msgq_counts[MSGQ_MAX_STORES];
static unsigned long store_id;

/******************************************************************************
 * LOCAL QUEUE FUNCTIONS
 ******************************************************************************/

/*----------------------------------------------------------------------------
 * Function:        flush_queue
 *----------------------------------------------------------------------------*/
static void flush_queue(queue_t* q)
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

/******************************************************************************
 * LOCAL MSGQ FUNCTIONS
 ******************************************************************************/

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
    int ready_lock;

    /* Check Parameters */
    if(name == NULL)
    {
        return MSGQ_INVALID_HANDLE;
    }

    /* Create Lock */
    ready_lock = bplib_os_createlock();
    if(ready_lock == -1)
    {
        printf("ERROR(%d): Unable to create ready sem: %s\n", ready_lock, name);
        return MSGQ_INVALID_HANDLE;
    }

    /* Allocate MSG Q */
    msgQ = (message_queue_t*)malloc(sizeof(message_queue_t));
    if(msgQ == NULL)
    {
        printf("ERROR, Unable to allocate message queue: %s\n", name);
        return MSGQ_INVALID_HANDLE;
    }

    /* Initialize MSG Q */
    msgQ->state = MSGQ_OKAY;
    strncpy(msgQ->name, name, MSGQ_MAX_NAME_CHARS);
    msgQ->queue.front         = NULL;
    msgQ->queue.rear          = NULL;
    msgQ->queue.depth         = depth;
    msgQ->queue.len           = 0;
    msgQ->queue.max_data_size = data_size;
    msgQ->ready = ready_lock;

    /* Return MSG Q */
    return (msgq_t)msgQ;
}

/*----------------------------------------------------------------------------
 * Function:        msgq_delete
 *
 * Notes: 1. de-allocates memory associated with message queue
 *        2. removes queue from system list
 *----------------------------------------------------------------------------*/
static void msgq_delete(msgq_t queue_handle)
{
    message_queue_t* msgQ = (message_queue_t*)queue_handle;
    if(msgQ != NULL)
    {
        flush_queue(&msgQ->queue);
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
    bplib_os_lock(msgQ->ready);
    {
        post_state = enqueue(&msgQ->queue, data, size);
        msgQ->state = post_state;
    }
    bplib_os_unlock(msgQ->ready);

    /* Trigger if Ready */
    if(post_state == MSGQ_OKAY)
    {
        bplib_os_signal(msgQ->ready);
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

    bplib_os_lock(msgQ->ready);
    {
        /* Wait for a message to be posted */
        if(block == BP_PEND)
        {
            while(isempty(&msgQ->queue))
            {
                bplib_os_waiton(msgQ->ready, BP_PEND);
            }
        }
        else if(block == BP_CHECK)
        {
        }
        else /* Timed Wait */
        {
            if(isempty(&msgQ->queue))
            {
                int wait_status = bplib_os_waiton(msgQ->ready, block);
                if(wait_status == BP_OS_TIMEOUT) recv_state = MSGQ_TIMEOUT;
                else if(wait_status == BP_OS_ERROR) recv_state = MSGQ_ERROR;
            }
        }

        /* Get data from queue */
        msgQ->state = recv_state;
        if(msgQ->state == MSGQ_OKAY)
        {
            *data = dequeue(&msgQ->queue, size);
            if(*data == NULL) recv_state = MSGQ_UNDERFLOW;
        }
    }
    bplib_os_unlock(msgQ->ready);

    /* Return Status */
    return recv_state;
}

/******************************************************************************
 * EXPORTED FUNCTIONS
 ******************************************************************************/

/*----------------------------------------------------------------------------
 * bplib_store_ram_init -
 *----------------------------------------------------------------------------*/
void bplib_store_ram_init (void)
{
    memset(msgq_stores, 0, sizeof(msgq_stores));
    memset(msgq_counts, 0, sizeof(msgq_counts));
    store_id = 0;
}

/*----------------------------------------------------------------------------
 * bplib_store_ram_create -
 *----------------------------------------------------------------------------*/
int bplib_store_ram_create (void* parm)
{
    (void)parm;
    
    int slot, i;

    /* Build Queue Name */
    char qname[MSGQ_STORE_STR_SIZE];
    bplib_os_format(qname, MSGQ_STORE_STR_SIZE, "%s%ld", MSGQ_STORE_STR, store_id++);

    /* Look for Empty Slots */
    slot = BP_INVALID_HANDLE;
    for(i = 0; i < MSGQ_MAX_STORES; i++)
    {
        if(msgq_stores[i] == MSGQ_INVALID_HANDLE)
        {
            msgq_t msgq = msgq_create(qname, MSGQ_MAX_DEPTH, MSGQ_MAX_SIZE);
            if(msgq != MSGQ_INVALID_HANDLE)
            {
                msgq_stores[i] = msgq;
                msgq_counts[i] = 0;
                slot = i;
            }
            break;
        }
    }

    /* Return Index into List */
    return slot;
}

/*----------------------------------------------------------------------------
 * bplib_store_ram_destroy -
 *----------------------------------------------------------------------------*/
int bplib_store_ram_destroy (int handle)
{
    assert(handle >= 0 && handle < MSGQ_MAX_STORES);
    assert(msgq_stores[handle] != MSGQ_INVALID_HANDLE);

    msgq_delete(msgq_stores[handle]);
    msgq_stores[handle] = MSGQ_INVALID_HANDLE;
    msgq_counts[handle] = 0;

    return BP_SUCCESS;
}

/*----------------------------------------------------------------------------
 * bplib_store_ram_enqueue -
 *----------------------------------------------------------------------------*/
int bplib_store_ram_enqueue(int handle, void* data1, int data1_size,
                             void* data2, int data2_size, int timeout)
{
    assert(handle >= 0 && handle < MSGQ_MAX_STORES);
    assert(msgq_stores[handle]);
    assert((data1_size >= 0) && (data2_size >= 0));
    assert((data1_size + data2_size) > 0);

    int status;
    int data_size = data1_size + data2_size;
    int object_size = offsetof(bp_object_t, data) + data_size;
    bp_object_t* object = (bp_object_t*)malloc(object_size);

    /* Check memory allocation */
    if(!object) return BP_FAILEDSTORE;

    /* Populate Object */
    object->handle = handle;
    object->sid = BP_SID_VACANT;
    object->size = data_size;
    memcpy(object->data, data1, data1_size);
    memcpy(&object->data[data1_size], data2, data2_size);

    /* Post object */
    status = msgq_post(msgq_stores[handle], object, object_size);
    if(status > 0)
    {
        msgq_counts[handle]++;
        return BP_SUCCESS;
    }
    else if(status == MSGQ_FULL)
    {
        free(object);
        bplib_os_sleep(timeout / 1000);
        return BP_TIMEOUT;
    }
    else
    {
        free(object);
        return BP_FAILEDSTORE;
    }
}

/*----------------------------------------------------------------------------
 * bplib_store_ram_dequeue -
 *----------------------------------------------------------------------------*/
int bplib_store_ram_dequeue(int handle, bp_object_t** object, int timeout)
{
    int size;
    
    assert(handle >= 0 && handle < MSGQ_MAX_STORES);
    assert(msgq_stores[handle]);
    assert(object);

    bp_object_t* dequeued_object;
    int status = msgq_receive(msgq_stores[handle], (void**)&dequeued_object, &size, timeout);
    if(status == MSGQ_OKAY)
    {
        (void)size; /* unused */
        dequeued_object->sid = dequeued_object; /* only update sid */
        *object = dequeued_object;
        return BP_SUCCESS;
    }
    else if(status == MSGQ_TIMEOUT || status == MSGQ_UNDERFLOW)
    {
        return BP_TIMEOUT;
    }
    else
    {
        return BP_FAILEDSTORE;
    }
}

/*----------------------------------------------------------------------------
 * bplib_store_ram_retrieve -
 *----------------------------------------------------------------------------*/
int bplib_store_ram_retrieve(int handle, bp_sid_t sid, 
                             bp_object_t** object, int timeout)
{
    (void)handle;
    (void)timeout;

    assert(handle >= 0 && handle < MSGQ_MAX_STORES);
    assert(msgq_stores[handle]);
    assert(object);

    *object = (bp_object_t*)sid;
    
    return BP_SUCCESS;
}

/*----------------------------------------------------------------------------
 * bplib_store_ram_release -
 *----------------------------------------------------------------------------*/
int bplib_store_ram_release (int handle, bp_sid_t sid)
{
    (void)handle;
    (void)sid;
    
    return BP_SUCCESS;
}

/*----------------------------------------------------------------------------
 * bplib_store_ram_relinquish -
 *----------------------------------------------------------------------------*/
int bplib_store_ram_relinquish (int handle, bp_sid_t sid)
{
    (void)handle;
    
    assert(handle >= 0 && handle < MSGQ_MAX_STORES);
    assert(msgq_stores[handle]);

    bp_object_t* object = (void*)sid;
    free(object);
    msgq_counts[handle]--;

    return BP_SUCCESS;
}

/*----------------------------------------------------------------------------
 * bplib_store_ram_getcount -
 *----------------------------------------------------------------------------*/
int bplib_store_ram_getcount (int handle)
{
    assert(handle >= 0 && handle < MSGQ_MAX_STORES);
    assert(msgq_stores[handle]);

    return msgq_counts[handle];
}
