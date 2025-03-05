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

#ifndef BPLIB_QM_HANDLERS_H
#define BPLIB_QM_HANDLERS_H

/*
** Include 
*/

#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"

#include "bplib_qm.h"


/*
** Function Definitions
*/

void UT_Handler_BPLib_QM_WaitQueueTryPull(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);

#endif /* BPLIB_QM_HANDLERS_H */
