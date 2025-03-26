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

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

/*******************************************************************************
* Definitions and Types
*/
#define BPLIB_STOR_BATCH_SIZE   100

/*******************************************************************************
* SQL Query Definitions 
*  The syntax for these is a little tricky. I reccommend sticking with \n instead
*  of multi-line string literals.
*/

/* Create Table */
static const char* CreateTableSQL = 
"CREATE TABLE IF NOT EXISTS bundle_data (\n"
"    id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
"    action_timestamp INTEGER,\n"
"    dest_node INTEGER\n"
");\n"
"CREATE TABLE IF NOT EXISTS bundle_blobs (\n"
"    id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
"    bundle_id INTEGER,\n"
"    blob_data BLOB,\n"
"    FOREIGN KEY (bundle_id) REFERENCES bundle_data(id) ON DELETE CASCADE\n"
");\n"
"CREATE INDEX IF NOT EXISTS idx_action_timestamp ON bundle_data (action_timestamp);\n"
"CREATE INDEX IF NOT EXISTS idx_dest_node ON bundle_data (dest_node);\n";

/* Insert Bundle Metadata */
static const char* InsertMetadataSQL = 
    "INSERT INTO bundle_data (action_timestamp, dest_node) VALUES (?, ?);";
static sqlite3_stmt* MetadataStmt;

/* Insert Bundle Blob */
const char* InsertBlobSQL = 
    "INSERT INTO bundle_blobs (bundle_id, blob_data) VALUES (?, ?)";
static sqlite3_stmt* InsertBlobStmt;

/* Find by Dest EID */
static const char* FindByDestNodeSQL = 
"SELECT id, action_timestamp, dest_node\n"
"FROM bundle_data\n"
"WHERE dest_node = ?\n"
"ORDER BY action_timestamp ASC\n"
"LIMIT ?;";
static sqlite3_stmt* FindByDestNodeStmt;

static const char* FindBlobSQL = 
"SELECT blob_data\n"
"FROM bundle_blobs\n"
"WHERE bundle_id = ?;";
static sqlite3_stmt* FindBlobStmt;

/* Expire Bundles */
static const char* ExpireBundlesSQL =
"DELETE FROM bundle_data\n"
"WHERE action_timestamp < ?;\n";
static sqlite3_stmt* ExpireBundlesStmt;

/*******************************************************************************
* Static Functions
*/
static int BPLib_SQL_InitImpl(sqlite3** db, const char* DbName)
{
    int SQLStatus;
    sqlite3* ActiveDB;

    SQLStatus = sqlite3_open(DbName, db);
    if (SQLStatus != SQLITE_OK)
    {
        return SQLStatus;
    }
    ActiveDB = *db;

    /* Initialize SQLite3 Pragmas we need for our use case */
    SQLStatus = sqlite3_exec(ActiveDB, "PRAGMA journal_mode=WAL;", 0, 0, NULL);
    if (SQLStatus != SQLITE_OK)
    {
        return SQLStatus;
    }
    SQLStatus = sqlite3_exec(ActiveDB, "PRAGMA foreign_keys=ON;", 0, 0, NULL);
    if (SQLStatus != SQLITE_OK)
    {
        return SQLStatus;
    }
    /* Note: Apparently SQLite3 can have foreign_keys=ON fail SILENTLY if
    ** libsqlite3.so wasn't compiled with foreign key support. We have to manually
    ** check if foreign keys were enabled by reading the setting back.
    */
    // TODO: It looks like I have to use prepare_v2 and step here. Worth creating new ticket for this check, we know it's enabled
    // Don't Merge before ticket is created or you add this.
    // More notes: looks like I have to read back a 0 or a 1 ?? This is confusing me

    /* Create the table if it doesn't already exist */
    SQLStatus = sqlite3_exec(ActiveDB, CreateTableSQL, 0, 0, NULL);
    if (SQLStatus != SQLITE_OK) {
        return SQLStatus;
    }

    /* Expecting SQLITE_OK */
    return SQLStatus;
}

