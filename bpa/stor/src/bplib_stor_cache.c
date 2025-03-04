#include "bplib_stor_cache.h"

#include <sqlite3.h> 

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

/*******************************************************************************
* Definitions and Types 
*/
#define BPLIB_STOR_BATCH_SIZE 100

typedef struct BPLib_BundleCache
{
    sqlite3* db;
    BPLib_Bundle_t* CurrBatch[BPLIB_STOR_BATCH_SIZE];
    size_t CurrBatchSize;
    pthread_mutex_t lock;
} BPLib_BundleCache_t;

static int BatchCnt = 0;

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

/*******************************************************************************
* Module State 
*/
static BPLib_BundleCache_t CacheInst;

/*******************************************************************************
* Static Functions
*/
static BPLib_Status_t BPLib_STOR_StoreBatch(BPLib_Instance_t* Inst)
{
    int i, SQLStatus, CurrBundleID;
    BPLib_Bundle_t* CurrBundle;
    BPLib_MEM_Block_t* CurrMemBlock;

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
        CurrBundle = CacheInst.CurrBatch[i];

        /* Store the bundle metadata */
        sqlite3_reset(MetadataStmt);
        sqlite3_bind_int(MetadataStmt, 1, (int64_t)CurrBundle->blocks.PrimaryBlock.Timestamp.CreateTime +
            CurrBundle->blocks.PrimaryBlock.Lifetime);
        sqlite3_bind_int(MetadataStmt, 2, CurrBundle->blocks.PrimaryBlock.DestEID.Node);

        SQLStatus = sqlite3_step(MetadataStmt);
        if (SQLStatus != SQLITE_DONE)
        {
            printf("Insert meta failed %d\n", i);
            // ROLLBACK
            return BPLIB_ERROR;
        }
        CurrBundleID = sqlite3_last_insert_rowid(CacheInst.db);

        /* Store bblocks. */
        sqlite3_reset(BlobStmt);
        sqlite3_bind_int(BlobStmt, 1, CurrBundleID);
        sqlite3_bind_blob(BlobStmt, 2, (const void*)&CurrBundle->blocks, sizeof(BPLib_BBlocks_t), SQLITE_STATIC);

        /* Store the blob */
        CurrMemBlock = CurrBundle->blob;
        while (CurrMemBlock != NULL)
        {
            sqlite3_reset(BlobStmt);
            sqlite3_bind_int(BlobStmt, 1, CurrBundleID);
            sqlite3_bind_blob(BlobStmt, 2, (const void*)CurrMemBlock->user_data.raw_bytes, CurrMemBlock->used_len,
                SQLITE_STATIC);

            // Execute the insert for each blob
            SQLStatus = sqlite3_step(BlobStmt);
            if (SQLStatus != SQLITE_DONE)
            {
                printf("Insert into bundle_blobs failed\n");
                return BPLIB_ERROR;
            }

            /* Move to the next blob in the linked list */
            CurrMemBlock = CurrMemBlock->next;
        }
    }

    /* Commit, which flushes the batch to persistent storage. */
    SQLStatus = sqlite3_exec(CacheInst.db, "COMMIT;", 0, 0, 0);
    if (SQLStatus != SQLITE_OK)
    {
        printf("Failed to commit transaction\n");
        // ROLLBACK
        return BPLIB_ERROR;
    }

    return BPLIB_SUCCESS;
}

/*******************************************************************************
* Exported Functions
*/
BPLib_Status_t BPLib_STOR_CacheInit(BPLib_Instance_t* Inst)
{
    int SQLStatus;

    pthread_mutex_init(&CacheInst.lock, NULL);

    /* Init SQLite3 Database */
    SQLStatus = sqlite3_open("test.db", &CacheInst.db);
    if (SQLStatus != SQLITE_OK)
    {
        return BPLIB_ERROR;
    }

    /* Pragmas 
    ** These ones are also worth playing with
    ** "PRAGMA synchronous=OFF;"
    ** "PRAGMA temp_store=MEMORY;"
    ** "PRAGMA cache_size=10000;"
    ** "PRAGMA page_size ..."
    */
    SQLStatus = sqlite3_exec(CacheInst.db, "PRAGMA journal_mode=WAL;", 0, 0, NULL);
    if (SQLStatus != SQLITE_OK)
    {
        return BPLIB_ERROR;
    }
    SQLStatus = sqlite3_exec(CacheInst.db, "PRAGMA foreign_keys=ON;", 0, 0, NULL);
    if (SQLStatus != SQLITE_OK)
    {
        return BPLIB_ERROR;
    }
    /* Need to check that foreign keys are in fact enabled by compilation */

    /* Create the table if it doesn't already exist */
    SQLStatus = sqlite3_exec(CacheInst.db, CreateTableSQL, 0, 0, NULL);
    if (SQLStatus != SQLITE_OK) {
        return BPLIB_ERROR;
    }

    /* Init Insert statements: Doing so in Init() is an optimization to avoid
    ** doing in the BatchStore()
    */
    SQLStatus = sqlite3_prepare_v2(CacheInst.db, InsertMetadataSQL, -1, &MetadataStmt, 0);
    if (SQLStatus != SQLITE_OK)
    {
        return BPLIB_ERROR;
    }
    SQLStatus = sqlite3_prepare_v2(CacheInst.db, InsertBlobSQL, -1, &BlobStmt, 0);
    if (SQLStatus != SQLITE_OK)
    {
        return BPLIB_ERROR;
    }

    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_STOR_StoreBundle(BPLib_Instance_t* Inst, BPLib_Bundle_t* bundle)
{
    BPLib_Status_t Status;
    int i;

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
        else
        {
            for (i = 0; i < CacheInst.CurrBatchSize; i++)
            {
                BPLib_QM_AddUnsortedJob(Inst, CacheInst.CurrBatch[i], CONTACT_OUT_STOR_TO_CT, QM_PRI_NORMAL, QM_WAIT_FOREVER);
            }
            CacheInst.CurrBatchSize = 0;
        }
        BatchCnt++;
        printf("BatchCnt %d\n", BatchCnt);
    }

    pthread_mutex_unlock(&CacheInst.lock);


    return Status;
}
