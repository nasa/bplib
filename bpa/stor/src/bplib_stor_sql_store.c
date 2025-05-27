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
#include "bplib_stor_sql.h"
#include "bplib_qm.h"

#include <stdio.h>

/*******************************************************************************
* SQL Query Definitions
*/

/* Insert Bundle Metadata */
static const char* InsertMetadataSQL = 
    "INSERT INTO bundle_data (action_timestamp, dest_node, dest_service) VALUES (?, ?, ?);";
static sqlite3_stmt* InsertMetadataStmt;

/* Insert Bundle Blob */
const char* InsertBlobSQL = 
    "INSERT INTO bundle_blobs (bundle_id, blob_data) VALUES (?, ?)";
static sqlite3_stmt* InsertBlobStmt;

/*******************************************************************************
** Static Functions
*/
static int BPLib_SQL_StoreMetadata(BPLib_BBlocks_t* BBlocks)
{
    int SQLStatus;

    sqlite3_reset(InsertMetadataStmt);
    SQLStatus = sqlite3_bind_int64(InsertMetadataStmt, 1, (int64_t)BBlocks->PrimaryBlock.Timestamp.CreateTime + 
        (int64_t)BBlocks->PrimaryBlock.Lifetime);
    if (SQLStatus != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind action_timestamp in store_meta\n");
        return SQLStatus;
    }
    SQLStatus = sqlite3_bind_int64(InsertMetadataStmt, 2, (int64_t)BBlocks->PrimaryBlock.DestEID.Node);
    if (SQLStatus != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind dest_node in store_meta\n");
        return SQLStatus;
    }
    SQLStatus = sqlite3_bind_int64(InsertMetadataStmt, 3, (int64_t)BBlocks->PrimaryBlock.DestEID.Service);
    if (SQLStatus != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind dest_service in store_meta\n");
        return SQLStatus;
    }

    SQLStatus = sqlite3_step(InsertMetadataStmt);
    if (SQLStatus != SQLITE_DONE)
    {
        fprintf(stderr, "Insert meta failed\n");
        return SQLStatus;
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
        fprintf(stderr, "Insert chunk failed\n");
        return SQLStatus;
    }

    return SQLStatus;
}

static int BPLib_SQL_StoreBundle(sqlite3* db, BPLib_Bundle_t* Bundle)
{
    int SQLStatus;
    int BundleRowID;
    BPLib_MEM_Block_t* CurrMemBlock;

    /* Store the indexable metadata */
    SQLStatus = BPLib_SQL_StoreMetadata(&Bundle->blocks);
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

static int BPLib_SQL_StoreImpl(BPLib_Instance_t* Inst)
{
    int SQLStatus;
    int i;
    sqlite3* db = Inst->BundleStorage.db;

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
        SQLStatus = BPLib_SQL_StoreBundle(db, Inst->BundleStorage.InsertBatch[i]);
        if (SQLStatus != SQLITE_DONE)
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
    if (SQLStatus != SQLITE_OK)
    {
        fprintf(stderr, "Attempting ROLLBACK\n");
        SQLStatus = sqlite3_exec(db, "ROLLBACK;", 0, 0, 0);
        if (SQLStatus != SQLITE_OK)
        {
            fprintf(stderr, "Failed to rollback transaction\n");
        }
    }

    /* Expecting SQLITE_OK */
    return SQLStatus;
}

/*******************************************************************************
** Exported Functions
*/
BPLib_Status_t BPLib_SQL_Store(BPLib_Instance_t* Inst)
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
        SQLStatus = BPLib_SQL_StoreImpl(Inst);
        if (SQLStatus != SQLITE_OK)
        {
            Status = BPLIB_STOR_SQL_STORAGE_ERR;
        }
    }

    /* Finalize */
    sqlite3_finalize(InsertMetadataStmt);
    sqlite3_finalize(InsertBlobStmt);

    return Status;
}
