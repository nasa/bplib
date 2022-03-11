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

#ifndef SOCK_H
#define SOCK_H

/******************************************************************************
 * Defines
 ******************************************************************************/

#define SOCK_PEND    (-1)
#define SOCK_CHECK   0
#define SOCK_TIMEOUT 1000 // milliseconds
#define SOCK_INVALID (-1)

/******************************************************************************
 * Exported Functions
 ******************************************************************************/

int  sockcreate(int type, const char *ip_addr, int port, int is_server, int *block);
int  sockstream(const char *ip_addr, int port, int is_server, int *block);
int  sockdatagram(const char *ip_addr, int port, int is_server, int *block);
int  socksend(int fd, const void *buf, int size, int timeout);
int  sockrecv(int fd, void *buf, int size, int timeout);
void sockclose(int fd);

#endif /* SOCK_H */
