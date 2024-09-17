---
# The document metadata is YAML format recognized by pandoc.
title: DTN-cFS BPLib MEM Design
# author: Grafton Kennedy
date: 2024-09-16
output:
    word_document:
#        pandoc_args: ["--toc"]
#        reference_doc: pandoc-custom-reference-sddd.docx
        reference_doc: pandoc-tabloid-reference-sddd.docx
        path: "MEM-Design-Overview-And-Scope.docx"
---
## Design Overview and Scope

### Stages of Prototype to MEM Transition

The MEM design relies on reuse of the Prototype MPOOL component. The Prototype MPOOL, CACHE, and STORE components are tightly coupled. (explanation TBD) Decoupling MEM from CACHE and STORE should improve:

  - Flexibility: Components can be modified independently.
  - Testability: Components can be unit tested independently.
  - Reusability: Components can be reused for other systems.

The MEM design seeks tight cohesion in the component, which can naturally follow from decoupling. Tight cohesione is worth the effort for the same reasons as decoupling plus it should improve the Understandability of the component. Understandable design and code are the keys for all the other "ilities": Flexibility, Maintainability, Reusability, Testability, and Reliability.

The MEM design calls for the decoupling to be done in stages:

  - In Scope/Out of Scope Allocation of MEM and Storage (S) Elements
  - Dependencies of MEM components on Prototype Components
  - Disposition Of Prototype  `bplib mpool` APIs to `dtn-cfs bplib` `mem` and `stor` APIs
  - Migration of `mpool` FSW Files to `mem` FSW Files
  - Divvying up of `mem` source and unit test files to `mem` and `stor` (Decoupling)

### DTN CDR Design Slide Excerpts

  - Provides BPA modules with fast, fragmentation-free volatile storage for bundle data, queues, FIFOs, indices, etc. [Without implementing queues, FIFOs, and indices in this issue.]
  - Not directly accessible by AA or outside node
  - Predictable free/alloc execution times
  - Pre-set total memory usage - less than total amount of system DRAM
  - Memory pool is pre-allocated with fixed block sizes in a memory space that BPNode creates
  - Not dynamically allocated/deallocated
  - Blocks are allocated from existing pool and returned/recycled when no longer needed

#### Service Provided

BPA Modules can request blocks of memory to store bundles (and other things).

#### In Scope/Out of Scope Allocation for this issue

**Relevant Requirements (from the DTNN-328 Description)**

|Name|Text|
|:- |:- |
|DTN.6.25000|Upon accepting a node-shutdown request from AA Node Configuration, CI Memory Allocator shall release the allocated memory.|
|DTN.6.25010|Upon request, CI Memory Allocator shall provide memory in fixed size blocks to the requsted module.|
|DTN.6.25020|If there is no available memory, CI Memory Allocator shall reply with an error indication.|
|DTN.6.25030|CI Memory allocator shall free memory blocks when requested.|
|DTN.6.25040|Upon initialization, CI Memory Allocator shall configure the available memory pool size.|
|DTN.6.25050|CI Memory Allocator shall have predictable execution time for free and allocate operations.|
|DTN.6.25060|Upon request from AA Node Configuration, CI Memory Allocator shall send telemetry to AA Framework Proxy per the Monitor and Control ICD.|

**IN** All of the CDR design elements for the requirements listed above. Also requirement DTN.6.25060 Memory Allocator Telemetry. The content of the telemetry will be determined during implementation.

**IN** The Memory Allocator (MEM) accepts the fixed memory allocation created by BPNode and uses only that memory allocation as the memory pool for its operations.

**IN** MEM manages memory blocks in its memory pool.

**OUT** Storage cache manages memory bundle blocks and other cache-related memory block types.

Note: **OUT** means "Migrated to Storage".

**OUT** Bundles in the Storage cache-related memory block types consist of:
**OUT** Primary Block
**OUT** List of Canonical Blocks ending with the payload block (CBLOCK_LIST)
**OUT** For primary and canonical blocks, links to other blocks (CHUNK_LIST)
**OUT** Payload Block

**OUT** One type of `deserialized_bundle` or `flat_bundle` data structure will be chosen for development and test of the Memory Allocator.
The type of internal data representation for a bundle will be selectable at compile-time even though in this implementation there will be only one type.

Jira Issue DTNN-339 defines the task to create Bundle Cache and Bundle Queue. They are out of scope for this issue.

**OUT** Blocks of memory pool memory are also used for other purposes such as job queues, references, etc.

**IN** Memory can contain more than one block
**IN** Memory Allocator empties all blocks in `recycle_list`, performing garbage collection as required, then moving them to the `free_list`

Initial Plan

Notes on naming:

The "new bplib" will be called the `dtn-cfs bplib` in documentation to distinguish it from the prototype `bplib`.
The "new bpnode" will be called the `dtn-cfs bpnode` in documentation.

The Memory Allocation short name will be `MEM`, not "MA" because `MEM` is a better mnemonic for MEMORY.

Memory Allocation (MEM) is already used in `dtn-cfs bplib` in the subfolder `bplib/ci/mem`.

The `dtn-cfs bplib` uses `MEM` rather than MA in names like `BPLib_MEM_Init`.

API names will use a variant of camel_Snake_Case (eg. `BPLib_MEM_Init`) with initial uppercase letters and acronyms & other short names in all uppercase.

Constants and #defines will use all uppercase (eg. `#define BPLIB_MEM_H`).

Notes on branches

Branches will be created from the develop branch in `bplib` and bpnode and from the main branch in `dtn-cfs`.

All design, implementation, and test documents (if any) will be placed in a new subfolder: `bplib/doc/mem` as either temporary or permanent files.

API names

The CDR API names will be changed to `dtn-cfs bplib` names as follows:

| CDR API Name | `dtn-cfs bplib` Name |
|:- |:- |
| BPL_mpool_bblock_canonical_alloc | BPLib_MEM_Bblock_Canonical_Alloc |
| BPL_mpool_bblock_cbor_alloc | BPLib_MEM_Bblock_CBOR_Alloc |
| BPL_mpool_bblock_primary_alloc | BPLib_MEM_Bblock_Primary_Alloc |
| BPL_mpool_create | BPLib_MEM_Create |
| BPL_mpool_duct_alloc | BPLib_MEM_Duct_Alloc |
| BPL_mpool_generic_data_alloc | BPLib_MEM_Generic_Data_Alloc |
| BPL_mpool_recycle_block | BPLIB_MEM_Recycle_Block |
| BPL_mpool_recycle_all_blocks_in_list | BPLib_MEM_Recycle_All_Blocks_In_List |

End of Jira Initial Comment

