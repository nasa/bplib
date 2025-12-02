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
#include "bplib_time.h"
#include "bplib_fwp.h"

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

/*******************************************************************************
** SQL Query Definitions
*/

/* Create Table */
/*
 * Table and Index Creation for bundle_data and bundle_blobs
 *
 * This schema is designed to support efficient queries and operations on bundle metadata and associated blob data.
 * The following indexes are created:
 *
 * 1. idx_bundle_blobs_bundle_id:
 *    - Index on the 'bundle_id' column in the 'bundle_blobs' table. This index supports quick lookup of blob data
 *      by its associated bundleID in the 'bundle_data' table.
 *
 * 2. idx_action_timestamp:
 *    - Index on 'action_timestamp' in the 'bundle_data' table. This helps with queries that need to sort or filter
 *      based on the timestamp of the bundle: This is used for expiring bundles
 *
 * 3. idx_find_bundle (Composite Index):
 *    - Composite index on the columns 'dest_node', 'dest_service', 'egress_attempted', 'action_timestamp', and 'id'.
 *    - This index optimizes queries that filter by node and service ranges, filter by egress_attempted (0),
 *      and sort by action_timestamp. It can also enable an index-only scan to quickly retrieve 'id'.
 *    - This composite index is designed for loading egress bundles by batch for a particular EgressID (A channel or contact)
 *
 * 4. idx_egress_attempted:
 *    - Index on the 'egress_attempted' column in the 'bundle_data' table. This index is designed to speed up
 *      DELETE queries and other queries filtering by 'egress_attempted'.
 */
static const char* CreateTableSQL = 
"CREATE TABLE IF NOT EXISTS bundle_data (\n"
"    id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
"    action_timestamp INTEGER,\n"
"    egress_attempted INTEGER DEFAULT 0,\n"
"    dest_node INTEGER,\n"
"    dest_service INTEGER,\n"
"    bundle_bytes INTEGER\n"
");\n"
"\n"
"CREATE TABLE IF NOT EXISTS bundle_blobs (\n"
"    id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
"    bundle_id INTEGER,\n"
"    blob_data BLOB,\n"
"    FOREIGN KEY (bundle_id) REFERENCES bundle_data(id) ON DELETE CASCADE\n"
");\n"
"\n"
"CREATE INDEX IF NOT EXISTS idx_bundle_blobs ON bundle_blobs (bundle_id);\n"
"CREATE INDEX IF NOT EXISTS idx_action_timestamp ON bundle_data (action_timestamp);\n"
"\n"
"CREATE INDEX IF NOT EXISTS idx_egress_id\n"
"ON bundle_data (\n"
"    dest_node,\n"
"    dest_service,\n"
"    egress_attempted,\n"
"    action_timestamp,\n"
"    id\n"
");\n"
"\n"
"CREATE INDEX IF NOT EXISTS idx_egress_attempted\n"
"ON bundle_data (egress_attempted);\n";

/* Expire Bundles */
static const char* DiscardExpiredSQL =
    "WITH to_delete AS ("
    "    SELECT id FROM bundle_data "
    "    WHERE (action_timestamp < ?) AND (egress_attempted = 0) "
    "    LIMIT ?"
    ") "
    "DELETE FROM bundle_data "
    "WHERE id IN (SELECT id FROM to_delete);";
static sqlite3_stmt* DiscardExpiredStmt;

static const char* DiscardEgressedSQL =
    "WITH to_delete AS ("
    "    SELECT id FROM bundle_data "
    "    WHERE egress_attempted = 1 "
    "    LIMIT ?"
    ") "
    "DELETE FROM bundle_data "
    "WHERE id IN (SELECT id FROM to_delete);";
static sqlite3_stmt* DiscardEgressedStmt;


