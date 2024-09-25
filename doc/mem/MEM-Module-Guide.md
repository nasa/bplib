---
# The document metadata is YAML format recognized by pandoc.
title: DTN-cFS BPLib MEM Module Guide
# author: Grafton Kennedy
date: 2024-09-23
output:
    word_document:
#        pandoc_args: ["--toc"]
#        reference_doc: pandoc-custom-reference-sddd.docx
        reference_doc: pandoc-tabloid-reference-sddd.docx
        path: "MEM-Module-Guide.docx"
---
## BPLib Memory Allocator Module Guide

### TODO For the module guide & implementation

1. Review cfe_es_mempool.c versus MEM
1. Add more unit tests - Stress test
1. Verify ut file layout and format
1. BPLib_MEM_BlocktypeFlat
1. Add required abstract types to ci/mem/bplib_api_types.h.patch ready to patch bplib/inc/bplib_api_types.h
1. Reconcile bp_blocktype_t in v7_types.h with bplib_mpool_blocktype
1. Reconcile bplib/v7 with bplib/mem
1. Verify usage of MEM_Block_t and MEM_Pool_t
1. Address all TODOs
1. Write module guide
1. Review Quong's memory allocator and filesystem

### MEM Module Block Diagram

mem_block, mem_blocktype, mem_subq, mem_pool

### MEM External and Interrnal Constraints

### MEM Data Structures

#### Abstract Typedefs

**Initial Design**

**Integrated Design**

### MEM Functions

#### MEM Activity Diagrams

#### MEM Module API

**MEM API**

(Draft)

MEM API

```
void BPLib_MEM_InitBaseObject(BPLib_MEM_BlockHeader_t *block_hdr, uint16_t user_content_length,
static inline bool BPLib_MEM_IsLinkUnattached(const BPLib_MEM_Block_t *list)
static inline bool BPLib_MEM_IsListHead(const BPLib_MEM_Block_t *list)
#define BPLIB_MEM_GET_BUFFER_USER_START_OFFSET(m) (offsetof(BPLib_MEM_BlockBuffer_t, m.user_data_start))
#define BPLIB_MEM_GET_BLOCK_USER_CAPACITY(m) (sizeof(BPLib_MEM_BlockBuffer_t) - MPOOL_GET_BUFFER_USER_START_OFFSET(m))
static inline bool BPLib_MEM_IsAnyContentNode(const BPLib_MEM_Block_t *cb)
static inline bool BPLib_MEM_IsSecondaryIndexNode(const BPLib_MEM_Block_t *cb)
BPLib_MEM_Block_t *BPLib_MEM_BlockFromExternalId(BPLib_MEM_Pool_t *pool, bp_handle_t handle);
static inline bp_handle_t BPLib_MEM_GetExternalId(const BPLib_MEM_Block_t *cb)
void BPLib_MEM_InitListHead(BPLib_MEM_Block_t *base_block, BPLib_MEM_Block_t *list_head);
BPLib_MEM_Pool_t *BPLib_MEM_PoolCreate(void *pool_mem, size_t pool_size);
void BPLib_MEM_RecycleAllBlocksInList(BPLib_MEM_Pool_t *pool, BPLib_MEM_Block_t *list);
int BPLib_MEM_RegisterBlocktype (BPLib_MEM_Pool_t *pool, uint32_t magic_number, const BPLib_MEM_BlocktypeApi_t *api,
int BPLib_MEM_Init(void);
void BPLIB_MEM_RecycleBlock(BPLib_MEM_Pool_t *blk);
```

Typical usage by CACHE:

```
BPLib_MEM_Init
BPLib_MEM_PoolCreate
BPLib_MEM_InitListHead
BPLib_MEM_SubqInit

BPLib_MEM_SubqPushSingle
BPLib_MEM_SubqPullSingle
BPLIB_MEM_RecycleBlock
```

subqs in MEM are free_blocks and recycle_blocks. They are really circular lists, not queues.

subqs in CACHE are cache_blocks like bundle_blocks, job_blocks, etc, circular lists of allocated queue_entry blocks

MEM API Data Structures

```
BPLib_MEM_Block_t
BPLib_MEM_Blocktype_t
BPLib_MEM_Pool_t
BPLib_MEM_SubqBase_t
BPLib_MEM_ListIter_t
```

```
MEM Subq APIs
void BPLib_MEM_SubqInit(BPLib_MEM_Block_t *base_block, BPLib_MEM_SubqBase_t *qblk);
void BPLib_MEM_SubqPushSingle(BPLib_MEM_SubqBase_t *subq, BPLib_MEM_Block_t *cpb);
BPLib_MEM_Block_t *BPLib_MEM_SubqPullSingle(BPLib_MEM_SubqBase_t *subq);

Other MEM subq APIs
static inline uint32_t BPLib_MEM_SubqGetDepth(const BPLib_MEM_SubqBase_t *subq)
uint32_t BPLib_MEM_SubqMergeList(BPLib_MEM_SubqBase_t *subq_dst, BPLib_MEM_Block_t *list);
uint32_t BPLib_MEM_SubqMoveAll(BPLib_MEM_SubqBase_t *subq_dst, BPLib_MEM_SubqBase_t *subq_src);
uint32_t BPLib_MEM_SubqDropAll(BPLib_MEM_Pool_t *pool, BPLib_MEM_SubqBase_t *subq);
```


**Red/Black Tree (RBT) API**

#### MEM Internal API

### MEM Unit Tests

