/*
 * TODO: Fill in file header, if necessary.
 */

#ifndef BPL_EVM_H
#define BPL_EVM_H

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_api_types.h"

/************************************************
 * Typedefs
 ************************************************/

typedef enum
{
    BPL_EVM_EventType_UNKNOWN = 0,
    BPL_EVM_EventType_DEBUG = 1,
    BPL_EVM_EventType_INFO = 2,
    BPL_EVM_EventType_WARNING = 3,
    BPL_EVM_EventType_ERROR = 4,
    BPL_EVM_EventType_CRITICAL = 5
} BPL_EVM_EventType_t;

typedef struct
{
    BPL_EVM_EventType_t Type;
    uint16_t ID;
} BPL_EVM_EventInfo_t;

typedef struct
{
    uint32_t ReturnValue;
} BPL_Status_t;

/************************************************
 * Exported Functions
 ************************************************/

BPL_Status_t BPL_EVM_Initialize(void);
char const * BPL_EVM_EventTypeToString(BPL_EVM_EventType_t Type);
BPL_Status_t BPL_EVM_SendEvent(BPL_EVM_EventInfo_t const * EventInfo, char const * EventText, ...);

#endif /* BPL_EVM_H */