/*******************************************************************************
** Static Functions
*/
static int BPLib_SQL_GetNumStoredBundles(sqlite3 *db, uint32_t *BundleCnt)
{
    const char *sql = "SELECT COUNT(*) FROM bundle_data;";
    sqlite3_stmt *stmt;
    int SQLStatus;

    SQLStatus = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (SQLStatus != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return SQLStatus;
    }

    SQLStatus = sqlite3_step(stmt);
    if (SQLStatus != SQLITE_ROW)
    {
        return SQLStatus;
    }
    *BundleCnt = sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

static int BPLib_SQL_GetTotalBundleBytes(sqlite3* db, uint64_t* TotalBytes)
{
    sqlite3_stmt* stmt;
    int SQLStatus;
    const char* TotalBytesSQL = "SELECT SUM(bundle_bytes) "
                                "AS TotalBytes "
                                "FROM bundle_data;";

    /* Load up the SQL command */
    SQLStatus = sqlite3_prepare_v2(db, TotalBytesSQL, -1, &stmt, NULL);
    if (SQLStatus == SQLITE_OK)
    {
        /* Evaluate the command */
        SQLStatus = sqlite3_step(stmt);
        if (SQLStatus == SQLITE_ROW)
        {
            /* Assign the result of the query to TotalBytes */
            *TotalBytes = sqlite3_column_int64(stmt, 0);
            sqlite3_finalize(stmt);

            /* Set the status to a success value */
            SQLStatus = SQLITE_OK;
        }
        else
        {
            fprintf(stderr, "Error code %s received while evaluating the SQL statement: %s\n",
                    sqlite3_errmsg(db),
                    TotalBytesSQL);
        }
    }
    else
    {
        fprintf(stderr, "Error code %s, received while preparing SQL statement: %s\n",
                sqlite3_errmsg(db),
                TotalBytesSQL);
    }

    return SQLStatus;
}

static int BPLib_SQL_InitImpl(BPLib_Instance_t *Inst, sqlite3** db, const char* DbName)
{
    int SQLStatus;
    sqlite3* ActiveDB;
    int ForeignKeysEnabled;
    sqlite3_stmt* ForeignKeyCheckStmt;
    uint32_t NumStoredBundles;
    uint64_t TotalBundleBytes;
    
    NumStoredBundles = 0;
    TotalBundleBytes = 0;

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

    /* Page size should already be 4096 by default, this just enforces it */
    SQLStatus = sqlite3_exec(ActiveDB, "PRAGMA page_size=4096;", 0, 0, NULL);
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
    if (SQLStatus != SQLITE_OK)
    {
        return SQLStatus;
    }

    /* Determine how many bundles are presently in storage, and set the stored counter to this value */
    if (BPLib_SQL_GetNumStoredBundles(ActiveDB, &NumStoredBundles) != SQLITE_OK)
    {
        return SQLStatus;
    }
    
    Inst->BundleStorage.BundleCountStored = NumStoredBundles;

    /* Find the total number of bytes of bundles stored */
    SQLStatus = BPLib_SQL_GetTotalBundleBytes(ActiveDB, &TotalBundleBytes);
    if (SQLStatus == SQLITE_OK)
    {
        Inst->BundleStorage.BytesStorageInUse = TotalBundleBytes;
    }

    /* Expecting SQLITE_OK */
    return SQLStatus;
}

BPLib_Status_t BPLib_SQL_GetDbSize(BPLib_Instance_t *Inst, size_t *DbSize)
{
    sqlite3_stmt* PageCntStmt;
    size_t PageCnt = 0;
    int SQLStatus;
    *DbSize = 0;

    SQLStatus = sqlite3_prepare_v2(Inst->BundleStorage.db, "PRAGMA page_count;", -1, &PageCntStmt, NULL);
    if (SQLStatus != SQLITE_OK)
    {
        return BPLIB_ERROR;
    }
    if (sqlite3_step(PageCntStmt) == SQLITE_ROW)
    {
        PageCnt = sqlite3_column_int(PageCntStmt, 0);
    }
    sqlite3_finalize(PageCntStmt);

    *DbSize = PageCnt * 4096;

    return BPLIB_SUCCESS;
}

static int BPLib_SQL_DiscardExpiredImpl(sqlite3* db, size_t* NumDiscarded, BPLib_BundleCache_t* BundleCache)
{
    int SQLStatus;
    //BPLib_TIME_MonotonicTime_t DtnMonotonicTime;
    uint64_t DtnNowMs;
    size_t ExpiredBytes;
    sqlite3_stmt* ExpiredBytesStmt;
    const char* ExpiredBytesSQL =
    "WITH expired_bytes AS (\n"
    "   SELECT id, bundle_bytes FROM bundle_data\n"
    "   WHERE (action_timestamp < ?) AND (egress_attempted = 0)\n"
    "   LIMIT ?)\n"
    "SELECT SUM(bundle_bytes)\n"
    "AS bytes_deleted\n"
    "FROM bundle_data\n"
    "WHERE id IN (SELECT id FROM expired_bytes);\n";

    *NumDiscarded = 0;
    ExpiredBytes  = 0;

    /* Get DTN Time */
    // BPLib_TIME_GetMonotonicTime(&DtnMonotonicTime);
    // DtnNowMs = BPLib_TIME_GetDtnTime(DtnMonotonicTime);
    DtnNowMs = BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostTime() - BPLIB_STOR_EPOCHOFFSET;

    /* Collect the size of the bundles to be discarded */
    /* Load up the SQL command */
    SQLStatus = sqlite3_prepare_v2(db, ExpiredBytesSQL, -1, &ExpiredBytesStmt, NULL);
    if (SQLStatus == SQLITE_OK)
    {
        SQLStatus = sqlite3_bind_int64(ExpiredBytesStmt, 1, (int64_t) DtnNowMs);
        if (SQLStatus == SQLITE_OK)
        {
            SQLStatus = sqlite3_bind_int64(ExpiredBytesStmt, 2, BPLIB_STOR_DISCARDBATCHSIZE);
            if (SQLStatus == SQLITE_OK)
            {
                /* Evaluate the command */
                SQLStatus = sqlite3_step(ExpiredBytesStmt);
                if (SQLStatus == SQLITE_ROW)
                {
                    /* Assign the result of the query to EgressedBytes */
                    ExpiredBytes = sqlite3_column_int64(ExpiredBytesStmt, 0);
                    sqlite3_finalize(ExpiredBytesStmt);

                    /* Amount is decremented when the command to discard is successful */
                }
                else
                {
                    fprintf(stderr, "Error code %s received while evaluating the SQL statement: %s\n",
                            sqlite3_errmsg(db),
                            ExpiredBytesSQL);
                }
            }
            else
            {
                fprintf(stderr, "Failed to bind LIMIT: %s\n", sqlite3_errmsg(db));
            }
        }
        else
        {
            fprintf(stderr, "Failed to bind action_timestamp: %s\n", sqlite3_errmsg(db));
        }
    }
    else
    {
        fprintf(stderr, "Error code %s, received while preparing SQL statement: %s\n",
                sqlite3_errmsg(db),
                ExpiredBytesSQL);
    }

    /* Create a batch query */
    SQLStatus = sqlite3_exec(db, "BEGIN;", 0, 0, 0);
    if (SQLStatus != SQLITE_OK)
    {
        fprintf(stderr, "Failed to start transaction: %s\n", sqlite3_errmsg(db));
        return SQLStatus;
    }

    sqlite3_reset(DiscardExpiredStmt);
    SQLStatus = sqlite3_bind_int64(DiscardExpiredStmt, 1, (int64_t)DtnNowMs);
    if (SQLStatus != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind action_timestamp: %s\n", sqlite3_errmsg(db));
        return SQLStatus;
    }
    SQLStatus = sqlite3_bind_int64(DiscardExpiredStmt, 2, BPLIB_STOR_DISCARDBATCHSIZE);
    if (SQLStatus != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind LIMIT: %s\n", sqlite3_errmsg(db));
        return SQLStatus;
    }

    /* Run the query */
    SQLStatus = sqlite3_step(DiscardExpiredStmt);
    if (SQLStatus != SQLITE_DONE)
    {
        fprintf(stderr, "Failed to discard expired bundles: %s\n", sqlite3_errmsg(db));  
        return SQLStatus;
    }

    /* If there have been no errors so far commit the delete  */
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
            fprintf(stderr, "Failed to rollback transaction, RC=%d\n", SQLStatus);
        }
    }

    /* Determine how many changes were made to the database, which is the number
    ** of discarded bundles.
    */
    *NumDiscarded = sqlite3_changes(db);

    /* Decrement that counter that tracks bytes of storage used */
    BundleCache->BytesStorageInUse -= ExpiredBytes;    

    return SQLITE_OK;
}