----
#### Dependencies of MEM components on Prototype Components

Analysis of the `mem` component content inherited from the `mpool` implementation revealed important dependencies that need to be met and eventually pared-back to the essentials.

|  | `mem` | Depends on | Which Migrates to |
|:- |:- |:- |:- |
| OS | `mem` - Posix/OSAL | bplib/os | bplib/os |
| bplib_init | `mem` | bplib/lib/src/v7_bplib.c:bplib_init() | bplib/libmgr/bplib_libmgr.c |
| Cache | bplib/ci/mem/src/bplib_mem_bblocks.c | bplib/cache/ | bplib/ci/mem/cache/ |
| Common rbtree/crc | bplib/libmgr/bplib_dataservice_api.c | bplib/common/inc/v7_rbtree.h | bplib/ci/mem/common/inc/bplib_rbtree.h |


Where there are more than one modules with the same dependency only one is shown.

#### Disposition Of Prototype  `bplib mpool` APIs to `dtn-cfs bplib` `mem` and `stor` APIs

The prototype  `bplib mpool` APIs include APIs that map to APIs defined for both the `dtn-cfs bplib` `mem` and `stor` APIs. The `stor` APIs are listed here and implemented with Jira Issue DTNN-339 Bundle Cache and Queues.

The prototype `mpool` APIs and internal functions map to the `dtn-cfs bplib` `mem` and `stor` as follows:

_Table of MPOOL Functions Dispositioned to MEM and STOR_

| mpool to MEM | mpool to STOR | ALL mpool APIs and internal functions
|:- |:- |:- 
| _52 MEM APIs and Functions_ | _44 STOR APIs and Functions_ | _96 Total APIs and Functions_
| bplib_mpool_bblock_canonical_alloc | | bplib_mpool_bblock_canonical_alloc
| | bplib_mpool_bblock_canonical_cast | bplib_mpool_bblock_canonical_cast
| | bplib_mpool_bblock_canonical_drop_encode | bplib_mpool_bblock_canonical_drop_encode
| bplib_mpool_bblock_canonical_get_content_length | | bplib_mpool_bblock_canonical_get_content_length
| bplib_mpool_bblock_canonical_get_content_offset | | bplib_mpool_bblock_canonical_get_content_offset
| | bplib_mpool_bblock_canonical_get_encoded_chunks | bplib_mpool_bblock_canonical_get_encoded_chunks
| | bplib_mpool_bblock_canonical_get_logical | bplib_mpool_bblock_canonical_get_logical
| | bplib_mpool_bblock_canonical_set_content_position | bplib_mpool_bblock_canonical_set_content_position
| bplib_mpool_bblock_cbor_alloc | | bplib_mpool_bblock_cbor_alloc
| bplib_mpool_bblock_cbor_append | | bplib_mpool_bblock_cbor_append
| | bplib_mpool_bblock_cbor_cast | bplib_mpool_bblock_cbor_cast
| | bplib_mpool_bblock_cbor_export | bplib_mpool_bblock_cbor_export
| | bplib_mpool_bblock_cbor_set_size | bplib_mpool_bblock_cbor_set_size
| bplib_mpool_bblock_primary_alloc | |  bplib_mpool_bblock_primary_alloc
| | bplib_mpool_bblock_primary_append  |  bplib_mpool_bblock_primary_append
| | bplib_mpool_bblock_primary_cast | bplib_mpool_bblock_primary_cast
| | bplib_mpool_bblock_primary_drop_encode | bplib_mpool_bblock_primary_drop_encode
| bplib_mpool_bblock_primary_get_canonical_list | | bplib_mpool_bblock_primary_get_canonical_list
| | bplib_mpool_bblock_primary_get_encoded_chunks | bplib_mpool_bblock_primary_get_encoded_chunks
| | bplib_mpool_bblock_primary_get_logical | bplib_mpool_bblock_primary_get_logical
| | bplib_mpool_bblock_primary_locate_canonical | bplib_mpool_bblock_primary_locate_canonical
| | bplib_mpool_block_from_external_id | bplib_mpool_block_from_external_id
| bplib_mpool_collect_blocks  | | bplib_mpool_collect_blocks
| bplib_mpool_create  | | bplib_mpool_create
| bplib_mpool_debug_print_list_stats | | bplib_mpool_debug_print_list_stats
| bplib_mpool_debug_scan | | bplib_mpool_debug_scan
| bplib_mpool_dereference  | |  bplib_mpool_dereference
| bplib_mpool_extract_node  | | bplib_mpool_extract_node
| bplib_mpool_flow_alloc | | bplib_mpool_flow_alloc
| | bplib_mpool_flow_cast | bplib_mpool_flow_cast
| | bplib_mpool_flow_disable | bplib_mpool_flow_disable
| | bplib_mpool_flow_enable | bplib_mpool_flow_enable
| | bplib_mpool_flow_is_up | bplib_mpool_flow_is_up
| | bplib_mpool_flow_modify_flags | bplib_mpool_flow_modify_flags
| | bplib_mpool_flow_try_move_all | bplib_mpool_flow_try_move_all
| | bplib_mpool_flow_try_pull | bplib_mpool_flow_try_pull
| | bplib_mpool_flow_try_push | bplib_mpool_flow_try_push
| bplib_mpool_foreach_item_in_list  |  | bplib_mpool_foreach_item_in_list
| bplib_mpool_generic_data_alloc  |  | bplib_mpool_generic_data_alloc
| | bplib_mpool_generic_data_cast  |  bplib_mpool_generic_data_cast
| | bplib_mpool_generic_data_uncast  |  bplib_mpool_generic_data_uncast
| bplib_mpool_get_block_from_link  | | bplib_mpool_get_block_from_link
| bplib_mpool_get_external_id  |  | bplib_mpool_get_external_id
| bplib_mpool_get_generic_data_capacity  | | bplib_mpool_get_generic_data_capacity
| bplib_mpool_get_next_block  | | bplib_mpool_get_next_block
| bplib_mpool_get_parent_pool_from_link | |  bplib_mpool_get_parent_pool_from_link
| bplib_mpool_get_prev_block  | | bplib_mpool_get_prev_block
| | bplib_mpool_get_user_content_size | bplib_mpool_get_user_content_size
| | bplib_mpool_get_user_data_offset_by_blocktype  |  bplib_mpool_get_user_data_offset_by_blocktype
| bplib_mpool_init_list_head  | | bplib_mpool_init_list_head
| bplib_mpool_init_secondary_link  |  bplib_mpool_init_secondary_link
| bplib_mpool_insert_after  | | bplib_mpool_insert_after
| bplib_mpool_insert_before  | | bplib_mpool_insert_before
| bplib_mpool_is_any_content_node  | | bplib_mpool_is_any_content_node
| bplib_mpool_is_empty_list_head  | | bplib_mpool_is_empty_list_head
| bplib_mpool_is_generic_data_block  | | bplib_mpool_is_generic_data_block
| bplib_mpool_is_indirect_block  | | bplib_mpool_is_indirect_block
| bplib_mpool_is_link_attached  | | bplib_mpool_is_link_attached
| bplib_mpool_is_link_unattached  | | bplib_mpool_is_link_unattached
| bplib_mpool_is_list_head  | | bplib_mpool_is_list_head
| bplib_mpool_is_nonempty_list_head  | | bplib_mpool_is_nonempty_list_head
| bplib_mpool_is_secondary_index_node  | | bplib_mpool_is_secondary_index_node
| | bplib_mpool_job_cast | bplib_mpool_job_cast
| | bplib_mpool_job_get_next_active | bplib_mpool_job_get_next_active
| | bplib_mpool_job_init | bplib_mpool_job_init
| | bplib_mpool_job_mark_active | bplib_mpool_job_mark_active
| | bplib_mpool_job_run_all | bplib_mpool_job_run_all
| bplib_mpool_list_iter_forward  |  | bplib_mpool_list_iter_forward
| bplib_mpool_list_iter_goto_first  | | bplib_mpool_list_iter_goto_first
| bplib_mpool_list_iter_goto_last  | | bplib_mpool_list_iter_goto_last
| bplib_mpool_list_iter_reverse  | | bplib_mpool_list_iter_reverse
| bplib_mpool_lock_init  | | bplib_mpool_lock_init
| bplib_mpool_maintain  | | bplib_mpool_maintain
| bplib_mpool_merge_list  | |  bplib_mpool_merge_list
| bplib_mpool_query_mem_current_use  | |  bplib_mpool_query_mem_current_use
| bplib_mpool_query_mem_max_use  | | bplib_mpool_query_mem_max_use
| bplib_mpool_read_refcount  | | bplib_mpool_read_refcount
| bplib_mpool_recycle_all_blocks_in_list  | | bplib_mpool_recycle_all_blocks_in_list
| bplib_mpool_recycle_block  | | bplib_mpool_recycle_block
| bplib_mpool_ref_create  | | bplib_mpool_ref_create
| bplib_mpool_ref_duplicate  | | bplib_mpool_ref_duplicate
| bplib_mpool_ref_from_block  | | bplib_mpool_ref_from_block
| bplib_mpool_ref_make_block  | | bplib_mpool_ref_make_block
| bplib_mpool_ref_release  | | bplib_mpool_ref_release
| | bplib_mpool_register_blocktype | bplib_mpool_register_blocktype
| | bplib_mpool_search_list | bplib_mpool_search_list
| | bplib_mpool_start_stream_init | bplib_mpool_start_stream_init
| | bplib_mpool_stream_attach | bplib_mpool_stream_attach
| | bplib_mpool_stream_close | bplib_mpool_stream_close
| | bplib_mpool_stream_read | bplib_mpool_stream_read
| | bplib_mpool_stream_seek | bplib_mpool_stream_seek
| | bplib_mpool_stream_tell | bplib_mpool_stream_tell
| | bplib_mpool_stream_write | bplib_mpool_stream_write
| | bplib_mpool_subq_get_depth | bplib_mpool_subq_get_depth
| | bplib_mpool_subq_workitem_may_pull | bplib_mpool_subq_workitem_may_pull
| | bplib_mpool_subq_workitem_may_push | bplib_mpool_subq_workitem_may_push

