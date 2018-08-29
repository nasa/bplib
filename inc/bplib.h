/******************************************************************************
 * Filename     : bplib.h
 * Purpose      : Header file for the Bundle Protocol Library
 * Design Notes :
 *******************************************************************************/

#ifndef __BPLIB_H__
#define __BPLIB_H__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdint.h>

/******************************************************************************
 DEFINES
 ******************************************************************************/

/* Booleans */
#define BP_TRUE                         1
#define BP_FALSE                        0

/* Storage ID Special Values */
#define BP_SID_VACANT                   NULL

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
#define BP_INVALIDCHANNEL               (-7)
#define BP_INVALIDHANDLE                (-8)
#define BP_OVERFLOW                     (-9)
#define BP_WRONGVERSION                 (-10)
#define BP_BUNDLEPARSEERR               (-11)
#define BP_UNKNOWNREC                   (-12)
#define BP_BUNDLETOOLARGE               (-13)
#define BP_PAYLOADTOOLARGE              (-14)
#define BP_WRONGCHANNEL                 (-15)
#define BP_FAILEDINTEGRITYCHECK         (-16)
#define BP_FAILEDSTORE                  (-17)
#define BP_FAILEDRESPONSE               (-18)

/* Processing, Acceptance,and Load Flags */
#define BP_FLAG_NONCOMPLIANT            0x00000001  // valid bundle but agent not able to comply with standard
#define BP_FLAG_INCOMPLETE              0x00000002  // block in bundle was not recognized
#define BP_FLAG_REPORTDELETE            0x00000004  // status report must be generated if bundle is deleted
#define BP_FLAG_TOOMANYSOURCES          0x00000008  // too many sources to keep track of for ACS bundles
#define BP_FLAG_FILLOVERFLOW            0x00000010  // a gap in the CIDs exceeds the max fill
#define BP_FLAG_TOOMANYFILLS            0x00000020  // all the fills in the ACS are used
#define BP_FLAG_CIDWENTBACKWARDS        0x00000040  // the custody ID went backwards
#define BP_FLAG_UNABLETOSTORE           0x00000080  // unable to store the ACS bundle for later transmission
#define BP_FLAG_ROUTENEEDED             0x00000100  // the bundle returned needs to be routed before transmission
#define BP_FLAG_STOREFAILURE            0x00000200  // storage service failed to deliver data
#define BP_FLAG_PARSEFAILURE            0x00000400  // storage service failed to deliver data
#define BP_FLAG_MIXEDRESPONSE           0x00000800  // aggregate acknowledgement must have uniform delivery vs. forward

/* Set/Get Option Defines */
#define BP_OPT_DSTNODE_D                1
#define BP_OPT_DSTSERV_D                2
#define BP_OPT_SRCNODE_D                3
#define BP_OPT_SRCSERV_D                4
#define BP_OPT_RPTNODE_D                5
#define BP_OPT_RPTSERV_D                6
#define BP_OPT_CSTNODE_D                7
#define BP_OPT_CSTSERV_D                8
#define BP_OPT_CREATETIMESYS_D          9
#define BP_OPT_CREATETIMEVAL_D          10
#define BP_OPT_SETSEQUENCE_D            11
#define BP_OPT_LIFETIME_D               12
#define BP_OPT_CSTRQST_D                13
#define BP_OPT_ALLOWFRAG_D              14
#define BP_OPT_PAYCRC_D                 15
#define BP_OPT_TIMEOUT                  16
#define BP_OPT_BUNDLELEN                17
#define BP_OPT_PROCADMINONLY            18
#define BP_OPT_WRAPRSP                  19
#define BP_OPT_ACSRATE                  20

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* IPN Schema Endpoint ID Integer Definition */
typedef uint16_t bp_ipn_t;

/* Bundle Time Stamp Definition */
typedef struct {
    uint32_t    s;     // seconds since epoch (January 1, 2000)
    uint32_t    ns;    // nanoseconds within the second
} bp_time_t;

/* Storage ID */
typedef void* bp_sid_t;

/* Storage API */
typedef int (*bp_store_create_t)    (void);
typedef int (*bp_store_destroy_t)   (int handle);
typedef int (*bp_store_enqueue_t)   (int handle, void* data1, int data1_size, void* data2, int data2_size, int timeout);
typedef int (*bp_store_dequeue_t)   (int handle, void** data, int* size, bp_sid_t* sid, int timeout);
typedef int (*bp_store_retrieve_t)  (int handle, void** data, int* size, bp_sid_t sid, int timeout);
typedef int (*bp_store_refresh_t)   (int handle, void* data, int size, int offset, bp_sid_t sid, int timeout);
typedef int (*bp_store_relinquish_t)(int handle, bp_sid_t sid);

/* Bundle Storage Service */
typedef struct {
    bp_store_create_t       create;
    bp_store_destroy_t      destroy;
    bp_store_enqueue_t      enqueue;
    bp_store_dequeue_t      dequeue;
    bp_store_retrieve_t     retrieve;
    bp_store_refresh_t      refresh;
    bp_store_relinquish_t   relinquish;
} bp_store_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

void    bplib_init      (void);
int     bplib_open      (bp_store_t store, bp_ipn_t local_node, bp_ipn_t local_service, bp_ipn_t destination_node, bp_ipn_t destination_service);
void    bplib_close     (int channel);

int     bplib_getopt    (int channel, int opt, void* val, int len);
int     bplib_setopt    (int channel, int opt, void* val, int len);

int     bplib_store     (int channel, void* payload, int size, int timeout);
int     bplib_load      (int channel, void* bundle,  int size, int timeout, uint32_t* loadflags); 
int     bplib_process   (int channel, void* bundle,  int size, int timeout, uint32_t* procflags);
int     bplib_accept    (int channel, void* payload, int size, int timeout, uint32_t* acptflags);

int     bplib_routeinfo (void* bundle, int size, bp_ipn_t* destination_node, bp_ipn_t* destination_service);
int     bplib_addtime   (bp_time_t* result, bp_time_t tm, int sec);
int     bplib_cmptime   (bp_time_t tm1, bp_time_t tm2);

#endif  /* __BPLIB_H__ */
