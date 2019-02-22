/******************************************************************************
 * Filename     : bplib.h
 * Purpose      : Header file for the Bundle Protocol Library
 * Design Notes :
 *******************************************************************************/

#ifndef __BPLIB_H__
#define __BPLIB_H__

#ifdef __cplusplus
extern "C" {
#endif 
    
/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/******************************************************************************
 DEFINES
 ******************************************************************************/

/* Timeouts */
#define BP_PEND                         (-1)
#define BP_CHECK                        0

/* Storage ID Special Values */
#define BP_SID_VACANT                   NULL
#define BP_INVALID_HANDLE               (-1)

/* Wrap Responses */
#define BP_WRAP_RESEND                  0
#define BP_WRAP_BLOCK                   1
#define BP_WRAP_DROP                    2

/* Bundle Integrity Types */
#define BP_BIB_NONE                     0
#define BP_BIB_CRC16                    1

/* Return Codes */            
#define BP_SUCCESS                      1
#define BP_TIMEOUT                      0
#define BP_PARMERR                      (-1)
#define BP_UNSUPPORTED                  (-2)
#define BP_EXPIRED                      (-3)
#define BP_IGNORE                       (-4)
#define BP_DROPPED                      (-5)
#define BP_CHANNELSFULL                 (-6)
#define BP_INVALIDHANDLE                (-7)
#define BP_OVERFLOW                     (-8)
#define BP_WRONGVERSION                 (-9)
#define BP_BUNDLEPARSEERR               (-10)
#define BP_UNKNOWNREC                   (-11)
#define BP_BUNDLETOOLARGE               (-12)
#define BP_PAYLOADTOOLARGE              (-13)
#define BP_WRONGORIGINATION             (-14)
#define BP_WRONGCHANNEL                 (-15)
#define BP_FAILEDINTEGRITYCHECK         (-16)
#define BP_FAILEDSTORE                  (-17)
#define BP_FAILEDRESPONSE               (-18)
#define BP_FAILEDOS                     (-19)
#define BP_FAILEDMEM                    (-20)
#define BP_INVALIDEID                   (-21)
    
/* Processing, Acceptance,and Load Flags */
#define BP_FLAG_NONCOMPLIANT            0x0001  // valid bundle but agent not able to comply with standard
#define BP_FLAG_INCOMPLETE              0x0002  // block in bundle was not recognized
#define BP_FLAG_REPORTDELETE            0x0004  // status report must be generated if bundle is deleted
#define BP_FLAG_TOOMANYSOURCES          0x0008  // too many sources to keep track of for ACS bundles
#define BP_FLAG_FILLOVERFLOW            0x0010  // a gap in the CIDs exceeds the max fill
#define BP_FLAG_TOOMANYFILLS            0x0020  // all the fills in the ACS are used
#define BP_FLAG_CIDWENTBACKWARDS        0x0040  // the custody ID went backwards
#define BP_FLAG_ROUTENEEDED             0x0080  // the bundle returned needs to be routed before transmission
#define BP_FLAG_STOREFAILURE            0x0100  // storage service failed to deliver data
#define BP_FLAG_MIXEDRESPONSE           0x0200  // aggregate acknowledgement must have uniform delivery vs. forward
#define BP_FLAG_SDNVOVERFLOW            0x0400  // insufficient room in variable to read/write value
#define BP_FLAG_SDNVINCOMPLETE          0x0800  // insufficient room in block to read/write value
#define BP_FLAG_ACTIVETABLEWRAP         0x1000  // the active table wrapped

/* Set/Get Option Defines */
#define BP_OPT_DSTNODE_D                1
#define BP_OPT_DSTSERV_D                2
#define BP_OPT_RPTNODE_D                3
#define BP_OPT_RPTSERV_D                4
#define BP_OPT_CSTNODE_D                5
#define BP_OPT_CSTSERV_D                6
#define BP_OPT_SETSEQUENCE_D            7
#define BP_OPT_LIFETIME_D               8
#define BP_OPT_CSTRQST_D                9
#define BP_OPT_ALLOWFRAG_D              10
#define BP_OPT_PAYCRC_D                 11
#define BP_OPT_TIMEOUT                  12
#define BP_OPT_BUNDLELEN                13
#define BP_OPT_ORIGINATE                14
#define BP_OPT_PROCADMINONLY            15
#define BP_OPT_WRAPRSP                  16
#define BP_OPT_CIDREUSE                 17
#define BP_OPT_ACSRATE                  18
    
/* IPN Scheme Defines */
#define BP_MAX_EID_STRING               128

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* IPN Schema Endpoint ID Integer Definition */
typedef uint32_t bp_ipn_t;

/* Storage ID */
typedef void* bp_sid_t;

/* Storage API */
typedef int (*bp_store_create_t)    (void);
typedef int (*bp_store_destroy_t)   (int handle);
typedef int (*bp_store_enqueue_t)   (int handle, void* data1, int data1_size, void* data2, int data2_size, int timeout);
typedef int (*bp_store_dequeue_t)   (int handle, void** data, int* size, bp_sid_t* sid, int timeout);
typedef int (*bp_store_retrieve_t)  (int handle, void** data, int* size, bp_sid_t sid, int timeout);
typedef int (*bp_store_relinquish_t)(int handle, bp_sid_t sid);
typedef int (*bp_store_getcount_t)  (int handle);

/* Bundle Storage Service */
typedef struct {
    bp_store_create_t       create;
    bp_store_destroy_t      destroy;
    bp_store_enqueue_t      enqueue;
    bp_store_dequeue_t      dequeue;
    bp_store_retrieve_t     retrieve;
    bp_store_relinquish_t   relinquish;
    bp_store_getcount_t     getcount;
} bp_store_t;

/* Bundle Channel Attributes */
typedef struct {
    int     active_table_size;      // number of unacknowledged bundles to keep track of
    int     max_concurrent_dacs;    // number of dacs to maintain at one time
    int     max_fills_per_dacs;     // dacs is built on stack (and therefore must fit on stack)
} bp_attr_t;

/* Bundle Channel Statistics */
typedef struct {
    uint32_t    lost;           // storage or copy failure, unable to retrieve
    uint32_t    expired;        // lifetime expired, deliberately removed
    uint32_t    acknowledged;   // freed by custody signal
    uint32_t    transmitted;    // sent (includes resends)
    uint32_t    retransmitted;  // timedout and resent
    uint32_t    received;       // bundles processed
    uint32_t    delivered;      // payloads accepted
    uint32_t    bundles;        // number of data bundles currently in storage
    uint32_t    payloads;       // number of payloads currently in storage
    uint32_t    records;        // number of dacs bundles currently in storage
    uint32_t    active;         // number of slots in active table in use
} bp_stats_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

void    bplib_init          (int max_channels);
int     bplib_open          (bp_store_t store, bp_ipn_t local_node, bp_ipn_t local_service, bp_ipn_t destination_node, bp_ipn_t destination_service, bp_attr_t* attributes);
void    bplib_close         (int channel);

int     bplib_getopt        (int channel, int opt, void* val, int len);
int     bplib_setopt        (int channel, int opt, void* val, int len);
int     bplib_latchstats    (int channel, bp_stats_t* stats);

int     bplib_store         (int channel, void* payload, int size, int timeout, uint16_t* storflags);
int     bplib_load          (int channel, void* bundle,  int size, int timeout, uint16_t* loadflags); 
int     bplib_process       (int channel, void* bundle,  int size, int timeout, uint16_t* procflags);
int     bplib_accept        (int channel, void* payload, int size, int timeout, uint16_t* acptflags);

int     bplib_routeinfo     (void* bundle, int size, bp_ipn_t* destination_node, bp_ipn_t* destination_service);
int     bplib_eid2ipn       (const char* eid, int len, bp_ipn_t* node, bp_ipn_t* service);
int     bplib_ipn2eid       (char* eid, int len, bp_ipn_t node, bp_ipn_t service);

#ifdef __cplusplus
} // extern "C"
#endif 

#endif  /* __BPLIB_H__ */