static int BPLib_SQL_StoreMetadata(BPLib_BBlocks_t* BBlocks) // pass metadata_stmt in here.
{
    int SQLStatus;

    sqlite3_reset(MetadataStmt);
    sqlite3_bind_int64(MetadataStmt, 1, (int64_t)BBlocks->PrimaryBlock.Timestamp.CreateTime + 
        (int64_t)BBlocks->PrimaryBlock.Lifetime);
    sqlite3_bind_int64(MetadataStmt, 2, (int64_t)BBlocks->PrimaryBlock.DestEID.Node);

    SQLStatus = sqlite3_step(MetadataStmt);
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

static int BPLib_SQL_DiscardExpiredImpl(sqlite3* db, size_t* NumDiscarded)
{
    int SQLStatus;
    uint64_t DtnTimeNow;

    *NumDiscarded = 0;

    /* Get DTN Time */
    // DtnTimeNow = AskSaraWhatTimeItIs();
    DtnTimeNow = 0; // Will never expire anything.

    /* Prepare and reset the ExpireBundlesStmt with the curren time */
    SQLStatus = sqlite3_prepare_v2(db, ExpireBundlesSQL, -1, &ExpireBundlesStmt, 0);
    if (SQLStatus != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prep: %s\n", sqlite3_errmsg(db));
        return SQLStatus;
    }
    sqlite3_reset(ExpireBundlesStmt);
    SQLStatus = sqlite3_bind_int64(ExpireBundlesStmt, 1, DtnTimeNow);
    if (SQLStatus != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind action_timestamp: %s\n", sqlite3_errmsg(db));
        return SQLStatus;
    }

    /* Run the query */
    SQLStatus = sqlite3_step(ExpireBundlesStmt);
    if (SQLStatus != SQLITE_DONE)
    {
        fprintf(stderr, "Failed to discard bundles: %s\n", sqlite3_errmsg(db));  
        return SQLStatus;
    }
    /* TODO: FINALIZE GOES HERE */

    /* Determine how many changes were made to the database */
    *NumDiscarded = sqlite3_changes(db);

    return SQLITE_OK;
}

/*******************************************************************************
* Exported Functions
*/
BPLib_Status_t BPLib_SQL_Init(sqlite3** db, const char* DbName)
{
    int SQLStatus;
    BPLib_Status_t Status = BPLIB_SUCCESS;

    SQLStatus = BPLib_SQL_InitImpl(db, DbName);
    if (SQLStatus == SQLITE_OK)
    {
        Status = BPLIB_STOR_SQL_INIT_ERR;
    }
    return Status;
}

BPLib_Status_t BPLib_SQL_StoreBatch(sqlite3* db, BPLib_Bundle_t** BatchArr, size_t BatchSize)
{
    BPLib_Status_t Status;
    int SQLStatus;
    int i;

    /* Prepare Insert Statements needed for this batch query */
    SQLStatus = sqlite3_prepare_v2(db, InsertMetadataSQL, -1, &MetadataStmt, 0);
    if (SQLStatus != SQLITE_OK)
    {
        return BPLIB_STOR_SQL_STORAGE_ERR;
    }
    SQLStatus = sqlite3_prepare_v2(db, InsertBlobSQL, -1, &InsertBlobStmt, 0);
    if (SQLStatus != SQLITE_OK)
    {
        return BPLIB_STOR_SQL_STORAGE_ERR;
    }

    /* Create a batch query */
    SQLStatus = sqlite3_exec(db, "BEGIN;", 0, 0, 0);
    if (SQLStatus != SQLITE_OK)
    {
        fprintf(stderr, "Failed to start transaction\n");
        return BPLIB_STOR_SQL_STORAGE_ERR;
    }

    /* Perform an insert for every bundle */
    for (i = 0; i < BatchSize; i++)
    {
        SQLStatus = BPLib_SQL_StoreBundle(db, BatchArr[i]);
        if (SQLStatus != SQLITE_DONE)
        {
            break;
        }
    }

    /* If there have been no errors so far, batch-write the data to persistent storage */
    if (SQLStatus == SQLITE_DONE)
    {
        Status = BPLIB_SUCCESS;
        SQLStatus = sqlite3_exec(db, "COMMIT;", 0, 0, 0);
        if (SQLStatus != SQLITE_OK)
        {
            fprintf(stderr, "Failed to commit transaction\n");
            Status = BPLIB_STOR_SQL_STORAGE_ERR;
        }
    }

    /* The batch commit was not successful, ROLLBACK to prevent DB corruption */
    if (SQLStatus != SQLITE_OK)
    {
        fprintf(stderr, "Attempting ROLLBACK\n");
        Status = BPLIB_STOR_SQL_STORAGE_ERR;
        SQLStatus = sqlite3_exec(db, "ROLLBACK;", 0, 0, 0);
        if (SQLStatus != SQLITE_OK)
        {
            fprintf(stderr, "Failed to rollback transaction\n");
        }
    }

    return Status;
}

BPLib_Status_t BPLib_SQL_EgressForDestEID(sqlite3* db, size_t MaxBundles, size_t* NumEgressed)
{
    // BPLib_Status_t Status;
    int SQLStatus;

    /* Prepare Queries for this batch transaction */
    SQLStatus = sqlite3_prepare_v2(db, FindByDestNodeSQL, -1, &FindByDestNodeStmt, 0);
    if (SQLStatus != SQLITE_OK)
    {
        return BPLIB_STOR_SQL_LOAD_ERR;
    }
    SQLStatus = sqlite3_prepare_v2(db, FindBlobSQL, -1, &FindBlobStmt, 0);
    if (SQLStatus != SQLITE_OK)
    {
        return BPLIB_STOR_SQL_LOAD_ERR;
    }

    

    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_SQL_DiscardExpired(sqlite3* db, size_t* NumDiscarded)
{
    int SQLStatus;

    SQLStatus = BPLib_SQL_DiscardExpiredImpl(db, NumDiscarded);
    if (SQLStatus != SQLITE_OK)
    {
        return BPLIB_STOR_SQL_DISCARD_ERR;
    }
    return BPLIB_SUCCESS;
}