static int BPLib_SQL_DiscardEgressedImpl(sqlite3* db, size_t* NumDiscarded, BPLib_BundleCache_t* BundleCache)
{
    int SQLStatus;
    size_t EgressedBytes ;
    sqlite3_stmt* EgressedBytesStmt;
    const char* EgressedBytesSQL =
    "WITH egressed_bytes AS (\n"
    "   SELECT id, bundle_bytes FROM bundle_data\n"
    "   WHERE egress_attempted = 1\n"
    "   LIMIT ?)\n"
    "SELECT SUM(bundle_bytes)\n"
    "AS bytes_deleted\n"
    "FROM bundle_data\n"
    "WHERE id IN (SELECT id FROM egressed_bytes);\n";

    *NumDiscarded = 0;
    EgressedBytes = 0;

    /* Collect the size of the bundles to be discarded */
    /* Load up the SQL command */
    SQLStatus = sqlite3_prepare_v2(db, EgressedBytesSQL, -1, &EgressedBytesStmt, NULL);
    if (SQLStatus == SQLITE_OK)
    {
        SQLStatus = sqlite3_bind_int64(EgressedBytesStmt, 1, BPLIB_STOR_DISCARDBATCHSIZE);
        if (SQLStatus == SQLITE_OK)
        {
            /* Evaluate the command */
            SQLStatus = sqlite3_step(EgressedBytesStmt);
            if (SQLStatus == SQLITE_ROW)
            {
                /* Assign the result of the query to EgressedBytes */
                EgressedBytes = sqlite3_column_int64(EgressedBytesStmt, 0);
                sqlite3_finalize(EgressedBytesStmt);

                /* Amount is decremented when the command to discard is successful */
            }
            else
            {
                fprintf(stderr, "Error code %s received while evaluating the SQL statement: %s\n",
                        sqlite3_errmsg(db),
                        EgressedBytesSQL);
            }
        }
        else
        {
            fprintf(stderr, "Failed to bind LIMIT: %s\n", sqlite3_errmsg(db));
        }
    }
    else
    {
        fprintf(stderr, "Error code %s, received while preparing SQL statement: %s\n",
                sqlite3_errmsg(db),
                EgressedBytesSQL);
    }

    /* Create a batch query */
    SQLStatus = sqlite3_exec(db, "BEGIN;", 0, 0, 0);
    if (SQLStatus != SQLITE_OK)
    {
        fprintf(stderr, "Failed to start transaction: %s\n", sqlite3_errmsg(db));
        return SQLStatus;
    }

    sqlite3_reset(DiscardEgressedStmt);
    SQLStatus = sqlite3_bind_int64(DiscardEgressedStmt, 1, BPLIB_STOR_DISCARDBATCHSIZE);
    if (SQLStatus != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind LIMIT: %s\n", sqlite3_errmsg(db));
        return SQLStatus;
    }

    /* Run the query */
    SQLStatus = sqlite3_step(DiscardEgressedStmt);
    if (SQLStatus != SQLITE_DONE)
    {
        fprintf(stderr, "Failed to discard egressed bundles: %s\n", sqlite3_errmsg(db));  
        return SQLStatus;
    }

    /* If there have been no errors so far commit the delete  */
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
            fprintf(stderr, "Failed to rollback transaction, RC=%d\n", SQLStatus);
        }
    }

    /* Determine how many changes were made to the database, which is the number
    ** of discarded bundles.
    */
    *NumDiscarded = sqlite3_changes(db);

    /* Decrement that counter that tracks bytes of storage used */
    BundleCache->BytesStorageInUse -= EgressedBytes;

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

    SQLStatus = BPLib_SQL_InitImpl(Inst, db, DbName);
    if (SQLStatus != SQLITE_OK)
    {
        Status = BPLIB_STOR_SQL_INIT_ERR;
    }

    return Status;
}

