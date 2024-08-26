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

#ifndef BPLIB_EM_H
#define BPLIB_EM_H

/*
** Include
*/

#include "bplib.h"


/*
** Exported Functions
*/

/**
 * \brief Events Management initialization
 *
 *  \par Description
 *       EM initialization function
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS - Initialization was successful
 */
BPLib_Status_t BPLib_EM_Init(void);

/**
 * \brief Convert event indicators from an integer type to a string
 *
 *  \par Description
 *       Take in an integer-type and translate it into a string
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 * 
 *  \param[in] Type Integer value of an event type
 *
 *  \return String representation of give event type
 */
/* const char* */ void BPL_EM_EventTypeToString(BPL_EM_EventType_t Type);

#endif /* BPLIB_EM_H */
