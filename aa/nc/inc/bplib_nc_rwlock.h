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
#ifndef BPLIB_NC_RWLOCK_H
#define BPLIB_NC_RWLOCK_H

#include "bplib_api_types.h"

#include <pthread.h>

/**
 * \brief Definition of the read-write lock structure.
 *
 * \details This structure defines the necessary mutex and condition variables
 * to implement a read-write lock. It includes the mutex, the condition variables
 * for readers and writers, and counters for the number of readers and writers.
 */
typedef struct BPLib_NC_RWLock {
    pthread_mutex_t Lock;          /**< Mutex to protect shared data */
    pthread_cond_t ReadCond;       /**< Condition variable for readers */
    pthread_cond_t WriteCond;      /**< Condition variable for writers */
    int ReaderCnt;                 /**< Number of active readers */
    int WriterCnt;               /**< Number of active writers */
} BPLib_NC_RWLock_t;

/**
 * \brief Initializes a read-write lock.
 *
 * \param[out] RWLock Pointer to the read-write lock to be initialized.
 * \return Execution status.
 * \retval BPLIB_SUCCESS: Initialization was successful.
 * \retval BPLIB_OS_ERROR: Initialization failed.
 * 
 * \details This function initializes the mutex and condition variables for
 * the read-write lock, allowing it to be used for synchronization.
 */
BPLib_Status_t BPLib_NC_RWLock_Init(BPLib_NC_RWLock_t *RWLock);

/**
 * \brief Destroys a read-write lock.
 *
 * \param[in] RWLock Pointer to the read-write lock to be destroyed.
 * \return None.
 * 
 * \details This function cleans up and destroys the read-write lock, releasing
 * any resources associated with it.
 */
void BPLib_NC_RWLock_Destroy(BPLib_NC_RWLock_t *RWLock);

/**
 * \brief Acquires the read lock on the given read-write lock.
 *
 * \param[in] RWLock Pointer to the read-write lock to acquire the read lock for.
 * \return None.
 * 
 * \details This function acquires the read lock for the given read-write lock.
 * It allows multiple threads to read the shared data as long as no writer is
 * holding the lock.
 */
void BPLib_NC_RWLock_RLock(BPLib_NC_RWLock_t *RWLock);

/**
 * \brief Releases the read lock on the given read-write lock.
 *
 * \param[in] RWLock Pointer to the read-write lock to release the read lock for.
 * \return None.
 * 
 * \details This function releases the read lock on the given read-write lock.
 * Once the read lock is released, other readers or writers can acquire the lock.
 */
void BPLib_NC_RWLock_RUnlock(BPLib_NC_RWLock_t *RWLock);

/**
 * \brief Acquires the write lock on the given read-write lock.
 *
 * \param[in] RWLock Pointer to the read-write lock to acquire the write lock for.
 * \return None.
 * 
 * \details This function acquires the write lock for the given read-write lock.
 * It blocks other readers or writers from accessing the shared data while the
 * writer lock is held.
 */
void BPLib_NC_RWLock_WLock(BPLib_NC_RWLock_t *RWLock);

/**
 * \brief Releases the write lock on the given read-write lock.
 *
 * \param[in] RWLock Pointer to the read-write lock to release the write lock for.
 * \return None.
 * 
 * \details This function releases the write lock on the given read-write lock,
 * allowing other readers or writers to acquire the lock.
 */
void BPLib_NC_RWLock_WUnlock(BPLib_NC_RWLock_t *RWLock);

#endif /* BPLIB_NC_RWLOCK */