The mpool to MEM functions map to, or are part-of the MEM API as follows:

| mpool -> MEM | MEM API or MEM Internal Class__Method |
|:- |:-
| | 8 APIs, 44 Internal Class__Method
| bplib_mpool_bblock_canonical_alloc | BPLib_MEM_Bblock_Canonical_Alloc
| bplib_mpool_bblock_canonical_get_content_length | BPLib_MEM_Bblock_Canonical__Get_Content_Length
| bplib_mpool_bblock_canonical_get_content_offset | BPLib_MEM_Bblock_Canonical__Get_Content_Offset
| bplib_mpool_bblock_cbor_alloc | BPLib_MEM_Bblock_CBOR_Alloc
| bplib_mpool_bblock_primary_alloc  |  BPLib_MEM_Bblock_Primary_Alloc
| bplib_mpool_bblock_primary_append  |  BPLib_MEM_Bblock_Primary__Append
| bplib_mpool_bblock_primary_get_canonical_list | BPLib_MEM_Bblock_Primary__Get_Canonical_List
| bplib_mpool_collect_blocks  | BPLib_MEM__CollectBlocks
| bplib_mpool_create  |  BPLib_MEM_Create
| bplib_mpool_debug_print_list_stats | BPLib_MEM_Debug__Print_List_Stats
| bplib_mpool_debug_scan | BPLib_MEM_Debug__Scan
| bplib_mpool_dereference  |  BPLib_MEM__Dereference
| bplib_mpool_extract_node  |  BPLib_MEM__Extract_Node
| bplib_mpool_flow_alloc  |  BPLib_MEM_Duct_Alloc
| bplib_mpool_foreach_item_in_list  |  BPLib_MEM__Foreach_Item_In_List
| bplib_mpool_generic_data_alloc  |  BPLib_MEM_Generic_Data_Alloc
| bplib_mpool_get_block_from_link  |  BPLib_MEM__Get_Block_From_Link
| bplib_mpool_get_external_id  |  BPLib_MEM__Get_External_ID
| bplib_mpool_get_generic_data_capacity  |  BPLib_MEM__Get_Generic_Data_Capacity
| bplib_mpool_get_next_block  |  BPLib_MEM__Get_Next_Block
| bplib_mpool_get_parent_pool_from_link  |  BPLib_MEM__Get_Parent_Pool_From_Link
| bplib_mpool_get_prev_block  |  BPLib_MEM__Get_Prev_Block
| bplib_mpool_init_list_head  |  BPLib_MEM__Init_List_Head
| bplib_mpool_init_secondary_link  |  BPLib_MEM__Init_Secondary_Link
| bplib_mpool_insert_after  |  BPLib_MEM__Insert_After
| bplib_mpool_insert_before  |  BPLib_MEM__Insert_Before
| bplib_mpool_is_any_content_node  |  BPLib_MEM__Is_Any_Content_Node
| bplib_mpool_is_empty_list_head  |  BPLib_MEM__Is_Empty_List_Head
| bplib_mpool_is_generic_data_block  |  BPLib_MEM__Is_Generic_Data_Block
| bplib_mpool_is_indirect_block  |  BPLib_MEM__Is_Indirect_Block
| bplib_mpool_is_link_attached  |  BPLib_MEM__Is_Link_Attached
| bplib_mpool_is_link_unattached  |  BPLib_MEM__Is_Link_Unattached
| bplib_mpool_is_list_head  |  BPLib_MEM__Is_List_Head
| bplib_mpool_is_nonempty_list_head  |  BPLib_MEM__Is_Nonempty_List_Head
| bplib_mpool_is_secondary_index_node  |  BPLib_MEM__Is_Secondary_Index_Node
| bplib_mpool_list_iter_forward  |  BPLib_MEM__List_Iter_Forward
| bplib_mpool_list_iter_goto_first  |  BPLib_MEM__List_Iter_Goto_First
| bplib_mpool_list_iter_goto_last  |  BPLib_MEM__List_Iter_Goto_Last
| bplib_mpool_list_iter_reverse  |  BPLib_MEM__List_Iter_Reverse
| bplib_mpool_lock_init  |  BPLib_MEM__Lock_Init
| bplib_mpool_maintain  |  BPLib_MEM__Maintain
| bplib_mpool_merge_list  |  BPLib_MEM__Merge_List
| bplib_mpool_query_mem_current_use  |  BPLib_MEM__Query_Mem_Current_Use
| bplib_mpool_query_mem_max_use  |  BPLib_MEM__Query_Mem_Max_Use
| bplib_mpool_read_refcount  |  BPLib_MEM__Read_Refcount
| bplib_mpool_recycle_all_blocks_in_list  |  BPLib_MEM_Recycle_All_Blocks_In_List
| bplib_mpool_recycle_block  |  BPLib_MEM_Recycle_Block
| bplib_mpool_ref_create  |  BPLib_MEM__Ref_Create
| bplib_mpool_ref_duplicate  |  BPLib_MEM__Ref_Duplicate
| bplib_mpool_ref_from_block  |  BPLib_MEM__Ref_From_Block
| bplib_mpool_ref_make_block  |  BPLib_MEM__Ref_Make_Block
| bplib_mpool_ref_release  |  BPLib_MEM__Ref_Release

