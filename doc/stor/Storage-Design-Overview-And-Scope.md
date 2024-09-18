---
# The document metadata is YAML format recognized by pandoc.
title: DTN-cFS BPLib Storage (S) Design
# author: Grafton Kennedy
date: 2024-09-16
output:
    word_document:
#        pandoc_args: ["--toc"]
#        reference_doc: pandoc-custom-reference-sddd.docx
        reference_doc: pandoc-tabloid-reference-sddd.docx
        path: "Storage-Design-Overview-And-Scope.docx"
---
## Design Overview and Scope

TODO - S WIP Storage Design

**This document is a Work-in-Progress (WIP)**

### Stages of Prototype to MEM Transition

The Storage design relies on reuse of the Prototype BPLIB_MEM component. The Prototype BPLIB_MEM, CACHE, and STORE components are tightly coupled. (explanation TBD) Decoupling S from BPLIB_MEM and STORE should improve:

  - Flexibility: Components can be modified independently.
  - Testability: Components can be unit tested independently.
  - Reusability: Components can be reused for other systems.

The S design seeks tight cohesion in the component, which can naturally follow from decoupling. Tight cohesione is worth the effort for the same reasons as decoupling plus it should improve the Understandability of the component. Understandable design and code are the keys for all the other "ilities": Flexibility, Maintainability, Reusability, Testability, and Reliability.

The S design calls for the decoupling to be done in stages:

  - In Scope/Out of Scope Allocation of S and Storage (S) Elements
  - Dependencies of S components on Prototype Components
  - Disposition Of Prototype  `bplib mpool` APIs to `dtn-cfs bplib` `S` APIs
  - Migration of `mpool` FSW Files to `S` FSW Files
  - Divvying up of `BPLIB_MEM` source and unit test files to `S` and `stor` (Decoupling)

### DTN CDR Design Slide Excerpts

  - Provides BPA modules with fast, fragmentation-free volatile storage for bundle data, queues, FIFOs, indices, etc. [Without implementing queues, FIFOs, and indices in this issue.]
  - Not directly accessible by AA or outside node
  - Predictable free/alloc execution times
  - Pre-set total memory usage - less than total amount of system DRAM
  - Memory pool is pre-allocated with fixed block sizes in a memory space that BPNode creates
  - Not dynamically allocated/deallocated
  - Blocks are allocated from existing pool and returned/recycled when no longer needed


**The rest of the content was copied from the MEM document.**

TODO - S Changes are TBD.


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
#### DDependencies of MEM components on Prototype Components

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

_Table of BPLIB_MEM Functions Dispositioned to MEM and STOR_

