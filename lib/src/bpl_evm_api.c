/*
 * TODO: Fill in file header, if necessary.
 */

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "cfe.h"
// #include "bplib.h"
// #include "bplib_os.h"
// #include "v7.h"
// #include "bplib_dataservice.h"
// #include "v7_base_internal.h"
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

BPL_Status_t BPL_EVM_SendEvent(BPL_EVM_EventInfo_t const * EventInfo, char const * EventText, ...)
{
    BPL_Status_t ReturnStatus = { .ReturnValue = 0 };
    // char * EventTypeString = BPL_EVM_EventTypeToString(EventInfo.Type);
    OS_printf("BPL_EVM_SendEvent called! Event Info (...).\n");
    // OS_printf("BPL_EVM_SendEvent called with arg 0x%08X!\n", EventID);
    return ReturnStatus;
}