#### Migration of `mpool` FSW Files to `mem` FSW Files

The  `mpool` FSW files migrate to `mem` files through renaming according to the convention established for `dtn-cfs` FSW filenames.
  - Change the prefix `v7_mpool` to `bplib_mem`.
  - Change the test filename convention from `test<component-name>.[c|h]`to `bplib_mem_test<component-name>.[c|h]` (with minor discretionary variations)
  - Defer `job` and `stream` components to STOR.
  
| mpool tree baseline   | mem tree baseline    | mem tree final    |
| --- | --- | --- |
mpool baseline:br:<br>├── CMakeLists.txt:br:<br> ├── inc:br:<br> │   ├── v7_mpool.h:br:<br> │   ├── v7_mpool_bblocks.h:br:<br> │   ├── v7_mpool_flows.h:br:<br> │   ├── v7_mpool_job.h:br:<br> │   ├── v7_mpool_ref.h:br:<br> │   └── v7_mpstream.h:br:<br> ├── src:br:<br> │   ├── v7_mpool.c:br:<br> │   ├── v7_mpool_bblocks.c:br:<br> │   ├── v7_mpool_flows.c:br:<br> │   ├── v7_mpool_internal.h:br:<br> │   ├── v7_mpool_job.c:br:<br> │   ├── v7_mpool_ref.c:br:<br> │   └── v7_mpstream.c:br:<br> ├── ut-coverage:br:<br> │   ├── CMakeLists.txt:br:<br> │   ├── test_bplib_mpool.h:br:<br> │   ├── test_bplib_mpool_setup.c:br:<br> │   ├── test_bplib_v7_mpool.c:br:<br> │   ├── test_bplib_v7_mpool_bblocks.c:br:<br> │   ├── test_bplib_v7_mpool_flows.c:br:<br> │   ├── test_bplib_v7_mpool_job.c:br:<br> │   ├── test_bplib_v7_mpool_ref.c:br:<br> │   └── test_bplib_v7_mpstream.c:br:<br> └── ut-stubs:br:<br>     ├── CMakeLists.txt:br:<br>     ├── v7_mpool_bblocks_stubs.c:br:<br>     ├── v7_mpool_flows_stubs.c:br:<br>     ├── v7_mpool_job_stubs.c:br:<br>     ├── v7_mpool_ref_stubs.c:br:<br>     ├── v7_mpool_stubs.c:br:<br>     └── v7_mpstream_stubs.c :br:<br>:br:<br>:br:<br>:br:<br>|  mem baseline:br:<br> ├── CMakeLists.txt:br:<br> ├── inc:br:<br> │   └── bplib_mem.h:br:<br> │:br:<br> │:br:<br> │:br:<br> │:br:<br> │:br:<br> ├── src:br:<br> │   └── bplib_mem.c:br:<br> │:br:<br> │:br:<br> │:br:<br> │:br:<br> │:br:<br> │:br:<br> └── unit-test:br:<br>     ├── CMakeLists.txt:br:<br>    │:br:<br>     │:br:<br>      ├── bplib_mem_test.c:br:<br>     │:br:<br>     │:br:<br>     │:br:<br>     │:br:<br>     │:br:<br>     ├── stubs:br:<br>      │:br:<br>     │:br:<br>     │:br:<br>     │:br:<br>     │:br:<br>       └── bplib_mem_stubs.c:br:<br>  │:br:<br>     └── utilities<br>&nbsp;&nbsp;&nbsp; ├── bplib_mem_test_utils.c<br>&nbsp;&nbsp;&nbsp; └── bplib_mem_test_utils.h |mem final:br:<br> ├── CMakeLists.txt:br:<br> ├── inc:br:<br> │   ├── bplib_mem.h:br:<br> │   ├── bplib_mem_bblocks.h:br:<br> │   ├── bplib_mem_ducts.h:br:<br> │   ├── STOR:br:<br> │   ├── bplib_mem_ref.h:br:<br> │   └── STOR:br:<br> ├── src:br:<br> │   ├── bplib_mem.c:br:<br> │   ├── bplib_mem_bblocks.c:br:<br> │   ├── bplib_mem_ducts.c:br:<br> │   ├── bplib_mem_internal.c:br:<br> │   ├── STOR:br:<br> │   ├── bplib_mem_ref.c:br:<br> │   └── STOR:br:<br> └── unit-test:br:<br>     ├── CMakeLists.txt:br:<br>     ├── bplib_mem_test.h:br:<br>     ├── bplib_mem_test_setup.c:br:<br>     ├── bplib_mem_test.c:br:<br>     ├── bplib_mem_test_bblocks.c:br:<br>     ├── bplib_mem_test_ducts.c:br:<br>     ├── STOR:br:<br>     ├── bplib_mem_ref.c:br:<br>     ├── STOR:br:<br>     ├── stubs:br:<br>     │   └── CMakeLists.txt:br:<br>     ├── bplib_mem_bblocks_stubs.c:br:<br>     ├── bplib_mem_ducts_stubs.c:br:<br>     ├── STOR:br:<br>     ├── bplib_mem_ref_stubs.c:br:<br>     ├── bplib_mem_stubs.c:br:<br>     ├── STOR:br:<br>     └── utilities:br:<br>&nbsp;&nbsp;&nbsp; ├── bplib_mem_test_utils.c:br:<br>&nbsp;&nbsp;&nbsp; └── bplib_mem_test_utils.h:br:<br>

