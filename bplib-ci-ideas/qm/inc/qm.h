#ifndef __GSFC_BPLIB_QM_H__
#define __GSFC_BPLIB_QM_H__

#include "bundle.h"
#include "waitqueue.h"

#define QM_MAX_CONTACTS 3U
#define QM_MAX_CHAN 3U

typedef enum BPLib_QM_Priority
{
    QM_PRI_NORMAL = 1
} BPLib_QM_Priority_t;

typedef bool (*BPLib_QM_JobFunc_t)(BPLib_Bundle_t* bundle);

typedef struct BPLib_QM_Job
{
    BPLib_QM_JobFunc_t job_func;
    BPLib_Bundle_t* bundle;
    BPLib_QM_Priority_t priority;
} BPLib_QM_Job_t;

typedef struct BPLib_QM_Event
{
    BPLib_Bundle_t* bundle;
    BPLib_QM_Priority_t priority;
} BPLib_QM_Event_t;

typedef struct BPLib_QM_QueueTable
{
    BPLib_WaitQueue_t jobs;
    BPLib_WaitQueue_t events;
    /* Can be made private, stored within waitqueue, etc */
    void* event_mem;
    void* job_mem;
} BPLib_QM_QueueTable_t;

bool BPLib_QM_QueueTableInit(BPLib_QM_QueueTable_t* tbl, size_t max_jobs);

void BPLib_QM_QueueTableDestroy(BPLib_QM_QueueTable_t* tbl);

void BPLib_QM_EventPost(BPLib_QM_QueueTable_t* tbl, BPLib_QM_Event_t* event);

void* BPLib_QM_EventLoop(BPLib_QM_QueueTable_t* tbl, bool* exit_flag);

void* BPLib_QM_WorkerFunc(BPLib_QM_QueueTable_t* tbl);

#endif /* __GSFC_BPLIB_QM_H__ */
