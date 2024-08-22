/*
 * TODO: Fill in file header, if necessary.
 */

#ifndef BPL_EM_H
#define BPL_EM_H

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_api_types.h"

/************************************************
 * Typedefs
 ************************************************/

typedef enum
{
    BPL_EM_EventType_UNKNOWN = 0,
    BPL_EM_EventType_DEBUG = 1,
    BPL_EM_EventType_INFO = 2,
    BPL_EM_EventType_WARNING = 3,
    BPL_EM_EventType_ERROR = 4,
    BPL_EM_EventType_CRITICAL = 5
} BPL_EM_EventType_t;

typedef struct
{
    uint32_t ReturnValue;
} BPL_Status_t;

#define BPL_STATUS_SUCCESS             (0u)
#define BPL_STATUS_ERROR_GENERAL       (1u)
#define BPL_STATUS_ERROR_INPUT_INVALID (2u)
#define BPL_STATUS_ERROR_PROXY_INIT    (3u)

typedef struct
{
    BPL_Status_t (*Initialize_Impl)(void);
    BPL_Status_t (*SendEvent_Impl)(uint16_t EventID, BPL_EM_EventType_t EventType,
                                   char const * EventText, va_list EventTextArgPtr);
} BPL_EM_ProxyCallbacks_t;

/************************************************
 * Exported Functions
 ************************************************/

BPL_Status_t BPL_EM_Initialize(BPL_EM_ProxyCallbacks_t ProxyCallbacks);
char const * BPL_EM_EventTypeToString(BPL_EM_EventType_t Type);
BPL_Status_t BPL_EM_SendEvent(uint16_t EventID, BPL_EM_EventType_t EventType,
                                char const * EventText, ...);
void BPL_EM_Deinitialize(void);

#endif /* BPL_EM_H */
