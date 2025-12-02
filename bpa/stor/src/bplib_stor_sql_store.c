/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking 
 * (DTN) Bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this 
 * file except in compliance with the License. You may obtain a copy of the License at 
 *
 * http://www.apache.org/licenses/LICENSE-2.0 
 *
 * Unless required by applicable law or agreed to in writing, software distributed under 
 * the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF 
 * ANY KIND, either express or implied. See the License for the specific language 
 * governing permissions and limitations under the License. The copyright notice to be 
 * included in the software is as follows: 
 *
 * Copyright 2025 United States Government as represented by the Administrator of the 
 * National Aeronautics and Space Administration. All Rights Reserved.
 *
 */
#include "bplib_stor_sql.h"
#include "bplib_qm.h"
#include "bplib_as.h"

#include <stdio.h>

/*******************************************************************************
* SQL Query Definitions
*/

/* Insert Bundle Metadata */
static const char* InsertMetadataSQL = 
    "INSERT INTO bundle_data (action_timestamp, dest_node, dest_service, bundle_bytes) VALUES (?, ?, ?, ?);";
static sqlite3_stmt* InsertMetadataStmt;

/* Insert Bundle Blob */
const char* InsertBlobSQL = 
    "INSERT INTO bundle_blobs (bundle_id, blob_data) VALUES (?, ?)";
static sqlite3_stmt* InsertBlobStmt;

/*******************************************************************************
** Static Functions
*/
static int BPLib_SQL_StoreMetadata(BPLib_Bundle_t* Bundle, BPLib_BundleCache_t* BundleCache)
{
    int SQLStatus;

    sqlite3_reset(InsertMetadataStmt);

    /* Add the value of the timestamp used as an indicator for some action to the InsertMetadataStmt variable */
    SQLStatus = sqlite3_bind_int64(InsertMetadataStmt, 1, (int64_t)Bundle->blocks.PrimaryBlock.Timestamp.CreateTime + 
                                                          (int64_t)Bundle->blocks.PrimaryBlock.Lifetime);

    if (SQLStatus == SQLITE_OK)
    {
        /* Add the destination node into the InsertMetadataStmt variable */
        SQLStatus = sqlite3_bind_int64(InsertMetadataStmt, 2, (int64_t)Bundle->blocks.PrimaryBlock.DestEID.Node);
        if (SQLStatus == SQLITE_OK)
        {
            /* Add the destination service number to the InsertMetadataStmt variable */
            SQLStatus = sqlite3_bind_int64(InsertMetadataStmt, 3, (int64_t)Bundle->blocks.PrimaryBlock.DestEID.Service);
            if (SQLStatus == SQLITE_OK)
            {
                /* Add the bundle size in bytes to the InsertMetadataStmt variable */
                SQLStatus = sqlite3_bind_int64(InsertMetadataStmt, 4, (int64_t)Bundle->Meta.TotalBytes);
                if (SQLStatus == SQLITE_OK)
                {
                    SQLStatus = sqlite3_step(InsertMetadataStmt);
                }
                else
                {
                    fprintf(stderr, "Failed to bind bundle_size in store_meta\n");
                }
            }
            else
            {
                fprintf(stderr, "Failed to bind dest_service in store_meta\n");
            }
        }
        else
        {
            fprintf(stderr, "Failed to bind dest_node in store_meta\n");
        }
    }
    else
    {
        fprintf(stderr, "Failed to bind action_timestamp in store_meta\n");
    }

    /* Expecting SQLITE_DONE */
    return SQLStatus;
}

static int BPLib_SQL_StoreChunk(int64_t BundleRowID, const void* Chunk, size_t ChunkSize)
{
    int SQLStatus;

    sqlite3_reset(InsertBlobStmt);
    sqlite3_bind_int64(InsertBlobStmt, 1, BundleRowID);
    sqlite3_bind_blob(InsertBlobStmt, 2, Chunk, ChunkSize, SQLITE_STATIC);

    SQLStatus = sqlite3_step(InsertBlobStmt);
    if (SQLStatus != SQLITE_DONE)
    {
        return SQLStatus;
    }

    return SQLStatus;
}

static int BPLib_SQL_StoreBundle(sqlite3* db, BPLib_Bundle_t* Bundle, BPLib_BundleCache_t* BundleCache)
{
    int SQLStatus;
    int BundleRowID;
    BPLib_MEM_Block_t* CurrMemBlock;

    /* Store the indexable metadata */
    SQLStatus = BPLib_SQL_StoreMetadata(Bundle, BundleCache);
    if (SQLStatus != SQLITE_DONE)
    {
        return SQLStatus;
    }
    BundleRowID = sqlite3_last_insert_rowid(db);

    /* Store the decoded metadata block */
    SQLStatus = BPLib_SQL_StoreChunk(BundleRowID, (const void*)&Bundle->blocks, sizeof(BPLib_BBlocks_t));
    if (SQLStatus != SQLITE_DONE)
    {
        return SQLStatus;
    }

    /* Store the blob chunks */
    CurrMemBlock = Bundle->blob;
    while (CurrMemBlock != NULL)
    {
        SQLStatus = BPLib_SQL_StoreChunk(BundleRowID, (const void*)CurrMemBlock->user_data.raw_bytes,
            CurrMemBlock->used_len);
        if (SQLStatus != SQLITE_DONE)
        {
            return SQLStatus;
        }
        CurrMemBlock = CurrMemBlock->next;
    }

    /* Expecting SQLITE_DONE */
    return SQLStatus;
}

