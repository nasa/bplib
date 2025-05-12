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
#include <stdlib.h> // Only used for abort(): REMOVE BEFORE MERGE

/*******************************************************************************
** SQL Query Definitions
*/
#define BPLIB_SQL_MAX_STRLEN 2048

/* Find for Egress ID */
static const char* FindForEgressID_RangeClause = 
    "((dest_node BETWEEN ? AND ?) AND (dest_service BETWEEN ? AND ?))";
static char WhereClause[BPLIB_SQL_MAX_STRLEN / 2] = {0}; 
static char FindForEgressIdSQL[BPLIB_SQL_MAX_STRLEN] = {0};
static sqlite3_stmt* FindForEgressIDStmt;

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
static int BPLib_SQL_LoadBundleImpl(BPLib_Instance_t* Inst, int64_t BundleID,
    BPLib_Bundle_t** Bundle)
{
    sqlite3_blob* blob = NULL;
    int SQLStatus;
    int64_t BlobRowId;
    size_t ChunkSize;
    BPLib_MEM_Block_t* BundleHead = NULL;
    BPLib_MEM_Block_t* CurrBlock = NULL;
    BPLib_MEM_Block_t* NextBlock = NULL;
    BPLib_Bundle_t* RetBundle;
    sqlite3* db = Inst->BundleStorage.db;
    BPLib_MEM_Pool_t* Pool = &Inst->pool;

    sqlite3_reset(FindBlobStmt);
    SQLStatus = sqlite3_bind_int(FindBlobStmt, 1, BundleID);
    if (SQLStatus != SQLITE_OK)
    {
        fprintf(stderr, "bind failed: %s\n", sqlite3_errmsg(db));
        return SQLStatus;
    }

    /* Load each block of the blob into a mem pool block */
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
            ** database or BPLib version had a different chunk size. We can't support this case safely. User
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

        /* For consistency with other helpers, set status to SQLITE_OK */
        SQLStatus = SQLITE_OK;
    }
    else if (BundleHead != NULL)
    {
        /* Something in the loop above went wrong: Free memory */
        BPLib_MEM_BlockListFree(Pool, BundleHead);
    }

    /* Expecting SQLITE_OK */
    return SQLStatus;
}

static int BPLib_SQL_FindForEIDsImpl(BPLib_Instance_t* Inst, BPLib_STOR_LoadBatch_t* Batch, 
    BPLib_EID_Pattern_t* DestEIDs, size_t NumEIDs, size_t MaxBundles)
{
    int SQLStatus;
    sqlite3* db = Inst->BundleStorage.db;
    int CurrBundleID, i, BindIndex;

    /* Bind parameters for metadata query */
    sqlite3_reset(FindForEgressIDStmt);
    BindIndex = 1;
    for (i = 0; i < NumEIDs; i++)
    {
        SQLStatus = sqlite3_bind_int64(FindForEgressIDStmt, BindIndex++, DestEIDs[i].MinNode);
        if (SQLStatus != SQLITE_OK)
        {
            fprintf(stderr, "Failed to bind dest_node min: %s\n", sqlite3_errmsg(db));
            return SQLStatus;
        }
        SQLStatus = sqlite3_bind_int64(FindForEgressIDStmt, BindIndex++, DestEIDs[i].MaxNode);
        if (SQLStatus != SQLITE_OK)
        {
            fprintf(stderr, "Failed to bind dest_node max: %s\n", sqlite3_errmsg(db));
            return SQLStatus;
        }
        SQLStatus = sqlite3_bind_int64(FindForEgressIDStmt, BindIndex++, DestEIDs[i].MinService);
        if (SQLStatus != SQLITE_OK)
        {
            fprintf(stderr, "Failed to bind dest_node min: %s\n", sqlite3_errmsg(db));
            return SQLStatus;
        }
        SQLStatus = sqlite3_bind_int64(FindForEgressIDStmt, BindIndex++, DestEIDs[i].MaxService);
        if (SQLStatus != SQLITE_OK)
        {
            fprintf(stderr, "Failed to bind dest_node max: %s\n", sqlite3_errmsg(db));
            return SQLStatus;
        }
    }
    SQLStatus = sqlite3_bind_int64(FindForEgressIDStmt, BindIndex++, MaxBundles);
    if (SQLStatus != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind limit: %s\n", sqlite3_errmsg(db));
        return SQLStatus;
    }

    /* For every bundle found, place it's ID in the load batch for the EgressID */
    SQLStatus = sqlite3_step(FindForEgressIDStmt);
    while (SQLStatus == SQLITE_ROW)
    {
        /* Load a single bundle from storage that matches the query */
        CurrBundleID = sqlite3_column_int64(FindForEgressIDStmt, 0);
        if (BPLib_STOR_LoadBatch_AddBundleID(Batch, CurrBundleID) != BPLIB_SUCCESS)
        {
            break;
        }

        /* Go to the next row, which corresponds to the next bundle ID */
        SQLStatus = sqlite3_step(FindForEgressIDStmt);
    }
    if (SQLStatus == SQLITE_DONE)
    {
        /* For consistency with other helpers, convert DONE to OK */
        SQLStatus = SQLITE_OK;
    }

    /* Expecting SQLITE_OK */
    return SQLStatus;
}

