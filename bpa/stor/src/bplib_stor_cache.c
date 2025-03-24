#include "bplib_stor_cache.h"

#include <sqlite3.h> 

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

/*******************************************************************************
* Definitions and Types 
*/
#define BPLIB_STOR_BATCH_SIZE   100

#define BPLIB_STOR_DBNAME       "bplib-storage.db"
/* Use this define to run SQLite3 entirely in RAM
** #define BPLIB_STOR_DBNAME       ":memory:"
*/

typedef struct BPLib_BundleCache
{
    sqlite3* db;
    BPLib_Bundle_t* InsertBatch[BPLIB_STOR_BATCH_SIZE];
    size_t InsertBatchSize;
    pthread_mutex_t lock;
} BPLib_BundleCache_t;

/*******************************************************************************
* Module State 
*/
static BPLib_BundleCache_t CacheInst; // Move to inst

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
// static const char* FindByDestNodeSQL = 
// "SELECT id, action_timestamp, dest_node\n"
// "FROM bundle_data\n"
// "WHERE dest_node = ?\n"
// "ORDER BY action_timestamp ASC\n"
// "LIMIT ?;";
// static sqlite3_stmt* FindByDestNodeStmt;

// static const char* FindBlobSQL = 
// "SELECT blob_data\n"
// "FROM bundle_blobs\n"
// "WHERE bundle_id = ?;";
// static sqlite3_stmt* FindBlobStmt;

/* Expire Bundles */
static const char* ExpireBundlesSQL =
"DELETE FROM bundle_data\n"
"WHERE action_timestamp < ?;\n";
static sqlite3_stmt* ExpireBundlesStmt;

/*******************************************************************************
* SQLite3 Implementation
*/
static int BPLib_SQL_Init(sqlite3** db)
{
    int SQLStatus;
    sqlite3* ActiveDB;

    if (db == NULL)
    {
        return -1;
    }

    SQLStatus = sqlite3_open(BPLIB_STOR_DBNAME, db);
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

    /* Init SQL Statements: Doing so in Init() is an optimization to avoid
    ** re-initializing during operation.
    */
    SQLStatus = sqlite3_prepare_v2(ActiveDB, InsertMetadataSQL, -1, &MetadataStmt, 0);
    if (SQLStatus != SQLITE_OK)
    {
        return SQLStatus;
    }
    SQLStatus = sqlite3_prepare_v2(ActiveDB, InsertBlobSQL, -1, &InsertBlobStmt, 0);
    if (SQLStatus != SQLITE_OK)
    {
        return SQLStatus;
    }

    /* Expecting SQLITE_OK */
    return SQLStatus;
}

static int BPLib_SQL_StoreMetadata(BPLib_BBlocks_t* BBlocks) // pass metadata_stmt in here.
{
    int SQLStatus;

    if (BBlocks == NULL)
    {
        return -1;
    }

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

    return SQLStatus;
}

