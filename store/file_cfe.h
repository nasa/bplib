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

#ifndef FILE_CFE_H
#define FILE_CFE_H

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdio.h>
#include "cfe.h"

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

/*
 * WARNING:
 * to make this appear like/compatible with the C stdio prototypes, FILE* is used, but the pointer
 * should not be ever passed to the actual C library implementation (it is not a file pointer)
 */
typedef FILE *bp_file_t;

bp_file_t bp_cfe_fopen(const char *filename, const char *mode);
int       bp_cfe_fclose(bp_file_t stream);
size_t    bp_cfe_fread(void *ptr, size_t size, size_t count, bp_file_t stream);
size_t    bp_cfe_fwrite(const void *ptr, size_t size, size_t count, bp_file_t stream);
size_t    bp_cfe_fwrite(const void *ptr, size_t size, size_t count, bp_file_t stream);
int       bp_cfe_fseek(bp_file_t stream, long offset, int origin);
int       bp_cfe_fflush(bp_file_t stream);

#endif /* FILE_CFE_H */