BPLib needs bplib/lib for bplib_init, bplib_deinit, etc. It will be named bplib/libmgr

```
bp-cfs/libs/
bplib/lib
├── CMakeLists.txt
├── inc
│   └── bplib_dataservice.h
├── src
│   ├── v7_base_internal.h
│   ├── v7_bplib.c
│   ├── v7_cla_api.c
│   ├── v7_dataservice_api.c
│   └── v7_routing.c
├── ut-coverage
│   ├── CMakeLists.txt
│   ├── test_bplib_base.h
│   ├── test_bplib_base_setup.c
│   ├── test_v7_bplib.c
│   ├── test_v7_cla_api.c
│   ├── test_v7_dataservice_api.c
│   └── test_v7_routing.c
└── ut-stubs
    ├── CMakeLists.txt
    └── bplib_dataservice_stubs.c

dtn-cfs/libs/
bplib/libmgr
├── CMakeLists.txt
├── inc
│   └── bplib_dataservice.h
├── src
│   ├── bplib_libmgr_internal.h
│   ├── bplib_libmgr.c
│   ├── bplib_cla_api.c
│   ├── bplib_dataservice_api.c
│   └── v7_routing.c
└── unit-test
    ├── CMakeLists.txt
    ├── test_bplib_libmgr.h
    ├── test_bplib_libmgr_setup.c
    ├── test_bplib_libmgr.c
    ├── test_bplib_cla_api.c
    ├── test_bplib_dataservice_api.c
    ├── test_bplib_routing.c
    └── stubs
        ├── CMakeLists.txt
        └── bplib_dataservice_stubs.c
```

The bp-cfs CMakeLists.txt file has a mismatch in the naming:

`lib` was `bplib_base` in the CMakeLists.txt file.
`libmgr` will be `blib_libmgr` in CMakeLists.txt.

#### Divvying up of `mem` source and unit test files to `mem` and `stor` (Decoupling)

 - #ifdef-out STOR code in C files
 - Comment-out STOR definitions in CMakeLists.txt build files
 - Build and unit-test dtn-cfs successfully
 - Document files with STOR for decoupling
 - Decouple STOR-flagged files

 Flagged, built, and unit-tested dtn-cfs successfully.

 ##### `mem` Files flagged for decoupling

 ```
 ```

 ##### Decoupled `mem` Files



gskenned@DESKTOP-HGB20G6:~/repos/gsfc-dtn/dtn-cfs/libs
$ find bplib -name "*.txt" -exec grep -Hlw STOR {} \; -o -name "*.[h.c]" -exec grep -Hlw STOR {} \; | sort
bplib/bpa/stor/CMakeLists.txt
bplib/bpa/stor/inc/bplib_stor.h
bplib/ci/mem/CMakeLists.txt
bplib/ci/mem/cache/src/v7_cache.c
bplib/ci/mem/cache/src/v7_cache_custody.c
bplib/ci/mem/cache/src/v7_cache_fsm.c
bplib/ci/mem/cache/src/v7_cache_internal.h
bplib/ci/mem/cache/ut-coverage/CMakeLists.txt
bplib/ci/mem/common/CMakeLists.txt
bplib/ci/mem/inc/bplib_mem_ducts.h
bplib/ci/mem/inc/bplib_mem_internal.h
bplib/ci/mem/src/bplib_mem.c
bplib/ci/mem/src/bplib_mem_bblocks.c
bplib/ci/mem/src/bplib_mem_ducts.c
bplib/ci/mem/unit-test/CMakeLists.txt
bplib/ci/mem/unit-test/test_bplib_mpool_setup.c
bplib/doc/mem/mpool-baseline-tree.txt
bplib/libmgr/CMakeLists.txt
bplib/libmgr/src/v7_base_internal.h
bplib/libmgr/src/v7_bplib.c
bplib/libmgr/src/v7_cla_api.c
bplib/libmgr/src/v7_dataservice_api.c
bplib/libmgr/ut-coverage/CMakeLists.txt
bplib/libmgr/ut-coverage/test_bplib_base_setup.c
bplib/libmgr/ut-coverage/test_v7_bplib.c

Plan to create S files
  - Rename and/or move files to s
  - Fix all filename changes in source files after each section of source file actions.

Rename bpa/stor/ to bpa/s

```
mv bpa/stor                                                 bpa/s
mv bpa/s/inc/bplib_stor.h                                bpa/s/inc/bplib_s.h
mv bpa/s/src/bplib_stor.c                                bpa/s/src/bplib_s.c
mv bpa/s/unit-test/stubs/bplib_stor_stubs.c              bpa/s/unit-test/stubs/bplib_s_stubs.c
mv bpa/s/unit-test/bplib_stor_test.c                     bpa/s/unit-test/bplib_s_test.c
mv bpa/s/unit-test/utilities/bplib_stor_test_utils.h     bpa/s/unit-test/utilities/bplib_s_test_utils.h
mv bpa/s/unit-test/utilities/bplib_stor_test_utils.c     bpa/s/unit-test/utilities/bplib_s_test_utils.c

Move ci/mem/cache to bpa/s/cache

```
mv ci/mem/cache                                             bpa/s/cache
mv bpa/s/cache/src/v7_cache.c                               bpa/s/cache/src/bplib_cache.c
mv bpa/s/cache/src/v7_cache_custody.c                       bpa/s/cache/src/bplib_cache_custody.c 
mv bpa/s/cache/src/v7_cache_fsm.c                           bpa/s/cache/src/bplib_cache_fsm.c
mv bpa/s/cache/src/v7_cache_internal.h                      bpa/s/cache/src/bplib_cache_internal.h

