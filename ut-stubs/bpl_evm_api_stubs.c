/*
 * TODO: Fill in file header, if necessary.
 */

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in bpl_evm_api header
 */

#include <stdarg.h>

#include "bpl_evm_api.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPL_EVM_Deinitialize()
 * ----------------------------------------------------
 */
void BPL_EVM_Deinitialize(void)
{

    UT_GenStub_Execute(BPL_EVM_Deinitialize, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPL_EVM_EventTypeToString()
 * ----------------------------------------------------
 */
char const *BPL_EVM_EventTypeToString(BPL_EVM_EventType_t Type)
{
    UT_GenStub_SetupReturnBuffer(BPL_EVM_EventTypeToString, char const *);

    UT_GenStub_AddParam(BPL_EVM_EventTypeToString, BPL_EVM_EventType_t, Type);

    UT_GenStub_Execute(BPL_EVM_EventTypeToString, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPL_EVM_EventTypeToString, char const *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPL_EVM_Initialize()
 * ----------------------------------------------------
 */
BPL_Status_t BPL_EVM_Initialize(BPL_EVM_ProxyCallbacks_t ProxyCallbacks)
{
    UT_GenStub_SetupReturnBuffer(BPL_EVM_Initialize, BPL_Status_t);

    UT_GenStub_AddParam(BPL_EVM_Initialize, BPL_EVM_ProxyCallbacks_t, ProxyCallbacks);

    UT_GenStub_Execute(BPL_EVM_Initialize, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPL_EVM_Initialize, BPL_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPL_EVM_SendEvent()
 * ----------------------------------------------------
 */
BPL_Status_t BPL_EVM_SendEvent(uint16_t EventID, BPL_EVM_EventType_t EventType, char const *EventText, ...)
{
    va_list UtStub_ArgList;

    UT_GenStub_SetupReturnBuffer(BPL_EVM_SendEvent, BPL_Status_t);

    UT_GenStub_AddParam(BPL_EVM_SendEvent, uint16_t, EventID);
    UT_GenStub_AddParam(BPL_EVM_SendEvent, BPL_EVM_EventType_t, EventType);
    UT_GenStub_AddParam(BPL_EVM_SendEvent, char const *, EventText);

    va_start(UtStub_ArgList, EventText);
    UT_GenStub_Execute(BPL_EVM_SendEvent, Va, NULL, UtStub_ArgList);
    va_end(UtStub_ArgList);

    return UT_GenStub_GetReturnValue(BPL_EVM_SendEvent, BPL_Status_t);
}