| mpool to MEM | mpool to STOR | ALL mpool APIs and internal functions
|:- |:- |:- 
| _52 MEM APIs and Functions_ | _44 STOR APIs and Functions_ | _96 Total APIs and Functions_
| BPLib_MEM_bblock_canonical_alloc | | BPLib_MEM_bblock_canonical_alloc
| | BPLib_MEM_bblock_canonical_cast | BPLib_MEM_bblock_canonical_cast
| | BPLib_MEM_bblock_canonical_drop_encode | BPLib_MEM_bblock_canonical_drop_encode
| BPLib_MEM_bblock_canonical_get_content_length | | BPLib_MEM_bblock_canonical_get_content_length
| BPLib_MEM_bblock_canonical_get_content_offset | | BPLib_MEM_bblock_canonical_get_content_offset
| | BPLib_MEM_bblock_canonical_get_encoded_chunks | BPLib_MEM_bblock_canonical_get_encoded_chunks
| | BPLib_MEM_bblock_canonical_get_logical | BPLib_MEM_bblock_canonical_get_logical
| | BPLib_MEM_bblock_canonical_set_content_position | BPLib_MEM_bblock_canonical_set_content_position
| BPLib_MEM_bblock_cbor_alloc | | BPLib_MEM_bblock_cbor_alloc
| BPLib_MEM_bblock_cbor_append | | BPLib_MEM_bblock_cbor_append
| | BPLib_MEM_bblock_cbor_cast | BPLib_MEM_bblock_cbor_cast
| | BPLib_MEM_bblock_cbor_export | BPLib_MEM_bblock_cbor_export
| | BPLib_MEM_bblock_cbor_set_size | BPLib_MEM_bblock_cbor_set_size
| BPLib_MEM_bblock_primary_alloc | |  BPLib_MEM_bblock_primary_alloc
| | BPLib_MEM_bblock_primary_append  |  BPLib_MEM_bblock_primary_append
| | BPLib_MEM_bblock_primary_cast | BPLib_MEM_bblock_primary_cast
| | BPLib_MEM_bblock_primary_drop_encode | BPLib_MEM_bblock_primary_drop_encode
| BPLib_MEM_bblock_primary_get_canonical_list | | BPLib_MEM_bblock_primary_get_canonical_list
| | BPLib_MEM_bblock_primary_get_encoded_chunks | BPLib_MEM_bblock_primary_get_encoded_chunks
| | BPLib_MEM_bblock_primary_get_logical | BPLib_MEM_bblock_primary_get_logical
| | BPLib_MEM_bblock_primary_locate_canonical | BPLib_MEM_bblock_primary_locate_canonical
| | BPLib_MEM_block_from_external_id | BPLib_MEM_block_from_external_id
| BPLib_MEM_collect_blocks  | | BPLib_MEM_collect_blocks
| BPLib_MEM_create  | | BPLib_MEM_create
| BPLib_MEM_debug_print_list_stats | | BPLib_MEM_debug_print_list_stats
| BPLib_MEM_debug_scan | | BPLib_MEM_debug_scan
| BPLib_MEM_dereference  | |  BPLib_MEM_dereference
| BPLib_MEM_extract_node  | | BPLib_MEM_extract_node
| BPLib_MEM_flow_alloc | | BPLib_MEM_flow_alloc
| | BPLib_MEM_flow_cast | BPLib_MEM_flow_cast
| | BPLib_MEM_flow_disable | BPLib_MEM_flow_disable
| | BPLib_MEM_flow_enable | BPLib_MEM_flow_enable
| | BPLib_MEM_flow_is_up | BPLib_MEM_flow_is_up
| | BPLib_MEM_flow_modify_flags | BPLib_MEM_flow_modify_flags
| | BPLib_MEM_flow_try_move_all | BPLib_MEM_flow_try_move_all
| | BPLib_MEM_flow_try_pull | BPLib_MEM_flow_try_pull
| | BPLib_MEM_flow_try_push | BPLib_MEM_flow_try_push
| BPLib_MEM_foreach_item_in_list  |  | BPLib_MEM_foreach_item_in_list
| BPLib_MEM_generic_data_alloc  |  | BPLib_MEM_generic_data_alloc
| | BPLib_MEM_generic_data_cast  |  BPLib_MEM_generic_data_cast
| | BPLib_MEM_generic_data_uncast  |  BPLib_MEM_generic_data_uncast
| BPLib_MEM_get_block_from_link  | | BPLib_MEM_get_block_from_link
| BPLib_MEM_get_external_id  |  | BPLib_MEM_get_external_id
| BPLib_MEM_get_generic_data_capacity  | | BPLib_MEM_get_generic_data_capacity
| BPLib_MEM_get_next_block  | | BPLib_MEM_get_next_block
| BPLib_MEM_get_parent_pool_from_link | |  BPLib_MEM_get_parent_pool_from_link
| BPLib_MEM_get_prev_block  | | BPLib_MEM_get_prev_block
| | BPLib_MEM_get_user_content_size | BPLib_MEM_get_user_content_size
| | BPLib_MEM_GetUserDataOffsetByBlocktype  |  BPLib_MEM_GetUserDataOffsetByBlocktype
| BPLib_MEM_init_list_head  | | BPLib_MEM_init_list_head
| BPLib_MEM_init_secondary_link  |  BPLib_MEM_init_secondary_link
| BPLib_MEM_insert_after  | | BPLib_MEM_insert_after
| BPLib_MEM_insert_before  | | BPLib_MEM_insert_before
| BPLib_MEM_is_any_content_node  | | BPLib_MEM_is_any_content_node
| BPLib_MEM_is_empty_list_head  | | BPLib_MEM_is_empty_list_head
| BPLib_MEM_is_generic_data_block  | | BPLib_MEM_is_generic_data_block
| BPLib_MEM_is_indirect_block  | | BPLib_MEM_is_indirect_block
| BPLib_MEM_is_link_attached  | | BPLib_MEM_is_link_attached
| BPLib_MEM_is_link_unattached  | | BPLib_MEM_is_link_unattached
| BPLib_MEM_is_list_head  | | BPLib_MEM_is_list_head
| BPLib_MEM_is_nonempty_list_head  | | BPLib_MEM_is_nonempty_list_head
| BPLib_MEM_is_secondary_index_node  | | BPLib_MEM_is_secondary_index_node
| | BPLib_MEM_job_cast | BPLib_MEM_job_cast
| | BPLib_MEM_job_get_next_active | BPLib_MEM_job_get_next_active
| | BPLib_MEM_job_init | BPLib_MEM_job_init
| | BPLib_MEM_job_mark_active | BPLib_MEM_job_mark_active
| | BPLib_MEM_job_run_all | BPLib_MEM_job_run_all
| BPLib_MEM_list_iter_forward  |  | BPLib_MEM_list_iter_forward
| BPLib_MEM_list_iter_goto_first  | | BPLib_MEM_list_iter_goto_first
| BPLib_MEM_list_iter_goto_last  | | BPLib_MEM_list_iter_goto_last
| BPLib_MEM_list_iter_reverse  | | BPLib_MEM_list_iter_reverse
| BPLib_MEM_lock_init  | | BPLib_MEM_lock_init
| BPLib_MEM_maintain  | | BPLib_MEM_maintain
| BPLib_MEM_merge_list  | |  BPLib_MEM_merge_list
| BPLib_MEM_query_mem_current_use  | |  BPLib_MEM_query_mem_current_use
| BPLib_MEM_query_mem_max_use  | | BPLib_MEM_query_mem_max_use
| BPLib_MEM_read_refcount  | | BPLib_MEM_read_refcount
| BPLib_MEM_recycle_all_blocks_in_list  | | BPLib_MEM_recycle_all_blocks_in_list
| BPLib_MEM_recycle_block  | | BPLib_MEM_recycle_block
| BPLib_MEM_ref_create  | | BPLib_MEM_ref_create
| BPLib_MEM_ref_duplicate  | | BPLib_MEM_ref_duplicate
| BPLib_MEM_ref_from_block  | | BPLib_MEM_ref_from_block
| BPLib_MEM_ref_make_block  | | BPLib_MEM_ref_make_block
| BPLib_MEM_ref_release  | | BPLib_MEM_ref_release
| | BPLib_MEM_register_blocktype | BPLib_MEM_register_blocktype
| | BPLib_MEM_search_list | BPLib_MEM_search_list
| | BPLib_MEM_start_stream_init | BPLib_MEM_start_stream_init
| | BPLib_MEM_stream_attach | BPLib_MEM_stream_attach
| | BPLib_MEM_stream_close | BPLib_MEM_stream_close
| | BPLib_MEM_stream_read | BPLib_MEM_stream_read
| | BPLib_MEM_stream_seek | BPLib_MEM_stream_seek
| | BPLib_MEM_stream_tell | BPLib_MEM_stream_tell
| | BPLib_MEM_stream_write | BPLib_MEM_stream_write
| | BPLib_MEM_subq_get_depth | BPLib_MEM_subq_get_depth
| | BPLib_MEM_subq_workitem_may_pull | BPLib_MEM_subq_workitem_may_pull
| | BPLib_MEM_subq_workitem_may_push | BPLib_MEM_subq_workitem_may_push

