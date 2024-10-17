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

/**
 * \file
 *
 * BPLib coverage stub replacement for stdlib.h
 */

#ifndef OVERRIDE_STDLIB_H
#define OVERRIDE_STDLIB_H

#include "bplib_cs_stdlib.h"

/* ----------------------------------------- */
/* mappings for declarations in stdlib.h */
/* ----------------------------------------- */

#define calloc BPLIB_CS_calloc
#define free   BPLIB_CS_free

#endif /* OVERRIDE_STDLIB_H */
