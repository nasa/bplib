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
static sqlite3_stmt* FindByDestNodeStmt;

/* Find blob by ID */
static const char* FindBlobSQL = 
"SELECT blob_data\n"
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
    int SQLStatus;
    int ChunkLen;
    void* ChunkData;
    BPLib_MEM_Block_t* CurrBlock = NULL;
    BPLib_MEM_Block_t* NextBlock = NULL;
    BPLib_MEM_Block_t* BundleHead = NULL;
    BPLib_Bundle_t* RetBundle;

    sqlite3_reset(FindBlobStmt);
    SQLStatus = sqlite3_bind_int64(FindBlobStmt, 1, BundleID);
    if (SQLStatus != SQLITE_OK) {
        fprintf(stderr, "Failed to bind BundleID: %s\n", sqlite3_errmsg(db));
        return SQLStatus;
    }

    SQLStatus = sqlite3_step(FindBlobStmt);
    while (SQLStatus == SQLITE_ROW)
    {
        /* We're loading bblocks_t */
        if (BundleHead == NULL)
        {
            BundleHead = BPLib_MEM_BlockAlloc(Pool);
            if (BundleHead == NULL)
            {
                SQLStatus = -1;
                break;
            }

            /* Load the bundle meta data */
            ChunkLen = sqlite3_column_bytes(FindBlobStmt, 0);
            ChunkData = (void*)sqlite3_column_blob(FindBlobStmt, 0);
            memcpy(&BundleHead->user_data.bundle.blocks, ChunkData, ChunkLen);

            CurrBlock = BundleHead;
            CurrBlock->used_len = ChunkLen;
        }
        /* We're loading part of the blob */
        else
        {
            NextBlock = BPLib_MEM_BlockAlloc(Pool);
            if (NextBlock == NULL)
            {
                BPLib_MEM_BlockListFree(Pool, BundleHead);
                SQLStatus = -1;
                break;
            }

            /* Copy the blob chunk into the bundle */
            ChunkLen = sqlite3_column_bytes(FindBlobStmt, 0);
            ChunkData = (void*)sqlite3_column_blob(FindBlobStmt, 0);
            memcpy(&NextBlock->user_data.raw_bytes, ChunkData, ChunkLen);

            CurrBlock->next = NextBlock;
            CurrBlock = NextBlock;
            CurrBlock->used_len = ChunkLen;
        }

        SQLStatus = sqlite3_step(FindBlobStmt);
    }

    /* Expecting SQLITE_DONE */
    if (SQLStatus == SQLITE_DONE)
    {
        RetBundle = (BPLib_Bundle_t*)(BundleHead);
        RetBundle->blob = BundleHead->next;
        *Bundle = RetBundle;
    }

    return SQLStatus;
}

static int BPLib_SQL_EgressForDestEIDImpl(BPLib_Instance_t* Inst, BPLib_EID_Pattern_t* DestEID,
    size_t MaxBundles)
{
    int SQLStatus, LoadStatus;
    sqlite3* db = Inst->BundleStorage.db;
    BPLib_Bundle_t* CurrBundle = NULL;
    int CurrBundleID, i;
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
        CurrBundleID = sqlite3_column_int(FindByDestNodeStmt, 0);
        LoadStatus = BPLib_SQL_LoadBundle(db, &Inst->pool, CurrBundleID, &CurrBundle);
        if (LoadStatus != SQLITE_DONE)
        {
            return LoadStatus;
        }

        Inst->BundleStorage.LoadBatch[Inst->BundleStorage.LoadBatchSize] = CurrBundle;
        LoadedBundleIDs[Inst->BundleStorage.LoadBatchSize] = CurrBundleID;
        Inst->BundleStorage.LoadBatchSize++;

        /* Get the next row */
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
