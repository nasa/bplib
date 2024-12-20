#ifndef __GSFC_BPLIB_QM_H__
#define __GSFC_BPLIB_QM_H__

#define QM_MAX_CONTACTS 3U
#define QM_MAX_CHAN 3U


typedef struct BPLib_Contact
{
    // other contact state

    // ingress
    BPLib_BundleQueue_t bi_to_ebp_q;
    BPLib_BundleQueue_t ebp_to_ct_q;
    BPLib_BundleQueue_t ct_to_stor_q;
    // egress
    BPLib_BundleQueue_t stor_to_ct_q;
    BPLib_BundleQueue_t ct_to_ebq_q;
    BPLib_BundleQueue_t ebp_to_bi_q;
} BPLib_Contact_t;

typedef struct BPLib_Channel
{
    // other channel state

    // ingress
    BPLib_BundleQueue_t pi_to_ebp_q;
    BPLib_BundleQueue_t ebp_to_ct_q;
    BPLib_BundleQueue_t ct_to_stor_q;

    // egress
    BPLib_BundleQueue_t stor_to_ct_q;
    BPLib_BundleQueue_t ct_to_ebq_q;
    BPLib_BundleQueue_t ebp_to_pi_q;
} BPLib_Channel_t;

struct BPLib_QueueTable
{
    BPLib_BundleQueue_t job_queue;
    BPLib_Contact_t contact_list[QM_MAX_CONTACTS];
    BPLib_Channel_t channel_list[QM_MAX_CHAN];
} BPLib_QueueTable_t;

/* Define all the job callbacks */


BPLib_QueueTable_t* BPLib_QM_Get_QueueTable();

#endif /* __GSFC_BPLIB_QM_H__ */
