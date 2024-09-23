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
1. Add more unit tests
1. Reconcile bp_blocktype_t in v7_types.h with bplib_mpool_blocktype
1. Reconcile bplib/v7 with bplib/mem
1. Verify ut file layout and format
1. BPLib_MEM_BlocktypeFlat
1. Remove all STOR-flagged code
1. Verify usage of MEM_Block_t and MEM_Pool_t
1. Address all TODOs
1. Write module guide

### MEM Module Block Diagram

mem_block, mem_blocktype, mem_subq, mem_pool

### MEM Data Structures

#### Abstract Typedefs

**Initial Design**

**Integrated Design**

### MEM Functions

#### MEM Activity Diagrams

#### MEM Module API

**MEM API**
**Red/Black Tree (RBT) API**

#### MEM Internal API

### MEM Unit Test
