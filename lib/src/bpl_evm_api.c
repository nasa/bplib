/*
 * TODO: Fill in file header, if necessary.
 */

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "cfe.h"
#include "bpl_evm_api.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/



/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

BPL_Status_t BPL_EVM_Initialize(void)
{
    BPL_Status_t ReturnStatus = { .ReturnValue = 0 };
    OS_printf("BPL_EVM_Initialize called!\n");
    return ReturnStatus;
}

char const * BPL_EVM_EventTypeToString(BPL_EVM_EventType_t Type)
{
    /* BPL_EVM_EventTypeStrings should always match BPL_EVM_EventType_t. */
    static char const * BPL_EVM_EventTypeStrings[] = {
        "UNKNOWN",
        "DEBUG",
        "INFO",
        "WARNING",
        "ERROR",
        "CRITICAL"
    };

    switch (Type)
    {
        case BPL_EVM_EventType_DEBUG:
            return BPL_EVM_EventTypeStrings[1];
        case BPL_EVM_EventType_INFO:
            return BPL_EVM_EventTypeStrings[2];
        case BPL_EVM_EventType_WARNING:
            return BPL_EVM_EventTypeStrings[3];
        case BPL_EVM_EventType_ERROR:
            return BPL_EVM_EventTypeStrings[4];
        case BPL_EVM_EventType_CRITICAL:
            return BPL_EVM_EventTypeStrings[5];
        default:
            /* This default case also captures the BPL_EVM_EventType_UNKNOWN case. */
            return BPL_EVM_EventTypeStrings[0];
    }
}

BPL_Status_t BPL_EVM_SendEvent(BPL_EVM_EventInfo_t const * EventInfo, char const * EventText, ...)
{
    BPL_Status_t ReturnStatus = { .ReturnValue = 0 };
    char const * EventTypeString = BPL_EVM_EventTypeToString(EventInfo->Type);
    OS_printf("BPL_EVM_SendEvent called! Event Info (%s, %u).\n",
        EventTypeString,
        EventInfo->ID);
    return ReturnStatus;
}