The mpool to MEM functions map to, or are part-of the MEM API as follows:

| mpool -> MEM | MEM API or MEM Internal Class__Method |
|:- |:-
| | 8 APIs, 44 Internal Class__Method
| BPLib_MEM_bblock_canonical_alloc | BPLib_MEM_Bblock_Canonical_Alloc
| BPLib_MEM_bblock_canonical_get_content_length | BPLib_MEM_Bblock_Canonical__Get_Content_Length
| BPLib_MEM_bblock_canonical_get_content_offset | BPLib_MEM_Bblock_Canonical__Get_Content_Offset
| BPLib_MEM_bblock_cbor_alloc | BPLib_MEM_Bblock_CBOR_Alloc
| BPLib_MEM_bblock_primary_alloc  |  BPLib_MEM_Bblock_Primary_Alloc
| BPLib_MEM_bblock_primary_append  |  BPLib_MEM_Bblock_Primary__Append
| BPLib_MEM_bblock_primary_get_canonical_list | BPLib_MEM_Bblock_Primary__Get_Canonical_List
| BPLib_MEM_collect_blocks  | BPLib_MEM__CollectBlocks
| BPLib_MEM_create  |  BPLib_MEM_Create
| BPLib_MEM_debug_print_list_stats | BPLib_MEM_Debug__Print_List_Stats
| BPLib_MEM_debug_scan | BPLib_MEM_Debug__Scan
| BPLib_MEM_dereference  |  BPLib_MEM__Dereference
| BPLib_MEM_extract_node  |  BPLib_MEM__Extract_Node
| BPLib_MEM_flow_alloc  |  BPLib_MEM_Duct_Alloc
| BPLib_MEM_foreach_item_in_list  |  BPLib_MEM__Foreach_Item_In_List
| BPLib_MEM_generic_data_alloc  |  BPLib_MEM_Generic_Data_Alloc
| BPLib_MEM_get_block_from_link  |  BPLib_MEM__Get_Block_From_Link
| BPLib_MEM_get_external_id  |  BPLib_MEM__Get_External_ID
| BPLib_MEM_get_generic_data_capacity  |  BPLib_MEM__Get_Generic_Data_Capacity
| BPLib_MEM_get_next_block  |  BPLib_MEM__Get_Next_Block
| BPLib_MEM_get_parent_pool_from_link  |  BPLib_MEM__Get_Parent_Pool_From_Link
| BPLib_MEM_get_prev_block  |  BPLib_MEM__Get_Prev_Block
| BPLib_MEM_init_list_head  |  BPLib_MEM__Init_List_Head
| BPLib_MEM_init_secondary_link  |  BPLib_MEM__Init_Secondary_Link
| BPLib_MEM_insert_after  |  BPLib_MEM__Insert_After
| BPLib_MEM_insert_before  |  BPLib_MEM__Insert_Before
| BPLib_MEM_is_any_content_node  |  BPLib_MEM__Is_Any_Content_Node
| BPLib_MEM_is_empty_list_head  |  BPLib_MEM__Is_Empty_List_Head
| BPLib_MEM_is_generic_data_block  |  BPLib_MEM__Is_Generic_Data_Block
| BPLib_MEM_is_indirect_block  |  BPLib_MEM__Is_Indirect_Block
| BPLib_MEM_is_link_attached  |  BPLib_MEM__Is_Link_Attached
| BPLib_MEM_is_link_unattached  |  BPLib_MEM__Is_Link_Unattached
| BPLib_MEM_is_list_head  |  BPLib_MEM__Is_List_Head
| BPLib_MEM_is_nonempty_list_head  |  BPLib_MEM__Is_Nonempty_List_Head
| BPLib_MEM_is_secondary_index_node  |  BPLib_MEM__Is_Secondary_Index_Node
| BPLib_MEM_list_iter_forward  |  BPLib_MEM__List_Iter_Forward
| BPLib_MEM_list_iter_goto_first  |  BPLib_MEM__List_Iter_Goto_First
| BPLib_MEM_list_iter_goto_last  |  BPLib_MEM__List_Iter_Goto_Last
| BPLib_MEM_list_iter_reverse  |  BPLib_MEM__List_Iter_Reverse
| BPLib_MEM_lock_init  |  BPLib_MEM__Lock_Init
| BPLib_MEM_maintain  |  BPLib_MEM__Maintain
| BPLib_MEM_merge_list  |  BPLib_MEM__Merge_List
| BPLib_MEM_query_mem_current_use  |  BPLib_MEM__Query_Mem_Current_Use
| BPLib_MEM_query_mem_max_use  |  BPLib_MEM__Query_Mem_Max_Use
| BPLib_MEM_read_refcount  |  BPLib_MEM__Read_Refcount
| BPLib_MEM_recycle_all_blocks_in_list  |  BPLib_MEM_Recycle_All_Blocks_In_List
| BPLib_MEM_recycle_block  |  BPLib_MEM_Recycle_Block
| BPLib_MEM_ref_create  |  BPLib_MEM__Ref_Create
| BPLib_MEM_ref_duplicate  |  BPLib_MEM__Ref_Duplicate
| BPLib_MEM_ref_from_block  |  BPLib_MEM__Ref_From_Block
| BPLib_MEM_ref_make_block  |  BPLib_MEM__Ref_Make_Block
| BPLib_MEM_ref_release  |  BPLib_MEM__Ref_Release

