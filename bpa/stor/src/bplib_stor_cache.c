#include "bplib_stor_cache.h"

#include <sqlite3.h> 

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

/*******************************************************************************
* Definitions and Types 
*/
#define BPLIB_STOR_BATCH_SIZE   1

#define BPLIB_STOR_DBNAME       "bplib.db"
/* Use this define to run SQLite3 entirely in RAM
** #define BPLIB_STOR_DBNAME       ":memory:"
*/

typedef struct BPLib_BundleCache
{
    sqlite3* db;
    BPLib_Bundle_t* CurrBatch[BPLIB_STOR_BATCH_SIZE];
    size_t CurrBatchSize;
    pthread_mutex_t lock;
} BPLib_BundleCache_t;

/*******************************************************************************
* Module State 
*/
static BPLib_BundleCache_t CacheInst;

/*******************************************************************************
* SQL Query Definitions 
*/
/* Create Table */
static const char* CreateTableSQL = 
    "CREATE TABLE IF NOT EXISTS bundle_data ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
    "action_timestamp INTEGER, "
    "dest_node INTEGER); "
    "CREATE TABLE IF NOT EXISTS bundle_blobs ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
    "bundle_id INTEGER, "
    "blob_data BLOB, "
    "FOREIGN KEY (bundle_id) REFERENCES bundle_data(id)); "
    "CREATE INDEX IF NOT EXISTS idx_action_timestamp ON bundle_data (action_timestamp); "
    "CREATE INDEX IF NOT EXISTS idx_dest_node ON bundle_data (dest_node);";

/* Insert Bundle Metadata */
static const char* InsertMetadataSQL = 
    "INSERT INTO bundle_data (action_timestamp, dest_node) VALUES (?, ?);";
static sqlite3_stmt* MetadataStmt;

/* Insert Bundle Blob */
const char* InsertBlobSQL = 
    "INSERT INTO bundle_blobs (bundle_id, blob_data) VALUES (?, ?)";
static sqlite3_stmt* BlobStmt;

/* Find by Dest EID */

/* Find by Timestamp */



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
    /* Note: Apparently SQLite3 can silently have foreign_keys=ON fail if
    ** libsqlite3.so wasn't compiled with foreign key support. We have to manually
    ** check if foreign keys were enabled by reading the setting back.
    */
    // TODO: It looks like I have to use prep_v2 and step here.

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
    SQLStatus = sqlite3_prepare_v2(ActiveDB, InsertBlobSQL, -1, &BlobStmt, 0);
    if (SQLStatus != SQLITE_OK)
    {
        return SQLStatus;
    }

    /* Expecting SQLITE_OK */
    return SQLStatus;
}

static int BPLib_SQL_StoreMetadata(BPLib_BBlocks_t* BBlocks)
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
        printf("Insert meta failed\n");
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

    sqlite3_reset(BlobStmt);
    sqlite3_bind_int64(BlobStmt, 1, BundleRowID);
    sqlite3_bind_blob(BlobStmt, 2, Chunk, ChunkSize, SQLITE_STATIC);

    SQLStatus = sqlite3_step(BlobStmt);
    if (SQLStatus != SQLITE_DONE)
    {
        printf("Insert chunk failed\n");
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

static BPLib_Status_t BPLib_STOR_StoreBatch()
{
    BPLib_Status_t Status;
    int SQLStatus;
    int i;

    /* Create a batch query */
    SQLStatus = sqlite3_exec(CacheInst.db, "BEGIN;", 0, 0, 0);
    if (SQLStatus != SQLITE_OK)
    {
        printf("Failed to start transaction\n");
        return BPLIB_ERROR;
    }

    /* Perform an insert for every bundle */
    for (i = 0; i < CacheInst.CurrBatchSize; i++)
    {
        SQLStatus = BPLib_SQL_StoreBundle(CacheInst.db, CacheInst.CurrBatch[i]);
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
            printf("Failed to commit transaction\n");
            Status = BPLIB_ERROR;
        }
    }

    /* The batch commit was not successful, ROLLBACK to prevent DB corruption */
    if (SQLStatus != SQLITE_OK)
    {
        printf("Attempting ROLLBACK\n");
        Status = BPLIB_ERROR;
        SQLStatus = sqlite3_exec(CacheInst.db, "ROLLBACK;", 0, 0, 0);
        if (SQLStatus != SQLITE_OK)
        {
            printf("Failed to rollback transaction\n");
        }
    }

    return Status;
}

/*******************************************************************************
* Exported Functions
*/
BPLib_Status_t BPLib_STOR_CacheInit(BPLib_Instance_t* Inst)
{
    pthread_mutex_init(&CacheInst.lock, NULL);

    if (BPLib_SQL_Init(&CacheInst.db) != SQLITE_OK)
    {
        return BPLIB_ERROR;
    }

    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_STOR_StoreBundle(BPLib_Instance_t* Inst, BPLib_Bundle_t* bundle)
{
    BPLib_Status_t Status;

    pthread_mutex_lock(&CacheInst.lock);

    /* Add to the next batch */
    CacheInst.CurrBatch[CacheInst.CurrBatchSize++] = bundle;

    if (CacheInst.CurrBatchSize == BPLIB_STOR_BATCH_SIZE)
    {
        Status = BPLib_STOR_StoreBatch(Inst);
        if (Status != BPLIB_SUCCESS)
        {
            printf("STORBatch Failed\n");
        }
        CacheInst.CurrBatchSize = 0;
    }

    pthread_mutex_unlock(&CacheInst.lock);

    printf("Batch\n");
    return Status;
}
