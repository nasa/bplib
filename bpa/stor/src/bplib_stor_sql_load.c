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
** SQL Query Definitions
*/

/* Find by Dest EID */
static const char* FindByDestNodeSQL = 
"SELECT id\n"
"FROM bundle_data\n"
"WHERE (dest_node BETWEEN ? AND ?) AND (dest_service BETWEEN ? AND ?) AND egress_attempted = 0\n"
"ORDER BY action_timestamp ASC\n"
"LIMIT ?;";
// static const char* FindByDestNodeSQL = 
// "SELECT id\n"
// "FROM bundle_data\n"
// "WHERE (dest_node BETWEEN ? AND ?) AND (dest_service BETWEEN ? AND ?)\n"
// "ORDER BY action_timestamp ASC\n"
// "LIMIT ?;";
static sqlite3_stmt* FindByDestNodeStmt;

/* Find blob by ID */
static const char* FindBlobSQL = 
"SELECT id\n"
"FROM bundle_blobs\n"
"WHERE bundle_id = ?;";
static sqlite3_stmt* FindBlobStmt;

/* Mark Egressed */
static const char* MarkEgressedSQL =
    "UPDATE bundle_data SET egress_attempted = 1 WHERE id = ?;";
static sqlite3_stmt* MarkEgressedStmt;

/*******************************************************************************
** Static Functions
*/
static int BPLib_SQL_LoadBundle(sqlite3* db, BPLib_MEM_Pool_t* Pool,
    int BundleID, BPLib_Bundle_t** Bundle)
{
    sqlite3_blob* blob = NULL;
    int SQLStatus;
    int64_t BlobRowId;
    size_t ChunkSize;
    BPLib_MEM_Block_t* BundleHead = NULL;
    BPLib_MEM_Block_t* CurrBlock = NULL;
    BPLib_MEM_Block_t* NextBlock = NULL;
    BPLib_Bundle_t* RetBundle;

    sqlite3_reset(FindBlobStmt);
    SQLStatus = sqlite3_bind_int(FindBlobStmt, 1, BundleID);
    if (SQLStatus != SQLITE_OK)
    {
        fprintf(stderr, "bind failed: %s\n", sqlite3_errmsg(db));
        return SQLStatus;
    }

    while ((SQLStatus = sqlite3_step(FindBlobStmt)) == SQLITE_ROW)
    {
        /* Determine the row ID of the blob in the bundle_blobs table */
        BlobRowId = sqlite3_column_int64(FindBlobStmt, 0);

        /* We're loading bblocks_t */
        if (BundleHead == NULL)
        {
            /* Open the metadata using the blob streaming API */
            SQLStatus = sqlite3_blob_open(db, "main", "bundle_blobs", "blob_data", BlobRowId, 0, &blob);
            if (SQLStatus != SQLITE_OK)
            {
                fprintf(stderr, "sqlite3_blob_open failed for rowid %ld: %s\n", BlobRowId, sqlite3_errmsg(db));
                break;
            }

            /* Ensure the metadata size exactly matches bblocks_t size. Otherwise, don't bother loading it */
            ChunkSize = sqlite3_blob_bytes(blob);
            if (ChunkSize != sizeof(BPLib_BBlocks_t))
            {
                fprintf(stderr, "Expected to read metadata chunk and got wrong size %lu != %lu\n",
                    ChunkSize, sizeof(BPLib_BBlocks_t));
                SQLStatus = SQLITE_CORRUPT;
                sqlite3_blob_close(blob);
                break;
            }

            /* Allocate a MEM pool block for the meta data */
            BundleHead = BPLib_MEM_BlockAlloc(Pool);
            if (BundleHead == NULL)
            {
                SQLStatus = SQLITE_IOERR;
                sqlite3_blob_close(blob);
                break;
            }

            /* Load the metadata directly into the mempool block */
            SQLStatus = sqlite3_blob_read(blob, (void*)&BundleHead->user_data.bundle.blocks, ChunkSize, 0);
            if (SQLStatus != SQLITE_OK)
            {
                fprintf(stderr, "sqlite3_blob_read failed: %s\n", sqlite3_errmsg(db));
                sqlite3_blob_close(blob);
                break;
            }

            /* Load succeeded */
            CurrBlock = BundleHead;
            CurrBlock->used_len = ChunkSize;
        }
        /* We're loading part of the blob */
        else
        {
            /* Open the metadata using the blob streaming API */
            SQLStatus = sqlite3_blob_open(db, "main", "bundle_blobs", "blob_data", BlobRowId, 0, &blob);
            if (SQLStatus != SQLITE_OK)
            {
                fprintf(stderr, "sqlite3_blob_open failed for rowid %ld: %s\n", BlobRowId, sqlite3_errmsg(db));
                break;
            }

            /* Make sure the chunk isn't larger than the buffer. This could happen if a previous
            ** database or BPLib version had a different chunk size. We can't support this case safely. They
            ** will have to create a new database.
            */
            ChunkSize = sqlite3_blob_bytes(blob);
            if (ChunkSize > sizeof(NextBlock->user_data.raw_bytes))
            {
                fprintf(stderr, "Stored BLOB is too large for buffer. DB is corrupted %lu > %lu\n",
                    ChunkSize, sizeof(NextBlock->user_data.raw_bytes));
                SQLStatus = SQLITE_CORRUPT;
                sqlite3_blob_close(blob);
                break;
            }

            /* Allocate a MEM pool block for the blob data */
            NextBlock = BPLib_MEM_BlockAlloc(Pool);
            if (NextBlock == NULL)
            {
                SQLStatus = SQLITE_IOERR;
                sqlite3_blob_close(blob);
                break;
            }

            /* Load the blob directly into the mempool block */
            SQLStatus = sqlite3_blob_read(blob, (void*)&NextBlock->user_data.raw_bytes, ChunkSize, 0);
            if (SQLStatus != SQLITE_OK)
            {
                fprintf(stderr, "sqlite3_blob_read failed: %s\n", sqlite3_errmsg(db));
                sqlite3_blob_close(blob);
                break;
            }

            /* Load Succeeded */
            CurrBlock->next = NextBlock;
            CurrBlock = NextBlock;
            CurrBlock->used_len = ChunkSize;
        }

        sqlite3_blob_close(blob);
    }

    /* Expecting SQLITE_DONE */
    if (SQLStatus == SQLITE_DONE)
    {
        RetBundle = (BPLib_Bundle_t*)(BundleHead);
        RetBundle->blob = BundleHead->next;
        *Bundle = RetBundle;
    }
    else if (BundleHead != NULL)
    {
        BPLib_MEM_BlockListFree(Pool, BundleHead);
    }

    return SQLStatus;
}

