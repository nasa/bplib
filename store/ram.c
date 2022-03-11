/*
 * NASA Docket No. GSC-18,587-1 and identified as “The Bundle Protocol Core Flight
 * System Application (BP) v6.5”
 *
 * Copyright © 2020 United States Government as represented by the Administrator of
 * the National Aeronautics and Space Administration. All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/******************************************************************************
 * INCLUDES
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "bplib.h"
#include "bplib_os.h"
#include "bplib_store_ram.h"

/******************************************************************************
 * DEFINES
 ******************************************************************************/

#define MSGQ_OKAY           (0)
#define MSGQ_TIMEOUT        (-1)
#define MSGQ_ERROR          (-2)
#define MSGQ_FULL           (-3)
#define MSGQ_MEMORY_ERROR   (-4)
#define MSGQ_UNDERFLOW      (-5)
#define MSGQ_INVALID_HANDLE ((msgq_t)NULL)
#define MSGQ_DEPTH_INFINITY 0
#define MSGQ_SIZE_INFINITY  0
#define MSGQ_STORE_STR      "bplibq"
#define MSGQ_STORE_STR_SIZE 32

/* Configurable Options */

#ifndef MSGQ_MAX_STORES
#define MSGQ_MAX_STORES 60
#endif

#ifndef MSGQ_MAX_DEPTH
#define MSGQ_MAX_DEPTH 65536
#endif

#ifndef MSGQ_MAX_SIZE
#define MSGQ_MAX_SIZE MSGQ_SIZE_INFINITY
#endif

/******************************************************************************
 * TYPEDEFS
 ******************************************************************************/

/* queue_node_t */
typedef struct queue_block_t
{
    void                 *data;
    unsigned int          size;
    struct queue_block_t *next;
} queue_node_t;

/* queue_t */
typedef struct queue_def_t
{
    queue_node_t *front; /* oldest node removed (read pointer) */
    queue_node_t *rear;  /* newest node added (write pointer) */
    unsigned int  depth; /* maximum length of linked list */
    unsigned int  len;   /* current length of linked list (lazy deallocation means this includes non-active items) */
    unsigned int  max_data_size; /* largest item that can be queued */
} queue_t;

/* message_queue_t */
typedef struct
{
    queue_t     queue; /* linked list */
    bp_handle_t ready; /* handle for mutex/conditional */
    int         state; /* state of queue */
    int         count; /* number of items in queue */
} message_queue_t;

/* message queue handle */
typedef message_queue_t *msgq_t;

/******************************************************************************
 * FILE DATA
 ******************************************************************************/

static msgq_t msgq_stores[MSGQ_MAX_STORES];

/******************************************************************************
 * LOCAL QUEUE FUNCTIONS
 ******************************************************************************/

/*----------------------------------------------------------------------------
 * Function:        flush_queue
 *----------------------------------------------------------------------------*/
BP_LOCAL_SCOPE void flush_queue(queue_t *q)
{
    queue_node_t *temp;

    while (q->front)
    {
        temp = q->front->next;
        bplib_os_free(q->front->data);
        bplib_os_free(q->front);
        q->front = temp;
    }
    q->rear = NULL;
}

/*----------------------------------------------------------------------------
 * Function:        isempty
 *----------------------------------------------------------------------------*/
