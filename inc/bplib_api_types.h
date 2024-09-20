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

#include <stdint.h>
#include <stddef.h>

/*
** Type Definitions
*/

/**
 * \brief BPLib status type for type safety
 */
typedef int32_t BPLib_Status_t;


/*
** Macros
*/

/* General Return Codes */
#define BPLIB_SUCCESS                       ((BPLib_Status_t)   0)
#define BPLIB_UNIMPLEMENTED                 ((BPLib_Status_t)  -1)
#define BPLIB_ERROR                         ((BPLib_Status_t)  -2)

/* Framework Proxy Errors */
#define BPLIB_FWP_CALLBACK_INIT_ERROR       ((BPLib_Status_t)  -3)

/* Time Management Errors */
#define BPLIB_TIME_UNDEF_DELTA_ERROR        ((BPLib_Status_t)  -4)
#define BPLIB_TIME_WRITE_ERROR              ((BPLib_Status_t)  -5)
#define BPLIB_TIME_READ_ERROR               ((BPLib_Status_t)  -6)
#define BPLIB_TIME_UNINIT_ERROR             ((BPLib_Status_t)  -7)

/* PerfLog Proxy Errors*/
#define BPLIB_PL_NULL_CALLBACK_ERROR    ((BPLib_Status_t) -8)

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* BPLIB_API_TYPES_H */