static int BPLib_SQL_StoreChunk(int64_t BundleRowID, const void* Chunk, size_t ChunkSize)
{
    int SQLStatus;

    if ((Chunk == NULL) || (ChunkSize == 0))
    {
        return -1;
    }

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

    if ((db == NULL) || (Bundle == NULL))
    {
        return -1;
    }

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

static BPLib_Status_t BPLib_SQL_StoreBatch()
{
    BPLib_Status_t Status;
    int SQLStatus;
    int i;

    // Need to prepare the statements here.

    /* Create a batch query */
    SQLStatus = sqlite3_exec(CacheInst.db, "BEGIN;", 0, 0, 0);
    if (SQLStatus != SQLITE_OK)
    {
        fprintf(stderr, "Failed to start transaction\n");
        return BPLIB_ERROR;
    }

    /* Perform an insert for every bundle */
    for (i = 0; i < CacheInst.InsertBatchSize; i++)
    {
        SQLStatus = BPLib_SQL_StoreBundle(CacheInst.db, CacheInst.InsertBatch[i]);
        if (SQLStatus != SQLITE_DONE)
        {
            break;
        }
    }

    /* If there have been no errors so far, batch-write the data to persistent storage */
    if (SQLStatus == SQLITE_DONE)
    {
        Status = BPLIB_SUCCESS;
        SQLStatus = sqlite3_exec(CacheInst.db, "COMMIT;", 0, 0, 0);
        if (SQLStatus != SQLITE_OK)
        {
            fprintf(stderr, "Failed to commit transaction\n");
            Status = BPLIB_ERROR;
        }
    }

    /* The batch commit was not successful, ROLLBACK to prevent DB corruption */
    if (SQLStatus != SQLITE_OK)
    {
        fprintf(stderr, "Attempting ROLLBACK\n");
        Status = BPLIB_ERROR;
        SQLStatus = sqlite3_exec(CacheInst.db, "ROLLBACK;", 0, 0, 0);
        if (SQLStatus != SQLITE_OK)
        {
            fprintf(stderr, "Failed to rollback transaction\n");
        }
    }

    return Status;
}

static int BPLib_SQL_DiscardExpired(size_t* NumDiscarded)
{
    int SQLStatus;
    uint64_t DtnTimeNow;

    if (NumDiscarded == NULL)
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    /* Get DTN Time */
    // DtnTimeNow = AskSaraWhatTimeItIs();
    DtnTimeNow = 0; // Will never expire anything.
    
    /* Prepare and reset the ExpireBundlesStmt with the curren time */
    SQLStatus = sqlite3_prepare_v2(CacheInst.db, ExpireBundlesSQL, -1, &ExpireBundlesStmt, 0);
    if (SQLStatus != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prap: %s\n", sqlite3_errmsg(CacheInst.db));
        return SQLStatus;
    }
    sqlite3_reset(ExpireBundlesStmt);
    SQLStatus = sqlite3_bind_int64(ExpireBundlesStmt, 1, DtnTimeNow);
    if (SQLStatus != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind action_timestamp: %s\n", sqlite3_errmsg(CacheInst.db));
        return SQLStatus;
    }

    /* Run the query */
    SQLStatus = sqlite3_step(ExpireBundlesStmt);
    if (SQLStatus != SQLITE_DONE)
    {
        fprintf(stderr, "Failed to discard bundles: %s\n", sqlite3_errmsg(CacheInst.db));  
        return SQLStatus;
    }

    /* Determine how many changes were made to the database */
    *NumDiscarded = sqlite3_changes(CacheInst.db);
    return SQLITE_OK;
}

/*******************************************************************************
* Exported Functions
*/
BPLib_Status_t BPLib_STOR_CacheInit(BPLib_Instance_t* Inst)
{
    if (Inst == NULL)
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    pthread_mutex_init(&CacheInst.lock, NULL);

    if (BPLib_SQL_Init(&CacheInst.db) != SQLITE_OK)
    {
        return BPLIB_ERROR;
    }

    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_STOR_Destroy()
{
    pthread_mutex_destroy(&CacheInst.lock);

    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_STOR_StoreBundle(BPLib_Instance_t* Inst, BPLib_Bundle_t* Bundle)
{
    BPLib_Status_t Status;

    if ((Inst == NULL) || (Bundle == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    pthread_mutex_lock(&CacheInst.lock);

    /* Add to the next batch */
    CacheInst.InsertBatch[CacheInst.InsertBatchSize++] = Bundle;
    if (CacheInst.InsertBatchSize == BPLIB_STOR_BATCH_SIZE)
    {
        Status = BPLib_SQL_StoreBatch(Inst);
        if (Status != BPLIB_SUCCESS)
        {
            fprintf(stderr, "STORBatch Failed\n");
        }
        CacheInst.InsertBatchSize = 0;
    }

    pthread_mutex_unlock(&CacheInst.lock);

    printf("Batch\n");
    return Status;
}

size_t BPLib_STOR_ScanForDestEID(BPLib_Instance_t* Inst, size_t MaxBundles)
{

    if ((Inst == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }
    if (MaxBundles == 0)
    {
        return BPLIB_ERROR; // Add different code?
    }

    pthread_mutex_lock(&CacheInst.lock);

        

    pthread_mutex_unlock(&CacheInst.lock);

    return 0;
}

size_t BPLib_STOR_DiscardExpired(BPLib_Instance_t* Inst)
{
    size_t NumDiscarded;

    pthread_mutex_lock(&CacheInst.lock);

    if (BPLib_SQL_DiscardExpired(&NumDiscarded) != BPLIB_SUCCESS)
    {
        NumDiscarded = 0;
    }

    pthread_mutex_unlock(&CacheInst.lock);

    return NumDiscarded;
}
