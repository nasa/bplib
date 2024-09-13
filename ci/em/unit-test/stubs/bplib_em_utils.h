#ifndef BPLIB_EM_UTILS_H
#define BPLIB_EM_UTILS_H

/* Macro to get expected event name */
#define BPLIB_EM_UT_CHECKEVENT_SETUP(Evt, ExpectedEvent, ExpectedFormat) \
    BPLib_EM_UT_CheckEvent_Setup_Impl(Evt, ExpectedEvent, #ExpectedEvent, ExpectedFormat)

#endif // BPLIB_EM_UTILS_H