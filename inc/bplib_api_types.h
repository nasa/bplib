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

#ifndef BPLIB_API_TYPES_H
#define BPLIB_API_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/*
** Include Files
*/

#include "bplib_cfg.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

/*
** Type Definitions
*/

/**
 * @brief Canonical block type
 *
 * @note The numeric values match the block type values in BPv7 section 9.1. The v6 types 
 *       are not included.
 */
enum BPLib_BlockType
{
    BPLib_BlockType_Reserved = 0,
    BPLib_BlockType_Payload = 1,
    BPLib_BlockType_PrevNode = 6,
    BPLib_BlockType_Age = 7,
    BPLib_BlockType_HopCount = 10,
    BPLib_BlockType_CTEB = 15,
    BPLib_BlockType_CREB = 16
};

typedef uint8_t BPLib_BlockType_t;

/**
 * \brief BPLib status type for type safety
 */
typedef int32_t BPLib_Status_t;

typedef struct BPLib_handle
{
    uint32_t hdl;
} BPLib_Handle_t;

// Integer typedefs

typedef uintmax_t BPLib_Val_t;
typedef intmax_t  BPLib_Sval_t;
typedef uint16_t  BPLib_Index_t;

// Bundle Protocol Typedefs

/* IPN Schema Endpoint ID Integer Definition */
typedef BPLib_Val_t BPLib_Ipn_t;

/* combine IPN node+service */
typedef struct BPLib_IpnAddr
{
    BPLib_Ipn_t node_number;
    BPLib_Ipn_t service_number;
} BPLib_IpnAddr_t;

typedef struct BPLib_Instance BPLib_Instance_t;

typedef struct BPLib_BundleCache BPLib_BundleCache_t;

/*
** Macros
*/

#define BPLIB_BUNDLE_PROTOCOL_VERSION       (7)     /** @brief Version of Bundle Protocol being implemented */

/**
 * \brief Job egress ID to use before a bundle's route is known 
*/
#define BPLIB_UNKNOWN_ROUTE_ID          (BPLIB_MAX_NUM_CHANNELS + BPLIB_MAX_NUM_CONTACTS)

/** @defgroup BPLib_ReturnCodes BPLib Return Codes
 * @{
 */
/* General Return Codes */
#define BPLIB_TBL_UPDATED                   ((BPLib_Status_t)  1)  /* Configuration has been updated */
#define BPLIB_SUCCESS                       ((BPLib_Status_t)  0)  /* Successful execution */
#define BPLIB_ERROR                         ((BPLib_Status_t) -1)  /* Failed execution */
#define BPLIB_UNIMPLEMENTED                 ((BPLib_Status_t) -2)  /* Unimplemented function */
#define BPLIB_UNKNOWN                       ((BPLib_Status_t) -3)  /* Unknown return status */
#define BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE   ((BPLib_Status_t) -4)  /* Configuration validation error code */
#define BPLIB_RBT_DUPLICATE                 ((BPLib_Status_t) -5)  /* BPLib Red-Black Tree (RBT) Duplicate Search Result */
#define BPLIB_TIMEOUT                       ((BPLib_Status_t) -6)  /* Timeout pending on a queue */
#define BPLIB_NULL_PTR_ERROR                ((BPLib_Status_t) -7)  /* Null pointer error */
#define BPLIB_BUF_LEN_ERROR                 ((BPLib_Status_t) -8)  /* Buffer length error */
#define BPLIB_INVALID_EID                   ((BPLib_Status_t) -9)  /* Invalid endpoint identification */
#define BPLIB_INVALID_EID_PATTERN           ((BPLib_Status_t) -10) /* Invalid endpoint identification pattern */
#define BPLIB_INVALID_CRC_ERROR             ((BPLib_Status_t) -11) /* Invalid CRC */
#define BPLIB_OS_ERROR                      ((BPLib_Status_t) -12)

/*

#define BPLIB_GENERIC_ERROR_17              ((BPLib_Status_t) -17) // Error description
#define BPLIB_GENERIC_ERROR_18              ((BPLib_Status_t) -18) // Error description
#define BPLIB_GENERIC_ERROR_19              ((BPLib_Status_t) -19) // Error description
#define BPLIB_GENERIC_ERROR_20              ((BPLib_Status_t) -20) // Error description
#define BPLIB_GENERIC_ERROR_21              ((BPLib_Status_t) -21) // Error description
#define BPLIB_GENERIC_ERROR_22              ((BPLib_Status_t) -22) // Error description
#define BPLIB_GENERIC_ERROR_23              ((BPLib_Status_t) -23) // Error description
#define BPLIB_GENERIC_ERROR_24              ((BPLib_Status_t) -24) // Error description
#define BPLIB_GENERIC_ERROR_25              ((BPLib_Status_t) -25) // Error description
*/

