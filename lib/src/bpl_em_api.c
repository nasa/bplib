/*
 * TODO: Fill in file header, if necessary.
 */

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdarg.h>
#include "cfe.h"
#include "bpl_em_api.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/


/******************************************************************************
 LOCAL DATA
 ******************************************************************************/

BPL_EM_ProxyCallbacks_t BPL_EM_ProxyCallbacks;

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

BPL_Status_t BPL_EM_Initialize(BPL_EM_ProxyCallbacks_t ProxyCallbacks)
{
    BPL_Status_t ReturnStatus;
    BPL_Status_t ProxyInitImplReturnStatus;

    if ((ProxyCallbacks.Initialize_Impl == NULL) || (ProxyCallbacks.SendEvent_Impl == NULL))
    {
        ReturnStatus.ReturnValue = BPL_STATUS_ERROR_INPUT_INVALID;
        OS_printf("BPL_EM_Initialize got an invalid argument!\n");
    }
    else
    {
        /* impl callbacks determined to be valid */
        BPL_EM_ProxyCallbacks.Initialize_Impl = ProxyCallbacks.Initialize_Impl;
        BPL_EM_ProxyCallbacks.SendEvent_Impl = ProxyCallbacks.SendEvent_Impl;

        /* TODO: immediately want to call the proxy init, or wait for a directive to do so? */
        ProxyInitImplReturnStatus = BPL_EM_ProxyCallbacks.Initialize_Impl();
        if (ProxyInitImplReturnStatus.ReturnValue != BPL_STATUS_SUCCESS)
        {
            ReturnStatus.ReturnValue = BPL_STATUS_ERROR_PROXY_INIT;
            OS_printf("BPL_EM_Initialize hit error (%u) when calling proxy init!\n",
                ProxyInitImplReturnStatus.ReturnValue);
        }
        else
        {
            ReturnStatus.ReturnValue = BPL_STATUS_SUCCESS;
        }
    }

    return ReturnStatus;
}

char const * BPL_EM_EventTypeToString(BPL_EM_EventType_t Type)
{
    /* BPL_EM_EventTypeStrings should always match BPL_EM_EventType_t. */
    static char const * BPL_EM_EventTypeStrings[] = {
        "UNKNOWN",
        "DEBUG",
        "INFO",
        "WARNING",
        "ERROR",
        "CRITICAL"
    };

    switch (Type)
    {
        case BPL_EM_EventType_DEBUG:
            return BPL_EM_EventTypeStrings[1];
        case BPL_EM_EventType_INFO:
            return BPL_EM_EventTypeStrings[2];
        case BPL_EM_EventType_WARNING:
            return BPL_EM_EventTypeStrings[3];
        case BPL_EM_EventType_ERROR:
            return BPL_EM_EventTypeStrings[4];
        case BPL_EM_EventType_CRITICAL:
            return BPL_EM_EventTypeStrings[5];
        default:
            /* This default case also captures the BPL_EM_EventType_UNKNOWN case. */
            return BPL_EM_EventTypeStrings[0];
    }
}

BPL_Status_t BPL_EM_SendEvent(uint16_t EventID, BPL_EM_EventType_t EventType,
    char const * EventText, ...)
{
    BPL_Status_t ReturnStatus;
    BPL_Status_t ProxyReturnStatus;
    va_list EventTextArgsPtr;

    if (BPL_EM_ProxyCallbacks.SendEvent_Impl == NULL)
    {
        ReturnStatus.ReturnValue = BPL_STATUS_ERROR_PROXY_INIT;
    }
    else if (EventText == NULL)
    {
        ReturnStatus.ReturnValue = BPL_STATUS_ERROR_INPUT_INVALID;
    }
    else
    {
        va_start(EventTextArgsPtr, EventText);
        ProxyReturnStatus = BPL_EM_ProxyCallbacks.SendEvent_Impl(EventID, EventType, EventText, EventTextArgsPtr);
        va_end(EventTextArgsPtr);

        if (ProxyReturnStatus.ReturnValue != BPL_STATUS_SUCCESS)
        {
            ReturnStatus.ReturnValue = BPL_STATUS_ERROR_GENERAL;
            OS_printf("BPL_EM_SendEvent hit error (%u) when calling proxy impl!\n",
                ProxyReturnStatus.ReturnValue);
        }
        else
        {
            ReturnStatus.ReturnValue = BPL_STATUS_SUCCESS;
        }
    }

    return ReturnStatus;
}

void BPL_EM_Deinitialize(void)
{
    /* Clear proxy function pointers */
    BPL_EM_ProxyCallbacks.Initialize_Impl = NULL;
    BPL_EM_ProxyCallbacks.SendEvent_Impl = NULL;

    return;
}