static int BPLib_SQL_EgressForDestEIDImpl(BPLib_Instance_t* Inst, BPLib_EID_Pattern_t* DestEID,
    size_t MaxBundles)
{
    int SQLStatus, LoadStatus;
    sqlite3* db = Inst->BundleStorage.db;
    BPLib_Bundle_t* CurrBlockBundle = NULL;
    int CurrBlockBundleID, i;
    int LoadedBundleIDs[BPLIB_STOR_LOADBATCHSIZE];

    /* Bind parameters for metadata query */
    sqlite3_reset(FindByDestNodeStmt);
    SQLStatus = sqlite3_bind_int64(FindByDestNodeStmt, 1, DestEID->MinNode);
    if (SQLStatus != SQLITE_OK) {
        fprintf(stderr, "Failed to bind dest_node min: %s\n", sqlite3_errmsg(db));
        return SQLStatus;
    }
    SQLStatus = sqlite3_bind_int64(FindByDestNodeStmt, 2, DestEID->MaxNode);
    if (SQLStatus != SQLITE_OK) {
        fprintf(stderr, "Failed to bind dest_node max: %s\n", sqlite3_errmsg(db));
        return SQLStatus;
    }
    SQLStatus = sqlite3_bind_int64(FindByDestNodeStmt, 3, DestEID->MinService);
    if (SQLStatus != SQLITE_OK) {
        fprintf(stderr, "Failed to bind dest_service min: %s\n", sqlite3_errmsg(db));
        return SQLStatus;
    }
    SQLStatus = sqlite3_bind_int64(FindByDestNodeStmt, 4, DestEID->MaxService);
    if (SQLStatus != SQLITE_OK) {
        fprintf(stderr, "Failed to bind dest_service max: %s\n", sqlite3_errmsg(db));
        return SQLStatus;
    }
    SQLStatus = sqlite3_bind_int64(FindByDestNodeStmt, 5, MaxBundles);
    if (SQLStatus != SQLITE_OK) {
        fprintf(stderr, "Failed to bind limit: %s\n", sqlite3_errmsg(db));
        return SQLStatus;
    }

    /* For every bundle found, load it into memory */
    Inst->BundleStorage.LoadBatchSize = 0;
    SQLStatus = sqlite3_step(FindByDestNodeStmt);
    while (SQLStatus == SQLITE_ROW)
    {
        /* Load a single bundle from storage that matches the query */
        CurrBlockBundleID = sqlite3_column_int(FindByDestNodeStmt, 0);
        LoadStatus = BPLib_SQL_LoadBundle(db, &Inst->pool, CurrBlockBundleID, &CurrBlockBundle);
        if (LoadStatus != SQLITE_DONE)
        {
            return LoadStatus;
        }

        Inst->BundleStorage.LoadBatch[Inst->BundleStorage.LoadBatchSize] = CurrBlockBundle;
        LoadedBundleIDs[Inst->BundleStorage.LoadBatchSize] = CurrBlockBundleID;
        Inst->BundleStorage.LoadBatchSize++;

        /* Get the NextBlock row */
        SQLStatus = sqlite3_step(FindByDestNodeStmt);
    }

    /* In the case where nothing was loaded, it makes no sense to continue */
    if (Inst->BundleStorage.LoadBatchSize == 0)
    {
        return SQLITE_OK;
    }

    /* Create a batch query */
    SQLStatus = sqlite3_exec(db, "BEGIN;", 0, 0, 0);
    if (SQLStatus != SQLITE_OK)
    {
        fprintf(stderr, "Failed to start transaction\n");
        return SQLStatus;
    }

    for (i = 0; i < Inst->BundleStorage.LoadBatchSize; i++)
    {
        sqlite3_reset(MarkEgressedStmt);
        sqlite3_bind_int64(MarkEgressedStmt, 1, LoadedBundleIDs[i]);
        SQLStatus = sqlite3_step(MarkEgressedStmt);
        if (SQLStatus != SQLITE_DONE)
        {
            fprintf(stderr, "Mark Egressed Failed: %s", sqlite3_errstr(SQLStatus));
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
BPLib_Status_t BPLib_SQL_EgressForDestEID(BPLib_Instance_t* Inst, BPLib_EID_Pattern_t* DestEID,
    size_t MaxBundles)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;
    int SQLStatus;
    sqlite3* db = Inst->BundleStorage.db;

    /* Prepare Insert Statements needed for this batch query */
    SQLStatus = sqlite3_prepare_v2(db, FindByDestNodeSQL, -1, &FindByDestNodeStmt, 0);
    if (SQLStatus != SQLITE_OK)
    {
        Status = BPLIB_STOR_SQL_LOAD_ERR;
    }
    SQLStatus = sqlite3_prepare_v2(db, FindBlobSQL, -1, &FindBlobStmt, 0);
    if (SQLStatus != SQLITE_OK)
    {
        Status = BPLIB_STOR_SQL_LOAD_ERR;
    }
    SQLStatus = sqlite3_prepare_v2(db, MarkEgressedSQL, -1, &MarkEgressedStmt, 0);
    if (SQLStatus != SQLITE_OK)
    {
        Status = BPLIB_STOR_SQL_LOAD_ERR;
    }

    /* Run Batch Load Logic */
    SQLStatus = BPLib_SQL_EgressForDestEIDImpl(Inst, DestEID, MaxBundles);
    if (SQLStatus != SQLITE_OK)
    {
        Status = BPLIB_STOR_SQL_LOAD_ERR;
    }

    /* Finalize */
    sqlite3_finalize(FindByDestNodeStmt);
    sqlite3_finalize(FindBlobStmt);
    sqlite3_finalize(MarkEgressedStmt);

    return Status;
}
