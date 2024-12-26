#ifndef __GSFC_BPLIB_QM_H__
#define __GSFC_BPLIB_QM_H__

#include "bundle.h"
#include "waitqueue.h"

#define QM_MAX_CONTACTS 3U
#define QM_MAX_CHAN     3U
#define QM_NOWAIT       WAITQUEUE_NONBLOCK
#define QM_WAIT_FOREVER WAITQUEUE_BLOCK_FOREVER

typedef enum BPLib_JobState
{
    STATE_CLA_TO_BI = 0,
    STATE_BI_TO_EBP,
    STATE_EBP_TO_CT,
    STATE_CT_TO_CACHE,
    STATE_CACHE_TO_CT,
    STATE_CT_TO_EBP,
    STATE_EBP_TO_BI,
    STATE_BI_TO_CLA,
    NUM_JOB_STATES /* Must be last */
} BPLib_QM_JobState_t;

typedef enum BPLib_QM_Priority
{
    QM_PRI_NORMAL = 1
} BPLib_QM_Priority_t;

typedef struct BPLib_QM_QueueTable
{
    BPLib_WaitQueue_t jobs;
    BPLib_WaitQueue_t events;
    /* Can be made private, stored within waitqueue, etc */
    void* event_mem;
    void* job_mem;
} BPLib_QM_QueueTable_t;

typedef BPLib_QM_JobState_t (*BPLib_QM_JobFunc_t)(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle);

typedef struct BPLib_QM_Job
{
    BPLib_Bundle_t* bundle;
    BPLib_QM_JobState_t state;
    BPLib_QM_JobFunc_t job_func;
    BPLib_QM_Priority_t priority;
} BPLib_QM_Job_t;

bool BPLib_QM_QueueTableInit(BPLib_QM_QueueTable_t* tbl, size_t max_jobs);

void BPLib_QM_QueueTableDestroy(BPLib_QM_QueueTable_t* tbl);

void* BPLib_QM_EventLoop(BPLib_QM_QueueTable_t* tbl, bool* exit_flag);

void BPLib_QM_RunJob(BPLib_QM_QueueTable_t* tbl, int timeout_ms);

bool BPLib_QM_PostEvent(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle,
    BPLib_QM_JobState_t state, BPLib_QM_Priority_t priority, int timeout_ms);

#endif /* __GSFC_BPLIB_QM_H__ */