mv bpa/s/cache/ut-coverage/test_bplib_cache.h              bpa/s/cache/ut-coverage/test_bplib_cache.h
mv bpa/s/cache/ut-coverage/test_bplib_cache_setup.c        bpa/s/cache/ut-coverage/test_bplib_cache_setup.c
mv bpa/s/cache/ut-coverage/test_v7_cache.c                 bpa/s/cache/ut-coverage/test_bplib_cache.c
mv bpa/s/cache/ut-coverage/test_v7_cache_custody.c         bpa/s/cache/ut-coverage/test_bplib_cache_custody.c
mv bpa/s/cache/ut-coverage/test_v7_cache_fsm.c             bpa/s/cache/ut-coverage/test_bplib_cache_fsm.c
```


Move "duct" and "flow" files to bpa/s/qm

```
mkdir -p bpa/s/qm/src bpa/s/qm/unit-test bpa/s/qm/inc bpa/s/qm/unit-test/stubs
mv ci/mem/src/bplib_mem_ducts.c                             bpa/s/qm/src/bplib_mem_ducts.c
mv ci/mem/unit-test/stubs/bplib_mem_ducts_stubs.c           bpa/s/qm/unit-test/stubs/bplib_mem_ducts_stubs.c
mv ci/mem/inc/bplib_mem_ducts.h                             bpa/s/qm/inc/bplib_mem_ducts.h
mv ci/mem/unit-test/test_bplib_v7_mpool_flows.c             bpa/s/qm/unit-test/test_bplib_v7_mpool_ducts.c
```

Move or rename libmgr files

mkdir -p bpa/s/cache/inc
mv libmgr/inc/bplib_dataservice.h                           bpa/s/cache/inc/bplib_dataservice.h
rm -rfi libmgr/inc
mv libmgr/src/v7_base_internal.h                            bpa/s/cache/src/bplib_s_cache_base_internal.h
mv libmgr/src/v7_bplib.c                                    libmgr/src/bplib_libmgr.c
mv libmgr/src/v7_cla_api.c                                  bpa/s/cache/src/bplib_s_cache_cla_api.c
mv libmgr/src/v7_dataservice_api.c                          bpa/s/cache/src/bplib_s_cache_dataservice_api.c
mv libmgr/src/v7_routing.c                                  bpa/s/cache/src/bplib_s_cache_enqueueing.c
mv libmgr/ut-coverage/CMakeLists.txt                        bpa/s/cache/ut-coverage/CMakeLists.txt
mv libmgr/ut-coverage/test_v7_bplib.c                       libmgr/ut-coverage/test_bplib_libmgr.c
mkdir -p bpa/s/cache/ut-coverage
mv libmgr/ut-coverage/test_v7_cla_api.c                     bpa/s/cache/ut-coverage/test_bplib_cla_api.c
mv libmgr/ut-coverage/test_v7_dataservice_api.c             bpa/s/cache/ut-coverage/test_bplib_dataservice_api.c
mv libmgr/ut-coverage/test_v7_routing.c                     bpa/s/cache/ut-coverage/test_bplib_enqueueing.c
mv libmgr/ut-stubs/CMakeLists.txt                           bpa/s/cache/ut-stubs/CMakeLists.txt
mv libmgr/ut-stubs/bplib_dataservice_stubs.c                bpa/s/cache/ut-stubs/bplib_dataservice_stubs.c

Move mem "cache", block, bundle files to bpa/s/cache

mv ci/mem/inc/bplib_mem_bundle.h                            bpa/s/cache/inc/bplib_cache_bundle.h
mv ci/mem/inc/bplib_mem_ref.h                               bpa/s/cache/inc/bplib_cache_ref.h
mv ci/mem/inc/bplib_mem_bblocks.h                           bpa/s/cache/inc/bplib_cache_bblocks.h
mv ci/mem/src/bplib_mem_ref.c                               bpa/s/cache/src/bplib_cache_ref.c
cp ci/mem/src/bplib_mem.c                                   bpa/s/cache/src/bplib_cache.c
mv ci/mem/src/bplib_mem_bblocks.c                           bpa/s/cache/src/bplib_cache_bblocks.c
mv ci/mem/unit-test/stubs/bplib_mem_bblocks_stubs.c         bpa/s/cache/unit-test/stubs/bplib_cache_bblocks_stubs.c
mv ci/mem/unit-test/stubs/bplib_mem_ref_stubs.c             bpa/s/cache/unit-test/stubs/bplib_cache_ref_stubs.c

Rename "*v7*" to "*bplib-mem*" or "*bplib-cache*"

mv bpa/s/cache/inc/v7_cache.h                               bpa/s/cache/inc/bplib_cache.h
mv ci/mem/common/inc/v7_rbtree.h                            ci/mem/common/inc/bplib_rbtree.h

mv bpa/s/cache/ut-stubs/v7_cache_stubs.c                    bpa/s/cache/ut-stubs/bplib_cache_stubs.c
mv bpa/s/qm/unit-test/test_bplib_mpool_ducts.c              bpa/s/qm/unit-test/test_bplib_qm_ducts.c
mv ci/mem/common/src/v7_rbtree.c                            ci/mem/common/src/bplib_rbtree.c
mv ci/mem/common/ut-coverage/test_bplib_v7_rbtree.c         ci/mem/common/ut-coverage/test_bplib_rbtree.c
mv ci/mem/common/ut-stubs/v7_rbtree_stubs.c                 ci/mem/common/ut-stubs/bplib_rbtree_stubs.c
mv ci/mem/unit-test/test_bplib_v7_mpool_bblocks.c           bpa/s/cache/unit-test/test_bplib_cache_bblocks.c
mv ci/mem/unit-test/test_bplib_v7_mpool_job.c               bpa/s/qm/unit-test/test_bplib_qm_job.c
mv ci/mem/unit-test/test_bplib_v7_mpool_ref.c               bpa/s/cache/unit-test/test_bplib_cache_ref.c
mv ci/mem/unit-test/test_bplib_v7_mpool.c                   bpa/s/cache/unit-test/test_bplib_cache.c
mv ci/mem/unit-test/test_bplib_v7_mpstream.c                bpa/s/qm/unit-test/test_bplib_mpstream.c

Prepend bplib_ to crc filenames.
mv ci/mem/common/inc/crc.h                                  ci/mem/common/inc/bplib_crc.h
mv ci/mem/common/src/crc.c                                  ci/mem/common/src/bplib_crc.c
mv ci/mem/common/src/crc_private.h                          ci/mem/common/src/bplib_crc_private.h
mv ci/mem/common/ut-coverage                                ci/mem/common/unit-test
mv ci/mem/common/ut_stubs                                   ci/mem/common/unit-test/stubs
```