/** @defgroup BPLib_ErrorCodes BPLib Error Code Defines
 * @{
 */
/* Framework Proxy Errors */
#define BPLIB_FWP_CALLBACK_INIT_ERROR       ((BPLib_Status_t) -26)

/* Time Management Errors */
#define BPLIB_TIME_UNDEF_DELTA_ERROR        ((BPLib_Status_t) -27)
#define BPLIB_TIME_WRITE_ERROR              ((BPLib_Status_t) -28)
#define BPLIB_TIME_READ_ERROR               ((BPLib_Status_t) -29)
#define BPLIB_TIME_UNINIT_ERROR             ((BPLib_Status_t) -30)

/* Event Management Errors */
#define BPLIB_EM_STRING_TRUNCATED           ((BPLib_Status_t) -31)
#define BPLIB_EM_ILLEGAL_APP_ID             ((BPLib_Status_t) -32)
#define BPLIB_EM_UNKNOWN_FILTER             ((BPLib_Status_t) -33)
#define BPLIB_EM_BAD_ARGUMENT               ((BPLib_Status_t) -34)
#define BPLIB_EM_INVALID_PARAMETER          ((BPLib_Status_t) -35)
#define BPLIB_EM_APP_NOT_REGISTERED         ((BPLib_Status_t) -36)
#define BPLIB_EM_APP_SQUELCHED              ((BPLib_Status_t) -37)
#define BPLIB_EM_EXPANDED_TEXT_ERROR        ((BPLib_Status_t) -38)

/* PerfLog Proxy Errors*/
#define BPLIB_PL_NULL_CALLBACK_ERROR        ((BPLib_Status_t) -39)

/* Node Configuration (NC) errors */
#define BPLIB_NC_INIT_CONFIG_PTRS_ERROR     ((BPLib_Status_t) -40)
#define BPLIB_NC_INVALID_MIB_ITEM_INDEX     ((BPLib_Status_t) -41)
#define BPLIB_NC_INVALID_MID_VALUE          ((BPLib_Status_t) -42)

/* CLA Errors*/
#define BPLIB_CLA_TIMEOUT                   ((BPLib_Status_t) -43)

/* Payload Interface Errors */
#define BPLIB_PI_INVALID_CONFIG_ERROR       ((BPLib_Status_t) -44)
#define BPLIB_PI_TIMEOUT                    ((BPLib_Status_t) -45)

/* Admin Statistics (AS) Errors */
#define BPLIB_AS_INIT_MUTEX_ERR             ((BPLib_Status_t) -46)
#define BPLIB_AS_INVALID_EID                ((BPLib_Status_t) -47)
#define BPLIB_AS_UNKNOWN_NODE_CNTR          ((BPLib_Status_t) -48)
#define BPLIB_AS_UNKNOWN_SRC_CNTR           ((BPLib_Status_t) -49)
#define BPLIB_AS_UNKNOWN_MIB_ARRAY_EID      ((BPLib_Status_t) -50)
#define BPLIB_AS_MIB_KEY_ARRAY_FULL         ((BPLib_Status_t) -51)
#define BPLIB_AS_MIB_KEYS_OVERLAP           ((BPLib_Status_t) -52)
#define BPLIB_AS_INVALID_MIB_INDEX          ((BPLib_Status_t) -53)
#define BPLIB_AS_NO_KEYS_GIVEN              ((BPLib_Status_t) -54)

/* Queue Manager (QM) Errors */
#define BPLIB_QM_PUSH_ERROR                 ((BPLib_Status_t) -55)

/* MEM Errors */
#define BPLIB_MEM_INITMEM_UNALIGN           ((BPLib_Status_t) -56)
#define BPLIB_MEM_CPY_FRM_OFFSET_NE_ERR     ((BPLib_Status_t) -57) /* BPLib_MEM_CopyOutFromOffset: bytes copied != requested */

/* Node Config Errors */
#define BPLIB_NC_TBL_UPDATE_ERR             ((BPLib_Status_t) -80)

/* CBOR Decode Errors */
#define BPLIB_CBOR_DEC_BUNDLE_TOO_SHORT_ERR            ((BPLib_Status_t) -120) /* CBOR decode error: bundle too short */
#define BPLIB_CBOR_DEC_BUNDLE_ENTER_ARRAY_ERR          ((BPLib_Status_t) -121) /* CBOR decode error: entry array */
#define BPLIB_CBOR_DEC_BUNDLE_MAX_BLOCKS_ERR           ((BPLib_Status_t) -122) /* CBOR decode error: max blocks */
#define BPLIB_CBOR_DEC_BUNDLE_EXIT_ARRAY_ERR           ((BPLib_Status_t) -123) /* CBOR decode error: entry array */