BP_LOCAL_SCOPE int isempty(queue_t *q)
{
    if (q->front == NULL)
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
BP_LOCAL_SCOPE int enqueue(queue_t *q, void *data, int size)
{
    /* check if queue is full */
    if ((q->depth != MSGQ_DEPTH_INFINITY) && (q->len >= q->depth))
    {
        return MSGQ_FULL;
    }

    /* check size */
    if ((size <= 0) || ((q->max_data_size != MSGQ_SIZE_INFINITY) && ((unsigned)size > q->max_data_size)))
    {
        return MSGQ_ERROR;
    }

    /* create temp node */
    queue_node_t *temp = (queue_node_t *)bplib_os_calloc((int)sizeof(queue_node_t));
    if (!temp)
    {
        return MSGQ_MEMORY_ERROR;
    }

    /* construct node to be added */
    temp->data = data;
    temp->size = size;
    temp->next = NULL;

    /* place temp node into queue */
    if (q->rear == NULL)
    {
        q->rear  = temp;
        q->front = temp;
    }
    else /* q->rear != NULL */
    {
        q->rear->next = temp;
        q->rear       = q->rear->next;
    }

    q->len++;

    return MSGQ_OKAY;
}

/*----------------------------------------------------------------------------
 * Function:        dequeue
 *----------------------------------------------------------------------------*/
BP_LOCAL_SCOPE void *dequeue(queue_t *q, int *size)
{
    void *data;

    if (q->front)
    {
        /* extract */
        data = q->front->data;
        if (size)
        {
            *size = q->front->size;
        }

        /* remove */
        queue_node_t *tmp = q->front;
        if (q->front == q->rear)
        {
            q->front = q->rear = NULL;
        }
        else
        {
            q->front = q->front->next;
        }
        bplib_os_free(tmp);

        q->len--;
    }
    else
    {
        if (size)
        {
            *size = 0;
        }
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
BP_LOCAL_SCOPE msgq_t msgq_create(int depth, int data_size)
{
    message_queue_t *msgQ;
    bp_handle_t      ready_lock;

    /* Create Lock */
    ready_lock = bplib_os_createlock();
    if (!bp_handle_is_valid(ready_lock))
    {
        printf("ERROR(%d): Unable to create ready sem\n", bp_handle_printable(ready_lock));
        return MSGQ_INVALID_HANDLE;
    }

    /* Allocate MSG Q */
    msgQ = (message_queue_t *)bplib_os_calloc(sizeof(message_queue_t));
    if (msgQ == NULL)
    {
        printf("ERROR, Unable to allocate message queue\n");
        return MSGQ_INVALID_HANDLE;
    }

    /* Initialize MSG Q */
    msgQ->state               = MSGQ_OKAY;
    msgQ->queue.front         = NULL;
    msgQ->queue.rear          = NULL;
    msgQ->queue.depth         = depth;
    msgQ->queue.len           = 0;
    msgQ->queue.max_data_size = data_size;
    msgQ->ready               = ready_lock;

    /* Return MSG Q */
    return (msgq_t)msgQ;
}

/*----------------------------------------------------------------------------
 * Function:        msgq_delete
 *
 * Notes: 1. de-allocates memory associated with message queue
 *        2. removes queue from system list
 *----------------------------------------------------------------------------*/
BP_LOCAL_SCOPE void msgq_delete(msgq_t queue_handle)
{
    message_queue_t *msgQ = (message_queue_t *)queue_handle;
    if (msgQ)
    {
        flush_queue(&msgQ->queue);
        bplib_os_destroylock(msgQ->ready);
        bplib_os_free(msgQ);
    }
}

/*----------------------------------------------------------------------------
 * Function:        msgq_post
 *----------------------------------------------------------------------------*/
BP_LOCAL_SCOPE int msgq_post(msgq_t queue_handle, void *data, int size)
{
    int post_state;

    message_queue_t *msgQ = (message_queue_t *)queue_handle;
    if (msgQ == NULL)
    {
        return MSGQ_ERROR;
    }

    /* Post Data */
    bplib_os_lock(msgQ->ready);
    {
        post_state  = enqueue(&msgQ->queue, data, size);
        msgQ->state = post_state;
    }
    bplib_os_unlock(msgQ->ready);

    /* Trigger if Ready */
    if (post_state == MSGQ_OKAY)
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
BP_LOCAL_SCOPE int msgq_receive(msgq_t queue_handle, void **data, int *size, int block)
{
    message_queue_t *msgQ = (message_queue_t *)queue_handle;
    if (msgQ == NULL)
    {
        return MSGQ_ERROR;
    }

    int recv_state = MSGQ_OKAY;

    bplib_os_lock(msgQ->ready);
    {
        /* Wait for a message to be posted */
        if (block == BP_PEND)
        {
            while (isempty(&msgQ->queue))
            {
                bplib_os_waiton(msgQ->ready, BP_PEND);
            }
        }
        else if (block != BP_CHECK)
        {
            /* Timed Wait */
            if (isempty(&msgQ->queue))
            {
                int wait_status = bplib_os_waiton(msgQ->ready, block);
                if (wait_status == BP_TIMEOUT)
                {
                    recv_state = MSGQ_TIMEOUT;
                }
                else if (wait_status == BP_ERROR)
                {
                    recv_state = MSGQ_ERROR;
                }
            }
        }

        /* Get data from queue */
        msgQ->state = recv_state;
        if (msgQ->state == MSGQ_OKAY)
        {
            *data = dequeue(&msgQ->queue, size);
            if (*data == NULL)
            {
                recv_state = MSGQ_UNDERFLOW;
            }
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
void bplib_store_ram_init(void)
{
    memset(msgq_stores, 0, sizeof(msgq_stores));
}

/*----------------------------------------------------------------------------
 * bplib_store_ram_create -
 *----------------------------------------------------------------------------*/
bp_handle_t bplib_store_ram_create(int type, bp_ipn_t node, bp_ipn_t service, bool recover, void *parm)
{
    (void)type;
    (void)node;
    (void)service;
    (void)recover;
    (void)parm;

    bp_handle_t slot;
    int         i;

    /* Look for Empty Slots */
    slot = BP_INVALID_HANDLE;
    for (i = 0; i < MSGQ_MAX_STORES; i++)
    {
        if (msgq_stores[i] == MSGQ_INVALID_HANDLE)
        {
            msgq_t msgq = msgq_create(MSGQ_MAX_DEPTH, MSGQ_MAX_SIZE);
            if (msgq != MSGQ_INVALID_HANDLE)
            {
                msgq_stores[i] = msgq;
                slot           = bp_handle_from_serial(i, BPLIB_HANDLE_RAM_STORE_BASE);
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
int bplib_store_ram_destroy(bp_handle_t h)
{
    int handle = bp_handle_to_serial(h, BPLIB_HANDLE_RAM_STORE_BASE);

    assert(handle >= 0 && handle < MSGQ_MAX_STORES);
    assert(msgq_stores[handle] != MSGQ_INVALID_HANDLE);

    msgq_delete(msgq_stores[handle]);
    msgq_stores[handle] = MSGQ_INVALID_HANDLE;

    return BP_SUCCESS;
}

/*----------------------------------------------------------------------------
 * bplib_store_ram_enqueue -
 *----------------------------------------------------------------------------*/
int bplib_store_ram_enqueue(bp_handle_t h, const void *data1, size_t data1_size, const void *data2, size_t data2_size,
                            int timeout)
{
    int handle = bp_handle_to_serial(h, BPLIB_HANDLE_RAM_STORE_BASE);

    assert(handle >= 0 && handle < MSGQ_MAX_STORES);
    assert(msgq_stores[handle]);
    assert((data1_size >= 0) && (data2_size >= 0));
    assert((data1_size + data2_size) > 0);

    int          status;
    size_t       data_size   = data1_size + data2_size;
    size_t       object_size = sizeof(bp_object_hdr_t) + data_size;
    bp_object_t *object      = (bp_object_t *)bplib_os_calloc(object_size);

    /* Check memory allocation */
    if (!object)
    {
        return BP_ERROR;
    }

    /* Populate Object */
    object->header.handle = h;
    object->header.sid    = BP_SID_VACANT;
    object->header.size   = data_size;
    memcpy(object->data, data1, data1_size);
    memcpy(&object->data[data1_size], data2, data2_size);

    /* Post object */
    status = msgq_post(msgq_stores[handle], object, object_size);
    if (status == MSGQ_OKAY)
    {
        msgq_stores[handle]->count++;
        return BP_SUCCESS;
    }
    else if (status == MSGQ_FULL)
    {
        bplib_os_free(object);
        bplib_os_sleep(timeout / 1000);
        return BP_TIMEOUT;
    }
    else
    {
        bplib_os_free(object);
        return BP_ERROR;
    }
}

/*----------------------------------------------------------------------------
 * bplib_store_ram_dequeue -
 *----------------------------------------------------------------------------*/
int bplib_store_ram_dequeue(bp_handle_t h, bp_object_t **object, int timeout)
{
    int handle = bp_handle_to_serial(h, BPLIB_HANDLE_RAM_STORE_BASE);
    int size;

    assert(handle >= 0 && handle < MSGQ_MAX_STORES);
    assert(msgq_stores[handle]);
    assert(object);

    bp_object_t *dequeued_object;
    int          status = msgq_receive(msgq_stores[handle], (void **)&dequeued_object, &size, timeout);
    if (status == MSGQ_OKAY)
    {
        (void)size;                                                   /* unused */
        dequeued_object->header.sid = (unsigned long)dequeued_object; /* only update sid */
        *object                     = dequeued_object;
        return BP_SUCCESS;
    }
    else if (status == MSGQ_TIMEOUT || status == MSGQ_UNDERFLOW)
    {
        return BP_TIMEOUT;
    }
    else
    {
        return BP_ERROR;
    }
}

/*----------------------------------------------------------------------------
 * bplib_store_ram_retrieve -
 *----------------------------------------------------------------------------*/
int bplib_store_ram_retrieve(bp_handle_t h, bp_sid_t sid, bp_object_t **object, int timeout)
{
    int handle = bp_handle_to_serial(h, BPLIB_HANDLE_RAM_STORE_BASE);

    (void)handle;
    (void)timeout;

    assert(handle >= 0 && handle < MSGQ_MAX_STORES);
    assert(msgq_stores[handle]);
    assert(object);

    *object = (bp_object_t *)sid;

    return BP_SUCCESS;
}

/*----------------------------------------------------------------------------
 * bplib_store_ram_release -
 *----------------------------------------------------------------------------*/
int bplib_store_ram_release(bp_handle_t h, bp_sid_t sid)
{
    (void)h;
    (void)sid;

    return BP_SUCCESS;
}

/*----------------------------------------------------------------------------
 * bplib_store_ram_relinquish -
 *----------------------------------------------------------------------------*/
int bplib_store_ram_relinquish(bp_handle_t h, bp_sid_t sid)
{
    int handle = bp_handle_to_serial(h, BPLIB_HANDLE_RAM_STORE_BASE);

    assert(handle >= 0 && handle < MSGQ_MAX_STORES);
    assert(msgq_stores[handle]);

    bp_object_t *object = (void *)sid;
    bplib_os_free(object);
    msgq_stores[handle]->count--;

    return BP_SUCCESS;
}

/*----------------------------------------------------------------------------
 * bplib_store_ram_getcount -
 *----------------------------------------------------------------------------*/
int bplib_store_ram_getcount(bp_handle_t h)
{
    int handle = bp_handle_to_serial(h, BPLIB_HANDLE_RAM_STORE_BASE);

    assert(handle >= 0 && handle < MSGQ_MAX_STORES);
    assert(msgq_stores[handle]);

    return msgq_stores[handle]->count;
}