#### Migration of `mpool` FSW Files to `mem` FSW Files

The  `mpool` FSW files migrate to `mem` files through renaming according to the convention established for `dtn-cfs` FSW filenames.
  - Change the prefix `v7_mpool` to `bplib_mem`.
  - Change the test filename convention from `test<component-name>.[c|h]`to `bplib_mem_test<component-name>.[c|h]` (with minor discretionary variations)
  - Defer `job` and `stream` components to STOR.
  
| mpool tree baseline   | mem tree baseline    | mem tree final    |
| --- | --- | --- |
mpool baseline:br:<br>├── CMakeLists.txt:br:<br> ├── inc:br:<br> │   ├── v7_mpool.h:br:<br> │   ├── v7_mpool_bblocks.h:br:<br> │   ├── v7_mpool_flows.h:br:<br> │   ├── v7_mpool_job.h:br:<br> │   ├── v7_mpool_ref.h:br:<br> │   └── v7_mpstream.h:br:<br> ├── src:br:<br> │   ├── v7_mpool.c:br:<br> │   ├── v7_mpool_bblocks.c:br:<br> │   ├── v7_mpool_flows.c:br:<br> │   ├── v7_mpool_internal.h:br:<br> │   ├── v7_mpool_job.c:br:<br> │   ├── v7_mpool_ref.c:br:<br> │   └── v7_mpstream.c:br:<br> ├── ut-coverage:br:<br> │   ├── CMakeLists.txt:br:<br> │   ├── test_BPLib_MEM.h:br:<br> │   ├── test_BPLib_MEM_setup.c:br:<br> │   ├── test_bplib_v7_mpool.c:br:<br> │   ├── test_bplib_v7_mpool_bblocks.c:br:<br> │   ├── test_bplib_v7_mpool_flows.c:br:<br> │   ├── test_bplib_v7_mpool_job.c:br:<br> │   ├── test_bplib_v7_mpool_ref.c:br:<br> │   └── test_bplib_v7_mpstream.c:br:<br> └── ut-stubs:br:<br>     ├── CMakeLists.txt:br:<br>     ├── v7_mpool_bblocks_stubs.c:br:<br>     ├── v7_mpool_flows_stubs.c:br:<br>     ├── v7_mpool_job_stubs.c:br:<br>     ├── v7_mpool_ref_stubs.c:br:<br>     ├── v7_mpool_stubs.c:br:<br>     └── v7_mpstream_stubs.c :br:<br>:br:<br>:br:<br>:br:<br>|  mem baseline:br:<br> ├── CMakeLists.txt:br:<br> ├── inc:br:<br> │   └── bplib_mem.h:br:<br> │:br:<br> │:br:<br> │:br:<br> │:br:<br> │:br:<br> ├── src:br:<br> │   └── bplib_mem.c:br:<br> │:br:<br> │:br:<br> │:br:<br> │:br:<br> │:br:<br> │:br:<br> └── unit-test:br:<br>     ├── CMakeLists.txt:br:<br>    │:br:<br>     │:br:<br>      ├── bplib_mem_test.c:br:<br>     │:br:<br>     │:br:<br>     │:br:<br>     │:br:<br>     │:br:<br>     ├── stubs:br:<br>      │:br:<br>     │:br:<br>     │:br:<br>     │:br:<br>     │:br:<br>       └── bplib_mem_stubs.c:br:<br>  │:br:<br>     └── utilities<br>&nbsp;&nbsp;&nbsp; ├── bplib_mem_test_utils.c<br>&nbsp;&nbsp;&nbsp; └── bplib_mem_test_utils.h |mem final:br:<br> ├── CMakeLists.txt:br:<br> ├── inc:br:<br> │   ├── bplib_mem.h:br:<br> │   ├── bplib_mem_bblocks.h:br:<br> │   ├── bplib_mem_ducts.h:br:<br> │   ├── STOR:br:<br> │   ├── bplib_mem_ref.h:br:<br> │   └── STOR:br:<br> ├── src:br:<br> │   ├── bplib_mem.c:br:<br> │   ├── bplib_mem_bblocks.c:br:<br> │   ├── bplib_mem_ducts.c:br:<br> │   ├── bplib_mem_internal.c:br:<br> │   ├── STOR:br:<br> │   ├── bplib_mem_ref.c:br:<br> │   └── STOR:br:<br> └── unit-test:br:<br>     ├── CMakeLists.txt:br:<br>     ├── bplib_mem_test.h:br:<br>     ├── bplib_mem_test_setup.c:br:<br>     ├── bplib_mem_test.c:br:<br>     ├── bplib_mem_test_bblocks.c:br:<br>     ├── bplib_mem_test_ducts.c:br:<br>     ├── STOR:br:<br>     ├── bplib_mem_ref.c:br:<br>     ├── STOR:br:<br>     ├── stubs:br:<br>     │   └── CMakeLists.txt:br:<br>     ├── bplib_mem_bblocks_stubs.c:br:<br>     ├── bplib_mem_ducts_stubs.c:br:<br>     ├── STOR:br:<br>     ├── bplib_mem_ref_stubs.c:br:<br>     ├── bplib_mem_stubs.c:br:<br>     ├── STOR:br:<br>     └── utilities:br:<br>&nbsp;&nbsp;&nbsp; ├── bplib_mem_test_utils.c:br:<br>&nbsp;&nbsp;&nbsp; └── bplib_mem_test_utils.h:br:<br>

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
bplib/ci/mem/unit-test/test_BPLib_MEM_setup.c
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
mv bpa/s/qm/unit-test/test_BPLib_MEM_ducts.c              bpa/s/qm/unit-test/test_bplib_qm_ducts.c
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


Update the .h filenames in the .c files.