#define BPLIB_CBOR_DEC_PRIM_ENTER_ARRAY_ERR            ((BPLib_Status_t) -124) /* CBOR primary block decode error: entry array */
#define BPLIB_CBOR_DEC_PRIM_EXIT_ARRAY_ERR             ((BPLib_Status_t) -125) /* CBOR primary block decode error: exit array */
#define BPLIB_CBOR_DEC_PRIM_VERSION_DEC_ERR            ((BPLib_Status_t) -126) /* CBOR primary block decode error: decode version field */
#define BPLIB_CBOR_DEC_PRIM_WRONG_VERSION_ERR          ((BPLib_Status_t) -127) /* CBOR primary block decode error: wrong version field */
#define BPLIB_CBOR_DEC_PRIM_FLAG_DEC_ERR               ((BPLib_Status_t) -128) /* CBOR primary block decode error: decode flag field */
#define BPLIB_CBOR_DEC_PRIM_WRONG_FLAG_ERR             ((BPLib_Status_t) -129) /* CBOR primary block decode error: wrong flag field */
#define BPLIB_CBOR_DEC_PRIM_CRC_TYPE_DEC_ERR           ((BPLib_Status_t) -130) /* CBOR primary block decode error: decode crc type field */
#define BPLIB_CBOR_DEC_PRIM_DEST_EID_DEC_ERR           ((BPLib_Status_t) -131) /* CBOR primary block decode error: decode Dest EID field */
#define BPLIB_CBOR_DEC_PRIM_SRC_EID_DEC_ERR            ((BPLib_Status_t) -132) /* CBOR primary block decode error: decode Src EID field */
#define BPLIB_CBOR_DEC_PRIM_REPORT_EID_DEC_ERR         ((BPLib_Status_t) -133) /* CBOR primary block decode error: decode Report-To EID field */
#define BPLIB_CBOR_DEC_PRIM_CREATE_TIME_DEC_ERR        ((BPLib_Status_t) -134) /* CBOR primary block decode error: decode create time field */
#define BPLIB_CBOR_DEC_PRIM_LIFETIME_DEC_ERR           ((BPLib_Status_t) -135) /* CBOR primary block decode error: decode lifetime field */
#define BPLIB_CBOR_DEC_PRIM_CRC_VAL_DEC_ERR            ((BPLib_Status_t) -136) /* CBOR primary block decode error: decode crc value field */

#define BPLIB_CBOR_DEC_CANON_BLOCK_INDEX_ERR           ((BPLib_Status_t) -137) /* CBOR canon block decode error: invalid block index */
#define BPLIB_CBOR_DEC_CANON_ENTER_ARRAY_ERR           ((BPLib_Status_t) -138) /* CBOR canon block decode error: entry array */
#define BPLIB_CBOR_DEC_CANON_EXIT_ARRAY_ERR            ((BPLib_Status_t) -139) /* CBOR canon block decode error: exit array */
#define BPLIB_CBOR_DEC_CANON_BLOCK_TYPE_DEC_ERR        ((BPLib_Status_t) -140) /* CBOR canon block decode error: decode block type */
#define BPLIB_CBOR_DEC_CANON_BLOCK_NUM_DEC_ERR         ((BPLib_Status_t) -141) /* CBOR canon block decode error: decode block num */
#define BPLIB_CBOR_DEC_CANON_BLOCK_FLAG_DEC_ERR        ((BPLib_Status_t) -142) /* CBOR canon block decode error: decode block proc flags */
#define BPLIB_CBOR_DEC_CANON_CRC_TYPE_DEC_ERR          ((BPLib_Status_t) -143) /* CBOR canon block decode error: decode crc type */
#define BPLIB_CBOR_DEC_CANON_ENTER_BYTE_STR_ERR        ((BPLib_Status_t) -144) /* CBOR canon block decode error: enter data byte string */
#define BPLIB_CBOR_DEC_CANON_EXIT_BYTE_STR_ERR         ((BPLib_Status_t) -145) /* CBOR canon block decode error: exit data byte string */
#define BPLIB_CBOR_DEC_CANON_CRC_VAL_DEC_ERR           ((BPLib_Status_t) -146) /* CBOR canon block decode error: decode crc value field */