static int BPLib_SQL_MarkBatchEgressedImpl(BPLib_Instance_t* Inst, BPLib_STOR_LoadBatch_t* Batch)
{
    int SQLStatus;
    sqlite3* db = Inst->BundleStorage.db;
    //int i;

    /* Create a batch query */
    SQLStatus = sqlite3_exec(db, "BEGIN;", 0, 0, 0);
    if (SQLStatus != SQLITE_OK)
    {
        fprintf(stderr, "Failed to start transaction\n");
        return SQLStatus;
    }

    /* Go through the load batch and add each ID as egressed */
    printf("UNIMPLEMENTED CODE\n");
    abort();
    // for (i = 0; i < Inst->BundleStorage.LoadBatchSize; i++)
    // {
    //     sqlite3_reset(MarkEgressedStmt);
    //     sqlite3_bind_int64(MarkEgressedStmt, 1, LoadedBundleIDs[i]);
    //     SQLStatus = sqlite3_step(MarkEgressedStmt);
    //     if (SQLStatus != SQLITE_DONE)
    //     {
    //         fprintf(stderr, "Mark Egressed Failed: %s", sqlite3_errstr(SQLStatus));
    //         break;
    //     }
    // }

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
BPLib_Status_t BPLib_SQL_FindForEIDs(BPLib_Instance_t* Inst, BPLib_STOR_LoadBatch_t* Batch,
    BPLib_EID_Pattern_t* DestEIDs, size_t NumEIDs)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;
    int SQLStatus, i;
    sqlite3* db = Inst->BundleStorage.db;
    size_t Offset;

    if ((Inst == NULL) || (Batch == NULL) || (DestEIDs == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }
    if (NumEIDs == 0)
    {
        return BPLIB_STOR_PARAM_ERR;
    }

    /* To keep search as efficient possible, we generate one combined query that contains all
    ** the DestEID patterns.
    **
    ** NOTE: 
    ** This bit is tricky to understand from inspection. It is just taking the 
    ** "((dest_node BETWEEN ? AND ?) AND (dest_service BETWEEN ? AND ?))" and appending it once
    **  for each DestEID in the EgressID array.
    */
    Offset = 0;
    for (i = 0; i < NumEIDs; i++)
    {
        if (i > 0)
        {
            Offset += snprintf(WhereClause + Offset, sizeof(WhereClause) - Offset, " OR ");
        }
    
        Offset += snprintf(WhereClause + Offset, sizeof(WhereClause) - Offset, "%s",
            FindForEgressID_RangeClause);
        if (Offset >= sizeof(WhereClause))
        {
            fprintf(stderr, "Programming Error: WHERE clause too long\n");
            return BPLIB_STOR_SQL_LOAD_ERR;
        }
    }
    WhereClause[Offset] = '\0';

    /* Build the final query */
    Offset = snprintf(FindForEgressIdSQL, sizeof(FindForEgressIdSQL),
        "SELECT id FROM bundle_data WHERE (%s) AND egress_attempted = 0 ORDER BY action_timestamp ASC LIMIT ?;",
        WhereClause);
   
    if (Offset >= sizeof(FindForEgressIdSQL))
    {
        fprintf(stderr, "Programming Error: final SQL query too long\n");
        return BPLIB_STOR_SQL_LOAD_ERR;
    }
    FindForEgressIdSQL[Offset] = '\0';

    /* Prepare Search Statements needed for this batch query */
    SQLStatus = sqlite3_prepare_v2(db, FindForEgressIdSQL, -1, &FindForEgressIDStmt, 0);
    if (SQLStatus != SQLITE_OK)
    {
        Status = BPLIB_STOR_SQL_LOAD_ERR;
    }

    if (Status == BPLIB_SUCCESS)
    {
        /* Run Batch Load Logic */
        SQLStatus = BPLib_SQL_FindForEIDsImpl(Inst, Batch, DestEIDs, NumEIDs, BPLIB_STOR_LOADBATCHSIZE);
        if (SQLStatus != SQLITE_OK)
        {
            Status = BPLIB_STOR_SQL_LOAD_ERR;
        }
    }

    /* Cleanup/Finalize */
    sqlite3_finalize(FindForEgressIDStmt);

    return Status;
}

BPLib_Status_t BPLib_SQL_MarkBatchEgressed(BPLib_Instance_t* Inst, BPLib_STOR_LoadBatch_t* Batch)
{
    sqlite3* db = Inst->BundleStorage.db;
    int SQLStatus;
    BPLib_Status_t Status = BPLIB_SUCCESS;

    if (Inst == NULL)
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    SQLStatus = sqlite3_prepare_v2(db, MarkEgressedSQL, -1, &MarkEgressedStmt, 0);
    if (SQLStatus != SQLITE_OK)
    {
        Status = BPLIB_STOR_SQL_LOAD_ERR;
    }

    if (Status == BPLIB_SUCCESS)
    {
        SQLStatus = BPLib_SQL_MarkBatchEgressedImpl(Inst, Batch);
    }

    sqlite3_finalize(MarkEgressedStmt);

    if (SQLStatus != SQLITE_OK)
    {
        return BPLIB_STOR_SQL_LOAD_ERR;
    }
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_SQL_LoadBundle(BPLib_Instance_t* Inst, int64_t BundleID, BPLib_Bundle_t** Bundle)
{
    int SQLStatus;
    sqlite3* db = Inst->BundleStorage.db;
    BPLib_Status_t Status = BPLIB_SUCCESS;

    if ((Inst == NULL) || (Bundle == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }
    if (BundleID < 0)
    {
        return BPLIB_STOR_PARAM_ERR;
    }

    SQLStatus = sqlite3_prepare_v2(db, FindBlobSQL, -1, &FindBlobStmt, 0);
    if (SQLStatus != SQLITE_OK)
    {
        Status = BPLIB_STOR_SQL_LOAD_ERR;
    }

    if (Status == BPLIB_SUCCESS)
    {
        SQLStatus = BPLib_SQL_LoadBundleImpl(Inst, BundleID, Bundle);
    }

    sqlite3_finalize(FindBlobStmt);

    if (SQLStatus != SQLITE_OK)
    {
        return BPLIB_STOR_SQL_LOAD_ERR;
    }
    return BPLIB_SUCCESS;
}
