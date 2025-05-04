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
#include "bplib_time.h"
#include "bplib_fwp.h"

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

/*******************************************************************************
** SQL Query Definitions
*/

/* Create Table */
static const char* CreateTableSQL = 
"CREATE TABLE IF NOT EXISTS bundle_data (\n"
"    id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
"    action_timestamp INTEGER,\n"
"    egress_attempted INTEGER DEFAULT 0,\n"
"    dest_node INTEGER,\n"
"    dest_service INTEGER\n"
");\n"
"CREATE TABLE IF NOT EXISTS bundle_blobs (\n"
"    id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
"    bundle_id INTEGER,\n"
"    blob_data BLOB,\n"
"    FOREIGN KEY (bundle_id) REFERENCES bundle_data(id) ON DELETE CASCADE\n"
");\n"
"CREATE INDEX IF NOT EXISTS idx_bundle_blobs_bundle_id ON bundle_blobs (bundle_id);\n"
"CREATE INDEX IF NOT EXISTS idx_action_timestamp ON bundle_data (action_timestamp);\n"
"CREATE INDEX IF NOT EXISTS idx_dest_node ON bundle_data (dest_node);\n";

/* Expire Bundles */
static const char* ExpireBundlesSQL =
    "DELETE FROM bundle_data WHERE action_timestamp < ? OR egress_attempted = 1;";
static sqlite3_stmt* ExpireBundlesStmt;


/*******************************************************************************
** Static Functions
*/
static int BPLib_SQL_InitImpl(sqlite3** db, const char* DbName)
{
    int SQLStatus;
    sqlite3* ActiveDB;
    int ForeignKeysEnabled;
    sqlite3_stmt* ForeignKeyCheckStmt;

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
    SQLStatus = sqlite3_exec(ActiveDB, "PRAGMA synchronous=OFF;", 0, 0, NULL);
    if (SQLStatus != SQLITE_OK)
    {
        return SQLStatus;
    }

    /* Note: Apparently SQLite3 can have foreign_keys=ON fail SILENTLY if
    ** libsqlite3.so wasn't compiled with foreign key support. We have to manually
    ** check if foreign keys were enabled by reading the setting back.
    */
    ForeignKeysEnabled = 0;
    SQLStatus = sqlite3_prepare_v2(ActiveDB, "PRAGMA foreign_keys;", -1, &ForeignKeyCheckStmt, NULL);
    if (SQLStatus != SQLITE_OK)
    {
        return SQLStatus;
    }
    if (sqlite3_step(ForeignKeyCheckStmt) == SQLITE_ROW)
    {
        ForeignKeysEnabled = sqlite3_column_int(ForeignKeyCheckStmt, 0);
    }
    sqlite3_finalize(ForeignKeyCheckStmt);
    if (ForeignKeysEnabled != 1)
    {
        fprintf(stderr, "Please use a SQLite3 compiled with Foreign Key Support.\n");
        return SQLITE_MISUSE;
    }

    /* Create the table if it doesn't already exist */
    SQLStatus = sqlite3_exec(ActiveDB, CreateTableSQL, 0, 0, NULL);
    if (SQLStatus != SQLITE_OK) {
        return SQLStatus;
    }

    /* Expecting SQLITE_OK */
    return SQLStatus;
}

static int BPLib_SQL_GarbageCollectImpl(sqlite3* db, size_t* NumDiscarded)
{
    int SQLStatus;
    //BPLib_TIME_MonotonicTime_t DtnMonotonicTime;
    uint64_t DtnNowMs;

    *NumDiscarded = 0;

    /* Get DTN Time */
    // BPLib_TIME_GetMonotonicTime(&DtnMonotonicTime);
    // DtnNowMs = BPLib_TIME_GetDtnTime(DtnMonotonicTime);
    DtnNowMs = BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostTime() - BPLIB_STOR_EPOCHOFFSET;

    sqlite3_reset(ExpireBundlesStmt);
    SQLStatus = sqlite3_bind_int64(ExpireBundlesStmt, 1, (uint64_t)DtnNowMs);
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

    /* Determine how many changes were made to the database, which is the number
    ** of discarded bundles.
    */
    *NumDiscarded = sqlite3_changes(db);

    return SQLITE_OK;
}

/*******************************************************************************
** Exported Functions
*/
BPLib_Status_t BPLib_SQL_Init(BPLib_Instance_t* Inst, const char* DbName)
{
    int SQLStatus;
    BPLib_Status_t Status = BPLIB_SUCCESS;
    sqlite3** db = &Inst->BundleStorage.db;

    SQLStatus = BPLib_SQL_InitImpl(db, DbName);
    if (SQLStatus != SQLITE_OK)
    {
        Status = BPLIB_STOR_SQL_INIT_ERR;
    }

    return Status;
}

BPLib_Status_t BPLib_SQL_GarbageCollect(BPLib_Instance_t* Inst, size_t* NumDiscarded)
{
    int SQLStatus;
    sqlite3* db = Inst->BundleStorage.db;
    BPLib_Status_t Status = BPLIB_SUCCESS;

    /* Prepare the ExpireBundlesStmt with the current time */
    SQLStatus = sqlite3_prepare_v2(db, ExpireBundlesSQL, -1, &ExpireBundlesStmt, 0);
    if (SQLStatus != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prep: %s\n", sqlite3_errmsg(db));
        Status = BPLIB_STOR_SQL_DISCARD_ERR;
    }

    if (Status == BPLIB_SUCCESS)
    {
        SQLStatus = BPLib_SQL_GarbageCollectImpl(db, NumDiscarded);
        if (SQLStatus != SQLITE_OK)
        {
            Status = BPLIB_STOR_SQL_DISCARD_ERR;
        }
    }

    /* Finalize the statement */
    sqlite3_finalize(ExpireBundlesStmt);

    return Status;
}