#define BPLIB_CBOR_DEC_PREV_NODE_EID_DEC_ERR           ((BPLib_Status_t) -147) /* CBOR Prev Node block decode error: decode eid field */

#define BPLIB_CBOR_DEC_AGE_BLOCK_DEC_ERR               ((BPLib_Status_t) -148) /* CBOR Age block decode error: decode eid field */

#define BPLIB_CBOR_DEC_HOP_BLOCK_ENTER_ARRAY_ERR       ((BPLib_Status_t) -149) /* CBOR Hop Count block decode error: enter array */
#define BPLIB_CBOR_DEC_HOP_BLOCK_EXIT_ARRAY_ERR        ((BPLib_Status_t) -150) /* CBOR Hop Count block decode error: exit array */
#define BPLIB_CBOR_DEC_HOP_BLOCK_HOP_LIMIT_DEC_ERR     ((BPLib_Status_t) -151) /* CBOR Hop Count block decode error: hop limit decode */
#define BPLIB_CBOR_DEC_HOP_BLOCK_HOP_COUNT_DEC_ERR     ((BPLib_Status_t) -152) /* CBOR Hop Count block decode error: hop count decode */


#define BPLIB_CBOR_DEC_TYPES_ENTER_DEF_ARRAY_QCBOR_ERR ((BPLib_Status_t) -160) /* CBOR decode types error: enter def array */
#define BPLIB_CBOR_DEC_TYPES_ENTER_DEF_ARRAY_COUNT_ERR ((BPLib_Status_t) -161) /* CBOR decode types error: def array size */
#define BPLIB_CBOR_DEC_TYPES_EXIT_DEF_ARRAY_QCBOR_ERR  ((BPLib_Status_t) -162) /* CBOR decode types error: exit def array */
#define BPLIB_CBOR_DEC_TYPES_GET_UINT64_QCBOR_ERR      ((BPLib_Status_t) -163) /* CBOR decode types error: get uint64 */
#define BPLIB_CBOR_DEC_TYPES_EID_ENTER_OUTER_ARRAY_ERR ((BPLib_Status_t) -164) /* CBOR decode types error: EID enter outer array */
#define BPLIB_CBOR_DEC_TYPES_EID_SCHEME_NOT_IMPL_ERR   ((BPLib_Status_t) -165) /* CBOR decode types error: EID scheme not implemented */
#define BPLIB_CBOR_DEC_TYPES_EID_ENTER_SSP_ARRAY_ERR   ((BPLib_Status_t) -166) /* CBOR decode types error: EID enter SSP array */
#define BPLIB_CBOR_DEC_TYPES_EID_IPN_NODE_DEC_ERR      ((BPLib_Status_t) -167) /* CBOR decode types error: EID IPN node num decode */
#define BPLIB_CBOR_DEC_TYPES_EID_IPN_SERV_DEC_ERR      ((BPLib_Status_t) -168) /* CBOR decode types error: EID IPN serv num decode */
#define BPLIB_CBOR_DEC_TYPES_EID_EXIT_SSP_ARRAY_ERR    ((BPLib_Status_t) -169) /* CBOR decode types error: EID exit SSP array */
#define BPLIB_CBOR_DEC_TYPES_EID_EXIT_OUTER_ARRAY_ERR  ((BPLib_Status_t) -170) /* CBOR decode types error: EID exit outer array */

#define BPLIB_CBOR_DEC_TYPES_TIMESTAMP_ENTER_ARRAY_ERR ((BPLib_Status_t) -171) /* CBOR decode types error: timestamp enter array */
#define BPLIB_CBOR_DEC_TYPES_TIMESTAMP_EXIT_ARRAY_ERR  ((BPLib_Status_t) -172) /* CBOR decode types error: timestamp exit array */
#define BPLIB_CBOR_DEC_TYPES_TIMESTAMP_CREATE_DEC_ERR  ((BPLib_Status_t) -173) /* CBOR decode types error: timestamp create time decode */
#define BPLIB_CBOR_DEC_TYPES_TIMESTAMP_SEQ_NUM_DEC_ERR ((BPLib_Status_t) -174) /* CBOR decode types error: timestamp seq num decode */

#define BPLIB_CBOR_DEC_TYPES_CRC_ENTER_BYTE_STR_ERR    ((BPLib_Status_t) -175) /* CBOR decode types error: CRC Val enter byte-string */
#define BPLIB_CBOR_DEC_TYPES_CRC_16_LEN_ERR            ((BPLib_Status_t) -176) /* CBOR decode types error: CRC Val length not 16 */
#define BPLIB_CBOR_DEC_TYPES_CRC_32_LEN_ERR            ((BPLib_Status_t) -177) /* CBOR decode types error: CRC Val length not 32 */
#define BPLIB_CBOR_DEC_TYPES_CRC_UNSUPPORTED_TYPE_ERR  ((BPLib_Status_t) -178) /* CBOR decode types error: CRC Val type */