static int BPLib_SQL_StoreImpl(BPLib_Instance_t* Inst, size_t *TotalBytesStored)
{
    int SQLStatus;
    int i;
    sqlite3* db = Inst->BundleStorage.db;
    size_t NewBundleBytes;

    /* Create a batch query */
    SQLStatus = sqlite3_exec(db, "BEGIN;", 0, 0, 0);
    if (SQLStatus != SQLITE_OK)
    {
        fprintf(stderr, "Failed to start transaction: %s\n", sqlite3_errmsg(db));
        return SQLStatus;
    }

    /* Perform an insert for every bundle */
    for (i = 0; i < Inst->BundleStorage.InsertBatchSize; i++)
    {
        /* Check that inserting the bundle won't cause the storage limit to be exceeded */
        NewBundleBytes = Inst->BundleStorage.InsertBatch[i]->Meta.TotalBytes;
        if (Inst->BundleStorage.BytesStorageInUse + *TotalBytesStored + NewBundleBytes > BPLIB_MAX_STORED_BUNDLE_BYTES)
        {
            SQLStatus = SQLITE_FULL;
            break;
        }
        
        SQLStatus = BPLib_SQL_StoreBundle(db, Inst->BundleStorage.InsertBatch[i], &(Inst->BundleStorage));
        if (SQLStatus == SQLITE_DONE)
        {
            *TotalBytesStored += NewBundleBytes;
        }
        else
        {
            /* If there was an error, don't keep trying to construsct the SQL INSERT */
            break;
        }
    }

    /* If there have been no errors so far, batch-write the data to persistent storage */
    if (SQLStatus == SQLITE_DONE)
    {
        SQLStatus = sqlite3_exec(db, "COMMIT;", 0, 0, 0);
        if (SQLStatus != SQLITE_OK)
        {
            fprintf(stderr, "Failed to commit transaction\n");
        }
    }

    /* The batch commit was not successful, ROLLBACK to prevent DB corruption */
    if (SQLStatus == SQLITE_FULL)
    {
        /* Don't want to override this error code, assume rollback works */
        (void) sqlite3_exec(db, "ROLLBACK;", 0, 0, 0);

    }
    else if (SQLStatus != SQLITE_OK)
    {
        fprintf(stderr, "Batch commit failed, RC=%d\n", SQLStatus);
        SQLStatus = sqlite3_exec(db, "ROLLBACK;", 0, 0, 0);
        if (SQLStatus != SQLITE_OK)
        {
            fprintf(stderr, "Failed to rollback transaction, RC=%d\n", SQLStatus);
        }
    }

    /* Expecting SQLITE_OK */
    return SQLStatus;
}

/*******************************************************************************
** Exported Functions
*/
BPLib_Status_t BPLib_SQL_Store(BPLib_Instance_t* Inst, size_t *TotalBytesStored)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;
    int SQLStatus;
    sqlite3* db = Inst->BundleStorage.db;

    /* Prepare Insert Statements needed for this batch query */
    SQLStatus = sqlite3_prepare_v2(db, InsertMetadataSQL, -1, &InsertMetadataStmt, 0);
    if (SQLStatus != SQLITE_OK)
    {
        Status = BPLIB_STOR_SQL_STORAGE_ERR;
    }
    SQLStatus = sqlite3_prepare_v2(db, InsertBlobSQL, -1, &InsertBlobStmt, 0);
    if (SQLStatus != SQLITE_OK)
    {
        Status = BPLIB_STOR_SQL_STORAGE_ERR;
    }

    if (Status == BPLIB_SUCCESS)
    {
        /* Run the batch storage logic */
        SQLStatus = BPLib_SQL_StoreImpl(Inst, TotalBytesStored);
        if (SQLStatus == SQLITE_FULL)
        {
            Status = BPLIB_STOR_DB_FULL_ERR;
        }
        else if (SQLStatus != SQLITE_OK)
        {
            Status = BPLIB_STOR_SQL_STORAGE_ERR;
        }
    }

    /* Finalize */
    sqlite3_finalize(InsertMetadataStmt);
    sqlite3_finalize(InsertBlobStmt);

    return Status;
}