Update the .h filenames in the .c files.

**Surgical transplant of cache and queue elements from MEM to S bpa/s/cache and bpa/s/qm**

Metaphorically speaking, the MEM cache and queue elements must be cut and pasted to the proper S components s/cache or s/queue. As the MEM requirements imply, MEM has no knowledge of BP bundles, bundle queues, or persistent storage.

The surgery begins by identifying all of the bundle memory pool-related data types to be transplanted to s/cache.

```
~/repos/gsfc-dtn/dtn-cfs/libs
$ find bplib  -name "*.[c|h]" -exec grep -HE "mpool_.*_t;" {} \;
bplib/ci/mem/unit-test/test_bplib_mpool.h:} UT_bplib_mpool_buf_t;
bplib/ci/mem/inc/bplib_mem.h:typedef struct bplib_mpool_bblock_primary   bplib_mpool_bblock_primary_t;
bplib/ci/mem/inc/bplib_mem.h:typedef struct bplib_mpool_bblock_canonical bplib_mpool_bblock_canonical_t;
bplib/ci/mem/inc/bplib_mem.h:typedef struct bplib_mpool_subq_base bplib_mpool_subq_base_t;
bplib/ci/mem/inc/bplib_mem.h:typedef struct bplib_mpool_flow      bplib_mpool_flow_t;
bplib/ci/mem/inc/bplib_mem.h:} bplib_mpool_blocktype_t;
bplib/ci/mem/inc/bplib_mem.h:} bplib_mpool_eventid_t;
bplib/ci/mem/inc/bplib_mem.h:} bplib_mpool_list_iter_t;
bplib/ci/mem/inc/bplib_mem.h:} bplib_mpool_blocktype_api_t;
bplib/ci/mem/inc/bplib_mem_internal.h:} bplib_mpool_lock_t;
bplib/ci/mem/inc/bplib_mem_internal.h:} bplib_mpool_block_header_t;
bplib/ci/mem/inc/bplib_mem_internal.h:} bplib_mpool_aligned_data_t;
bplib/ci/mem/inc/bplib_mem_internal.h:} bplib_mpool_api_content_t;
bplib/ci/mem/inc/bplib_mem_internal.h:} bplib_mpool_generic_data_content_t;
bplib/ci/mem/inc/bplib_mem_internal.h:} bplib_mpool_bblock_primary_content_t;
bplib/ci/mem/inc/bplib_mem_internal.h:} bplib_mpool_bblock_canonical_content_t;
bplib/ci/mem/inc/bplib_mem_internal.h:} bplib_mpool_flow_content_t;
bplib/ci/mem/inc/bplib_mem_internal.h:} bplib_mpool_block_ref_content_t;
bplib/ci/mem/inc/bplib_mem_internal.h:} bplib_mpool_block_admin_content_t;
bplib/ci/mem/inc/bplib_mem_internal.h:} bplib_mpool_block_buffer_t;
bplib/ci/mem/inc/bplib_mem_internal.h:} bplib_mpool_block_content_t;
bplib/ci/mem/inc/bplib_mem_bblocks.h:} bplib_mpool_bblock_tracking_t;
bplib/ci/mem/inc/bplib_mem_bblocks.h:} bplib_mpool_bblock_primary_data_t;
bplib/bpa/s/qm/inc/bplib_qm_ducts.h:} bplib_mpool_flow_event_t;
bplib/bpa/s/qm/inc/bplib_qm_ducts.h:} bplib_mpool_flow_statechange_event_t;
bplib/bpa/s/qm/inc/bplib_qm_ducts.h:} bplib_mpool_flow_generic_event_t;
bplib/bpa/s/qm/inc/bplib_qm_ducts.h:} bplib_mpool_subq_workitem_t;
bplib/inc/bplib_api_types.h:typedef struct bplib_mpool_block bplib_mpool_block_t;
bplib/inc/bplib_api_types.h:typedef struct bplib_mpool_block_content *bplib_mpool_ref_t;
```

All of the declarations found are for cache memory blocks, not MEM.

The tight coupling of MEM with cache is clear from the mpool datatype declarations in bplib_mem.h and bplib_mem_internal.h. Of 50 "_t;" declarations in mem/inc, 22 are cache-related.

Note that mem/inc/bplib_mem_bundle.h is cache-related too.

It's clear now that all mem/inc/*.h files require relocation to s/cache or surgical transplants.

```
gskenned@ip-10-1-23-64:~/repos/gsfc-dtn/dtn-cfs/libs
$ find bplib/ci/mem/inc  -name "*.[c|h]" -exec grep -HE "_t;" {} \; | grep -vE "mpool_.*_t"
bplib/ci/mem/inc/bplib_mem_bundle.h:typedef uint64_t bp_integer_t;
bplib/ci/mem/inc/bplib_mem_bundle.h:typedef uint8_t bp_blocknum_t;
bplib/ci/mem/inc/bplib_mem_bundle.h:} bp_blocktype_t;
bplib/ci/mem/inc/bplib_mem_bundle.h:} bp_adminrectype_t;
bplib/ci/mem/inc/bplib_mem_bundle.h:typedef bp_integer_t bp_dtntime_t;
bplib/ci/mem/inc/bplib_mem_bundle.h:typedef bp_integer_t bp_sequencenumber_t;
bplib/ci/mem/inc/bplib_mem_bundle.h:} bp_iana_uri_scheme_t;
bplib/ci/mem/inc/bplib_mem_bundle.h:} bp_endpointid_scheme_t;
bplib/ci/mem/inc/bplib_mem_bundle.h:typedef bp_integer_t bp_ipn_nodenumber_t;
bplib/ci/mem/inc/bplib_mem_bundle.h:typedef bp_integer_t bp_ipn_servicenumber_t;
bplib/ci/mem/inc/bplib_mem_bundle.h:} bp_ipn_uri_ssp_t;
bplib/ci/mem/inc/bplib_mem_bundle.h:} bp_dtn_uri_ssp_t;
bplib/ci/mem/inc/bplib_mem_bundle.h:} bp_creation_timestamp_t;
bplib/ci/mem/inc/bplib_mem_bundle.h:} bp_bundle_processing_control_flags_t;
bplib/ci/mem/inc/bplib_mem_bundle.h:} bp_block_processing_flags_t;
bplib/ci/mem/inc/bplib_mem_bundle.h:typedef bp_integer_t bp_lifetime_t;
bplib/ci/mem/inc/bplib_mem_bundle.h:typedef bp_integer_t bp_adu_length_t;
bplib/ci/mem/inc/bplib_mem_bundle.h:} bp_endpointid_ssp_t;
bplib/ci/mem/inc/bplib_mem_bundle.h:} bp_endpointid_buffer_t;
bplib/ci/mem/inc/bplib_mem_bundle.h:} bp_primary_block_t;
bplib/ci/mem/inc/bplib_mem_bundle.h:} bp_canonical_bundle_block_t;
bplib/ci/mem/inc/bplib_mem_bundle.h:} bp_previous_node_block_t;
bplib/ci/mem/inc/bplib_mem_bundle.h:} bp_bundle_age_block_t;
bplib/ci/mem/inc/bplib_mem_bundle.h:} bp_hop_count_block_t;
bplib/ci/mem/inc/bplib_mem_bundle.h:} bp_custody_tracking_block_t;
bplib/ci/mem/inc/bplib_mem_bundle.h:} bp_custody_accept_payload_block_t;
bplib/ci/mem/inc/bplib_mem_bundle.h:} bp_canonical_block_data_t;
bplib/ci/mem/inc/bplib_mem_bundle.h:} bp_canonical_block_buffer_t;
```