/* CBOR Encode Errors */
#define BPLIB_CBOR_ENC_PRIM_COPY_SIZE_GT_OUTPUT_ERR    ((BPLib_Status_t) -182) /* BPLib_CBOR_CopyOrEncodePrimary: Copy Size Error */
#define BPLIB_CBOR_ENC_PAYL_COPY_SIZE_GT_OUTPUT_ERR    ((BPLib_Status_t) -183) /* BPLib_CBOR_CopyOrEncodePayload: Copy Size Error */
#define BPLIB_CBOR_ENC_PRIM_QCBOR_FINISH_ERR           ((BPLib_Status_t) -184) /* BPLib_CBOR_EncodePrimary: QCBOREncode_Finish Error */
#define BPLIB_CBOR_ENC_EXT_INPUT_BLOCK_INDEX_ERR       ((BPLib_Status_t) -185) /* BPLib_CBOR_EncodeExtensionBlock: Ext Block Index Error */
#define BPLIB_CBOR_ENC_EXT_QCBOR_FINISH_ERR            ((BPLib_Status_t) -186) /* BPLib_CBOR_EncodeExtensionBlock: QCBOREncode_Finish Error */
#define BPLIB_CBOR_ENC_PAYL_QCBOR_FINISH_HEAD_ERR      ((BPLib_Status_t) -187) /* BPLib_CBOR_EncodeExtensionBlock: QCBOREncode_Finish Error */
#define BPLIB_CBOR_ENC_PAYL_ADD_BYTE_STR_HEAD_ERR      ((BPLib_Status_t) -188) /* BPLib_CBOR_EncodeExtensionBlock: QCBOREncode_Finish Error */
#define BPLIB_CBOR_ENC_PAYL_QCBOR_FINISH_TAIL_ERR      ((BPLib_Status_t) -189) /* BPLib_CBOR_EncodeExtensionBlock: QCBOREncode_Finish Error */

#define BPLIB_CBOR_ENC_BUNDLE_OUTPUT_BUF_LEN_1_ERR     ((BPLib_Status_t) -190) /* BPLib_CBOR_EncodeBundle: Output buf too small (check 1) */
#define BPLIB_CBOR_ENC_BUNDLE_OUTPUT_BUF_LEN_2_ERR     ((BPLib_Status_t) -191) /* BPLib_CBOR_EncodeBundle: Output buf too small (check 2) */
#define BPLIB_CBOR_ENC_BUNDLE_OUTPUT_BUF_LEN_3_ERR     ((BPLib_Status_t) -192) /* BPLib_CBOR_EncodeBundle: Output buf too small (check 3) */
#define BPLIB_CBOR_ENC_BUNDLE_OUTPUT_BUF_LEN_4_ERR     ((BPLib_Status_t) -193) /* BPLib_CBOR_EncodeBundle: Output buf too small (check 4) */

/* More PI Errors */
#define BPLIB_PI_CHAN_ID_INPUT_ERR                     ((BPLib_Status_t) -200)

// More CLA Errors */
#define BPLIB_CLA_CONT_ID_INPUT_ERR                     ((BPLib_Status_t) -210)

/* Storage Errors: SQL */
#define BPLIB_STOR_SQL_INIT_ERR                         ((BPLib_Status_t) -240)
#define BPLIB_STOR_SQL_STORAGE_ERR                      ((BPLib_Status_t) -241)
#define BPLIB_STOR_SQL_LOAD_ERR                         ((BPLib_Status_t) -242)
#define BPLIB_STOR_SQL_DISCARD_ERR                      ((BPLib_Status_t) -243)
#define BPLIB_STOR_PARAM_ERR                            ((BPLib_Status_t) -244)

// TODO TIME Helpers

// Candidates for inclusion in bplib_time.h or in bplib_stor_cache_types.h.
#define BPLIB_TIME_TO_INT(t)   ((t).Time)
#define BPLIB_TIME_FROM_INT(t) { (t) }
#define BPLIB_TIME_IS_VALID(t) (BPLIB_TIME_TO_INT(t) != 0)
#define BPLIB_TIME_IS_INFINITE(t) (BPLIB_TIME_TO_INT(t) == UINT64_MAX)

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* BPLIB_API_TYPES_H */