BPLib_Status_t BPLib_SQL_DiscardExpired(BPLib_Instance_t* Inst, size_t* NumDiscarded)
{
    int SQLStatus;
    sqlite3* db = Inst->BundleStorage.db;
    BPLib_Status_t Status = BPLIB_SUCCESS;

    SQLStatus = sqlite3_prepare_v2(db, DiscardExpiredSQL, -1, &DiscardExpiredStmt, 0);
    if (SQLStatus != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prep: %s\n", sqlite3_errmsg(db));
        Status = BPLIB_STOR_SQL_DISCARD_ERR;
    }

    if (Status == BPLIB_SUCCESS)
    {
        SQLStatus = BPLib_SQL_DiscardExpiredImpl(db, NumDiscarded, &(Inst->BundleStorage));
        if (SQLStatus != SQLITE_OK)
        {
            Status = BPLIB_STOR_SQL_DISCARD_ERR;
        }
    }

    /* Finalize the statement */
    sqlite3_finalize(DiscardExpiredStmt);

    return Status;
}

BPLib_Status_t BPLib_SQL_DiscardEgressed(BPLib_Instance_t* Inst, size_t* NumDiscarded)
{
    int SQLStatus;
    sqlite3* db = Inst->BundleStorage.db;
    BPLib_Status_t Status = BPLIB_SUCCESS;

    SQLStatus = sqlite3_prepare_v2(db, DiscardEgressedSQL, -1, &DiscardEgressedStmt, 0);
    if (SQLStatus != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prep: %s\n", sqlite3_errmsg(db));
        Status = BPLIB_STOR_SQL_DISCARD_ERR;
    }

    if (Status == BPLIB_SUCCESS)
    {
        SQLStatus = BPLib_SQL_DiscardEgressedImpl(db, NumDiscarded, &(Inst->BundleStorage));
        if (SQLStatus != SQLITE_OK)
        {
            Status = BPLIB_STOR_SQL_DISCARD_ERR;
        }
    }

    /* Finalize the statement */
    sqlite3_finalize(DiscardEgressedStmt);

    return Status;
}