What will be left in MEM?

```
gskenned@ip-10-1-23-64:~/repos/gsfc-dtn/dtn-cfs/libs/bplib/ci
$ find mem  -name "*.[c|h]" -exec grep -HE "_t;" {} \; | grep -vE "mpool_.*_t" | grep -vE "bp_.*_t"
mem/common/ut-coverage/test_bplib_rbtree.c:} rbtest_node_t;
mem/common/inc/bplib_crc.h:typedef const struct bplib_crc_parameters bplib_crc_parameters_t;
mem/common/inc/bplib_rbtree.h:} bplib_rbt_link_t;
mem/common/inc/bplib_rbtree.h:} bplib_rbt_root_t;
mem/common/inc/bplib_rbtree.h:} bplib_rbt_iter_t;
mem/common/ut-functional/rbtest.c:} rbtest_node_t;
```

What happened? Where are the fundamental data structures for MEM?

In bplib_mem.h, of course. The coupling in the naming `bplib_mpool_block` and in the `built-in bplib_mpool_blocktype_t type` plus the obscurity of an abstract type definition (explanation TBD) means that loosening the coupling requires surgical removal of the `type` element and a deep understanding of why the `parent_offset` is uin32_t.


```
struct bplib_mpool_block
{
    /* note that if it becomes necessary to recover bits here,
     * both the type and offset could be reduced in size */
    bplib_mpool_blocktype_t   type;
    uint32_t                  parent_offset;
    struct bplib_mpool_block *next;
    struct bplib_mpool_block *prev;
};
```

Searching for `mpool_block` or `mpool_bblock`:

```
gskenned@ip-10-1-23-64:~/repos/gsfc-dtn/dtn-cfs/libs
$ find bplib  -name "*.[c|h]" -exec grep -HE "mpool_b?block_.*_t;" {} \;
bplib/ci/mem/inc/bplib_mem.h:typedef struct bplib_mpool_bblock_primary   bplib_mpool_bblock_primary_t;
bplib/ci/mem/inc/bplib_mem.h:typedef struct bplib_mpool_bblock_canonical bplib_mpool_bblock_canonical_t;
bplib/ci/mem/inc/bplib_mem_internal.h:} bplib_mpool_block_header_t;
bplib/ci/mem/inc/bplib_mem_internal.h:} bplib_mpool_bblock_primary_content_t;
bplib/ci/mem/inc/bplib_mem_internal.h:} bplib_mpool_bblock_canonical_content_t;
bplib/ci/mem/inc/bplib_mem_internal.h:} bplib_mpool_block_ref_content_t;
bplib/ci/mem/inc/bplib_mem_internal.h:} bplib_mpool_block_admin_content_t;
bplib/ci/mem/inc/bplib_mem_internal.h:} bplib_mpool_block_buffer_t;
bplib/ci/mem/inc/bplib_mem_internal.h:} bplib_mpool_block_content_t;
bplib/ci/mem/inc/bplib_mem_bblocks.h:} bplib_mpool_bblock_tracking_t;
bplib/ci/mem/inc/bplib_mem_bblocks.h:} bplib_mpool_bblock_primary_data_t;
bplib/inc/bplib_api_types.h:typedef struct bplib_mpool_block_content *bplib_mpool_ref_t;
```

cache | bplib_mpool_bblock_primary_t
cache | bplib_mpool_bblock_canonical_t
mem   | bplib_mpool_block_header_t
cache |  bplib_mpool_bblock_primary_content_t
cache | bplib_mpool_bblock_canonical_content_t
mem   | bplib_mpool_block_ref_content_t
mem   | bplib_mpool_block_admin_content_t
mem   | bplib_mpool_block_buffer_t
mem   | bplib_mpool_block_content_t
mem   | bplib_mpool_bblock_tracking_t
cache | bplib_mpool_bblock_primary_data_t
mem   | bplib_mpool_ref_t

There is also the concept of a "user block" with content of size BP_MPOOL_MIN_USER_BLOCK_SIZE 480 bytes.

The decoupling strategy is to uniquely name mem versus cache versus qm (and maybe persistent storage)

**Surgery on mem/inc/bplib_mem_internal.h (to begin)**

The naming convention uses the MagicDraw and SDDD naming for the Memory Allocator and Storage modules.  
However, there are some exceptions.

1. Sometimes BPL_mpool refers to the MEM memory pool and sometimes the Bundle Cache (BC) memory pool.
For example, `BPL_mpool_create` creates the MEM memory pool, whereas `BPL_mpool_bblock_primary_alloc` creates a BP Primary Block in BC.
2. The BPLib prefix is “bplib” in the heritage code, “BPL” in the SDDD, and often “BPLib” in the new code.
3. The module or component short name is lowercase in the heritage code and uppercase in the new code.
4. The short name for persistent storage is “PS” in the SDDD. The heritage code uses “file_offload” The CDR slides have “file_offload”

The plan:
1. Use BPLib_MEM_, BPLib_BC, BPLib_QM, and BPLib_file_offload for MEM, BC, QM, and Persistent Storage respectively.
Divvy up the heritage bplib_mpool_* data declarations and functions to BPLib_MEM_* and BPLIB_BC* (mostly)  
For example,  
bplib_mpool_create => BPLib_MEM_create  
bplib_mpool_bblock_primary_alloc => BPLib_BC_bundle_create
2. Note that all MEM and S code will use the BPLib prefix.
3. Use uppercase for the short names except "file_offload".
4. The persistent storage short name will be “file_offload”.

