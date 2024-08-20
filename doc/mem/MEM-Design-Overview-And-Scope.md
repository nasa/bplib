---
# The document metadata is YAML format recognized by pandoc.
title: DTN-cFS BPLib MEM Design
# author: Grafton Kennedy
date: 2024-08-19
output:
    word_document:
#        pandoc_args: ["--toc"]
#        reference_doc: pandoc-custom-reference-sddd.docx
        path: "MEM-Design-Overview-And-Scope.docx"
---
## Design Overview and Scope

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

**IN** All of the CDR design elements listed above. Also requirement DTN.6.25060 Memory Allocator Telemetry. The content of the telemetry will be determined during implementation.

**IN** The Memory Allocator accepts the memory pool created by BPNode and uses only that memory pool for its operations.

At least one type of bundle data model will be used to implement and test the Memory Allocator because it will lead to a smoother transition to Jira Issue DTNN-339 Bundle Cache and Bundle Queue.

**IN** Bundles in the memory pool consist of:
**IN** Primary Block
**IN** List of Canonical Blocks ending with the payload block (CBLOCK_LIST)
**IN** For primary and canonical blocks, links to other blocks (CHUNK_LIST)
**IN** Payload Block

One type of `deserialized_bundle` or `flat_bundle` data structure will be chosen for development and test of the Memory Allocator.
The type of internal data representation for a bundle will be selectable at compile-time even though in this implementation there will be only one type.

**IN** One type of internal data model for bundle.

Jira Issue DTNN-339 defines the task to create Bundle Cache and Bundle Queue so those are out of scope for this issue.

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
#### Dependencies of `mem` modules on `bp-cfs` Modules

Analysis of the `mem` module content inherited from the `mpool` implementation revealed important dependencies that need to be met and eventually pared-back to the essentials.

|  | `mem` Module | Depends on | Which Migrates to |
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
mpool baseline<br> ├── CMakeLists.txt<br>  ├── inc<br>  │   ├── v7_mpool.h<br>  │   ├── v7_mpool_bblocks.h<br>  │   ├── v7_mpool_flows.h<br>  │   ├── v7_mpool_job.h<br>  │   ├── v7_mpool_ref.h<br>  │   └── v7_mpstream.h<br>  ├── src<br>  │   ├── v7_mpool.c<br>  │   ├── v7_mpool_bblocks.c<br>  │   ├── v7_mpool_flows.c<br>  │   ├── v7_mpool_internal.h<br>  │   ├── v7_mpool_job.c<br>  │   ├── v7_mpool_ref.c<br>  │   └── v7_mpstream.c<br>  ├── ut-coverage<br>  │   ├── CMakeLists.txt<br>  │   ├── test_bplib_mpool.h<br>  │   ├── test_bplib_mpool_setup.c<br>  │   ├── test_bplib_v7_mpool.c<br>  │   ├── test_bplib_v7_mpool_bblocks.c<br>  │   ├── test_bplib_v7_mpool_flows.c<br>  │   ├── test_bplib_v7_mpool_job.c<br>  │   ├── test_bplib_v7_mpool_ref.c<br>  │   └── test_bplib_v7_mpstream.c<br>  └── ut-stubs<br>      ├── CMakeLists.txt<br>      ├── v7_mpool_bblocks_stubs.c<br>      ├── v7_mpool_flows_stubs.c<br>      ├── v7_mpool_job_stubs.c<br>      ├── v7_mpool_ref_stubs.c<br>      ├── v7_mpool_stubs.c<br>      └── v7_mpstream_stubs.c <br> <br> <br> <br> |  mem baseline<br>  ├── CMakeLists.txt<br>  ├── inc<br>  │   └── bplib_mem.h<br>  │<br>  │<br>  │<br>  │<br>  │<br>  ├── src<br>  │   └── bplib_mem.c<br>  │<br>  │<br>  │<br>  │<br>  │<br>  │<br>  └── unit-test<br>      ├── CMakeLists.txt<br>     │<br>      │<br>       ├── bplib_mem_test.c<br>      │<br>      │<br>      │<br>      │<br>      │<br>      ├── stubs<br>       │<br>      │<br>      │<br>      │<br>      │<br>        └── bplib_mem_stubs.c<br>   │<br>      └── utilities<br>&nbsp;&nbsp;&nbsp; ├── bplib_mem_test_utils.c<br>&nbsp;&nbsp;&nbsp; └── bplib_mem_test_utils.h |mem final<br>  ├── CMakeLists.txt<br>  ├── inc<br>  │   ├── bplib_mem.h<br>  │   ├── bplib_mem_bblocks.h<br>  │   ├── bplib_mem_ducts.h<br>  │   ├── STOR<br>  │   ├── bplib_mem_ref.h<br>  │   └── STOR<br>  ├── src<br>  │   ├── bplib_mem.c<br>  │   ├── bplib_mem_bblocks.c<br>  │   ├── bplib_mem_ducts.c<br>  │   ├── bplib_mem_internal.c<br>  │   ├── STOR<br>  │   ├── bplib_mem_ref.c<br>  │   └── STOR<br>  └── unit-test<br>      ├── CMakeLists.txt<br>      ├── bplib_mem_test.h<br>      ├── bplib_mem_test_setup.c<br>      ├── bplib_mem_test.c<br>      ├── bplib_mem_test_bblocks.c<br>      ├── bplib_mem_test_ducts.c<br>      ├── STOR<br>      ├── bplib_mem_ref.c<br>      ├── STOR<br>      ├── stubs<br>      │   └── CMakeLists.txt<br>      ├── bplib_mem_bblocks_stubs.c<br>      ├── bplib_mem_ducts_stubs.c<br>      ├── STOR<br>      ├── bplib_mem_ref_stubs.c<br>      ├── bplib_mem_stubs.c<br>      ├── STOR<br>      └── utilities<br> &nbsp;&nbsp;&nbsp; ├── bplib_mem_test_utils.c<br> &nbsp;&nbsp;&nbsp; └── bplib_mem_test_utils.h<br> 

BPLib needs bplib/lib for bplib_init, bplib_deinit, etc.

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
├── unit-test
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

The bp-cfs CMakeLists.txt file has a mismatch in the naming:

`lib` was `bplib_base` in the CMakeLists.txt file.
`libmgr` will be `blib_libmgr` in CMakeLists.txt.
