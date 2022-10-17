/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/*
 * Includes
 */
#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"

#include "bplib_os.h"
#include "bplib_cs_stdlib.h"
#include "osapi-error.h"
#include "osapi-clock.h"

static void UT_OS_GetTime_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    OS_time_t *time_struct = UT_Hook_GetArgValueByName(Context, "time_struct", OS_time_t *);
    int32_t    retval;

    *time_struct = OS_TimeAssembleFromMilliseconds(1000000000, 496);
    retval       = OS_SUCCESS;

    UT_Stub_SetReturnValue(FuncKey, retval);
}

/*--------------------------------------------------------------------------------------
 * bplib_os_init -
 *-------------------------------------------------------------------------------------*/
void test_bplib_os_init(void)
{
    /* Test function for:
     * void bplib_os_init(void)
     */

    UtAssert_VOIDCALL(bplib_os_init());
}

void test_bplib_os_sleep(void)
{
    /* Test function for:
     * void bplib_os_sleep(int seconds)
     */

    UtAssert_VOIDCALL(bplib_os_sleep(1));
}

void test_bplib_os_createlock(void)
{
    /* Test function for:
     * bp_handle_t bplib_os_createlock(void)
     */
    bp_handle_t h = bp_handle_from_serial(100, BPLIB_HANDLE_OS_BASE);

    UtAssert_VOIDCALL(h = bplib_os_createlock());
    UtAssert_BOOL_TRUE(bp_handle_is_valid(h));
}

void test_bplib_os_destroylock(void)
{
    /* Test function for:
     * void bplib_os_destroylock(bp_handle_t h)
     */
    bp_handle_t h = bp_handle_from_serial(100, BPLIB_HANDLE_OS_BASE);

    UtAssert_VOIDCALL(bplib_os_destroylock(h));
}

void test_bplib_os_lock(void)
{
    /* Test function for:
     * void bplib_os_lock(bp_handle_t h)
     */
    bp_handle_t h = bp_handle_from_serial(100, BPLIB_HANDLE_OS_BASE);

    UtAssert_VOIDCALL(bplib_os_lock(h));
}

void test_bplib_os_unlock(void)
{
    /* Test function for:
     * void bplib_os_unlock(bp_handle_t h)
     */
    bp_handle_t h = bp_handle_from_serial(100, BPLIB_HANDLE_OS_BASE);

    UtAssert_VOIDCALL(bplib_os_unlock(h));
}

void test_bplib_os_broadcast_signal_and_unlock(void)
{
    /* Test function for:
     * void bplib_os_broadcast_signal_and_unlock(bp_handle_t h)
     */
    bp_handle_t h = bp_handle_from_serial(100, BPLIB_HANDLE_OS_BASE);

    UtAssert_VOIDCALL(bplib_os_broadcast_signal_and_unlock(h));
}

void test_bplib_os_broadcast_signal(void)
{
    /* Test function for:
     * void bplib_os_broadcast_signal(bp_handle_t h)
     */
    bp_handle_t h = bp_handle_from_serial(100, BPLIB_HANDLE_OS_BASE);

    UtAssert_VOIDCALL(bplib_os_broadcast_signal(h));
}

void test_bplib_os_signal(void)
{
    /* Test function for:
     * void bplib_os_signal(bp_handle_t h)
     */
    bp_handle_t h = bp_handle_from_serial(100, BPLIB_HANDLE_OS_BASE);

    UtAssert_VOIDCALL(bplib_os_signal(h));
}

void test_bplib_os_wait_until_ms(void)
{
    /* Test function for:
     * int bplib_os_wait_until_ms(bp_handle_t h, uint64_t abs_dtntime_ms)
     */
    bp_handle_t h = bp_handle_from_serial(100, BPLIB_HANDLE_OS_BASE);

    UtAssert_VOIDCALL(bplib_os_wait_until_ms(h, 500000000));
}

void test_bplib_os_get_dtntime_ms(void)
{
    /* Test function for:
     * uint64_t bplib_os_get_dtntime_ms(void)
     */
    UT_SetHandlerFunction(UT_KEY(OS_GetLocalTime), UT_OS_GetTime_Handler, NULL);
    UtAssert_UINT32_EQ(bplib_os_get_dtntime_ms(), 1775592944);
}

void test_bplib_os_calloc_free(void)
{
    /* Test function for:
     * void *bplib_os_calloc(size_t size)
     * void bplib_os_free(void *ptr)
     */
    uint32 buffer[16];
    void  *p;

    UT_SetDataBuffer(UT_KEY(BPLIB_CS_calloc), buffer, sizeof(buffer), false);
    UtAssert_ADDRESS_EQ(p = bplib_os_calloc(sizeof(buffer)), buffer);
    UtAssert_VOIDCALL(bplib_os_free(p));
}

void UtTest_Setup(void)
{
    UtTest_Add(test_bplib_os_init, NULL, NULL, "bplib_os_init");
    UtTest_Add(test_bplib_os_sleep, NULL, NULL, "bplib_os_sleep");
    UtTest_Add(test_bplib_os_createlock, NULL, NULL, "bplib_os_createlock");
    UtTest_Add(test_bplib_os_destroylock, NULL, NULL, "bplib_os_destroylock");
    UtTest_Add(test_bplib_os_lock, NULL, NULL, "bplib_os_lock");
    UtTest_Add(test_bplib_os_unlock, NULL, NULL, "bplib_os_unlock");
    UtTest_Add(test_bplib_os_broadcast_signal_and_unlock, NULL, NULL, "bplib_os_broadcast_signal_and_unlock");
    UtTest_Add(test_bplib_os_broadcast_signal, NULL, NULL, "bplib_os_broadcast_signal");
    UtTest_Add(test_bplib_os_signal, NULL, NULL, "bplib_os_signal");
    UtTest_Add(test_bplib_os_wait_until_ms, NULL, NULL, "bplib_os_wait_until_ms");
    UtTest_Add(test_bplib_os_get_dtntime_ms, NULL, NULL, "bplib_os_get_dtntime_ms");
    UtTest_Add(test_bplib_os_calloc_free, NULL, NULL, "bplib_os_calloc/free");
}
