# bplib

[1. Overview](#1-overview)  
[2. Application Design](#2-application-design)  
[3. Build with Make](#3-build-with-make)  
[4. Protocol Compliance and Tailoring](#4-protocol-compliance-and-tailoring)  
[5. Bundle Formats](#5-bundle-formats)  
[6. Application Programming Interface](#6-application-programming-interface)  
[7. Storage Service](#7-storage-service)  

----------------------------------------------------------------------
## 1. Overview
----------------------------------------------------------------------

The Bundle Protocol library (bplib) implements a subset of the RFC5050 Bundle Protocol necessary for embedded space flight applications. The library uses the concept of a bundle channel to manage the process of encapsulating application data in bundles, and extracting application data out of bundles.  A channel specifies how the bundles are created (e.g. primary header block fields), and how bundles are processed (e.g. payloads extracted from payload block). Bplib contains no threads and relies entirely on the calling application for its execution context and implements a thread-safe synchronous blocking I/O model where requested operations will either block according to the provided timeout, or return an error code immediately if the operation cannot be performed. 

Bplib assumes the availability of a persistent queued storage system for managing the rate buffering that must occur between data and bundle processing. This storage system is provided at run-time by the application, which can either use its own or can use one of the bplib included storage services. In addition to the storage service, bplib needs an operating system interface provided at compile-time. By default a POSIX compliant operating systems interface is built with the included makefile - see below for further instructions on changing the operating system interface.

----------------------------------------------------------------------
## 2. Application Design
----------------------------------------------------------------------

![Figure 1](doc/bp_api_architecture.png "BP Library API (Architecture)")

Bplib is written in "vanilla C" and is intended to be linked in as either a shared or static library into an application with an ABI for reading/writing application data and reading/writing bundles.  

Conceptually, the library is meant to exist inside a board support package for an operating system and be presented to the application as a service.  In such a design only the interface to reading/writing data would be provided to the application, and the interface for reading/writing bundles would be kept inside the board support package.  This use case would look a lot like a typical socket application where a bundle channel (socket) is opened, data is read/written to/from it, and then at some later time the channel is closed.  Underneath the operating system (via the board support packet) would take care of sending and receiving the bundles.

In order to support bplib being used directly by the application, both the data and the bundle interfaces are provided in the ABI. In these cases, the application is also responsible for sending and receiving the bundles.

An example applicatino design that manages both the data and bundle interfaces could look as follows:
1. A __bundle reader__ thread that receives bundles from a convergence layer and calls bplib to _processes_ them
2. A __data writer__ thread that _accepts_ application data from bplib
3. A __bundle writer__ thread that _loads_ bundles from bplib and sends bundles over a convergence layer
4. A __data reader__ thread that _stores_ application data to bplib

The stream of bundles received by the application is handled by the bundle reader and data writer threads. The __bundle reader__ uses the `bplib_process` function to pass bundles read from the convergence layer into the library.  If those bundles contain payload data bound for the application, that data is pulled out of the bundles and queued in storage until the __data writer__ thread calls the `bplib_accept` function to dequeue the data out of storage and write it to the application.  

Conversely, the stream of bundles sent by the application is handled by the data reader and bundler writer threasd. The __data reader__ thread calls `bplib_store` to pass data from the application into the library to be bundled.  Those bundles are queued in storage until the __bundle writer__ threads calls the `bplib_load` function to dequeue them out of storage and write them to the convergence layer.

----------------------------------------------------------------------
## 3. Build with Make
----------------------------------------------------------------------

Go to repository root directory and execute the following commands:
* `make`
* `sudo make install`

This will build the following binaries:
* `build/libbp.so.<version>` - shared library
* `build/libbp.a` - static library

And perform the following installations:
* `/usr/local/lib`: bplib libraries
* `/usr/local/inc`: bplib includes

Additional make commands are as follows:
* `make clean` will remove all generated files and directories
* `make prep` will create the subdirectories used when building the code
* `make lib` will compile just the library with dependencies only mapping to the source files.

On CentOS you may need to create a file with the conf extension in /etc/ld.so.conf.d that contains the line '/usr/local/lib'.
* `sudo echo "/usr/local/lib" > /etc/ld.so.conf.d/local.conf`
* `sudo ldconfig` 


----------------------------------------------------------------------
## 4. Protocol Compliance and Tailoring
----------------------------------------------------------------------

#### 4.1 Endpoint IDs
* Compressed Bundle Header Encoding (CBHE) is used on all generated data bundles.  
* The only addressing schema supported by the BP library is the "ipn" schema defined in RFC6260.  
* The format of endpoint IDs as defined by this schema are "ipn:{node}.{service}" where {node} and {service} are unsigned integers encoded as SDNVs in the bundle header block. 
Constant static data structures declared in the code set the size of all {node} numbers to 28 bits (which occupies 32 bits of bundle memory encoded as an SDNV), and all {service} numbers to 14 bits (which occupies 16 bits of bundle memory encoded as an SDNV).
* The convention used by the BP library is that {node} number identifies the endpoint application agent and the {service} number identifies a flow of data that is able to be processed by the application agent.
* All endpoints are treated as singletons and marked as such in the PCF, even for forwarded bundles.

#### 4.2 Fixed Size SDNVs
* The BP library uses fixed sized SDNVs for all SDNV fields of generated bundles. 
* Bundles that are received by the library can have any size SDNV as long as the encoded value fits within 32-bits (future enhancement to make compile time switch for 64-bits).

#### 4.3 Creation Time Sequence
* The BP library does not reset the creation time sequence number, but maintains a strictly incrementing value (stepped by one for each successive bundle created) per channel. This allows receiving nodes to detect gaps in the data and allows reconstruction of the original sending order of the data.

#### 4.4 Lifetimes
* The BP library calculates an absolute time for a bundles expiration at the time the bundle is created.
* The expiration time is checked prior to transmission and on receipt (in `bplib_load` and `bplib_process` functions).
* A value of zero has special meaning when using bplib and specifies an infinite lifetime, this is different than the protocol specification which interprets zero as an immediate expiration.  In order to be compatible with other BP nodes, 2^32 - 1 should be used to set the maximum lifetime (136 years).

#### 4.5 Administrative Records
* The only supported administrative record type is the aggregate custody signal, all other record types are ignored.
* The report to EID for bundles generated by this library is set to the null EID.

#### 4.6 DTN Aggregate Custody Signaling
* The BP library only supports custody transfers when DTN Aggregate Custody Signals are used.  Regular RFC5050 custody signals are not supported. A custody signal bundle received by the library is ignored.
* A data bundle received by the library that is requesting custody transfer without a CTEB is dropped.  
* ACS acknowledgments are delivered to the “custodian” node and service provided in the bundle.
* The rate that ACS bundles are sent is controlled via a receiver node configuration setting.  The BP library has a run-time defined maximum number of fills per ACS, and a dynamically configurable setting for the longest time a partially full ACS bundle can remain in memory before it is sent.
* Only positive acknowledgements are generated (i.e. ACS bundles mark the appropriate fields indicating positive acknowledgment of custody transfer). This means that the lifetime of a bundle and the timeout setting of the sending bundle agent completely define the bundle’s retransmission behavior.

#### 4.7 Bundle Integrity
* The BP library inserts a BIB in all data bundles it generates and all data bundles it accepts custody of for forwarding.  
* Bundles received by the library are not required to have BIBs, but if they do, then the bundle is verified against the BIB prior to accepting custody (if requested) and processing the payload (if destined for the local node).

#### 4.8 Timeouts
* Timeout processing is not fully implemented.  The library keeps track of active bundles (i.e. bundles that have been sent but not acknowledged) in a circular buffer of bundle references that maintain strictly incrementing custody IDs. When the application requests a bundle to be loaded, only the oldest active bundle is checked for a timeout.  
* If the circular buffer wraps and the oldest active bundle has not timed out, then onboard transmission of bundles will either: 
  1. Stop until the oldest bundle times out, 
  2. Force an immediate timeout of the oldest bundle regardless of its actual timeout period
  3. Delete the oldest bundle and continue transmitting new bundles.  
* The default behavior is option 2 - to force an immediate time.  This can be configured at run-time.
* The implication of using option 1 is that if large timeouts are used, it is possible for the entire process to be held up by a bundle that has not yet been acknowledged even though many other bundles sent after it have.
* The implication of using option 3 is the loss of bundles outside the bundle protocol parameters.

#### 4.9 Class of Service
* The class of service of generated data bundles and received bundles is set to normal and otherwise completely ignored.

#### 4.10 Fragmentation
* Custody transfer is requested per fragment.  In other words, each fragment has its own CTEB.
* A fragment of a bundle that requests custody transfer, and is sent to an intermediary hop, is acknowledged and forwarded by that hop without requiring the entire original bundle to be reconstituted.  Even though it is not required, there is also no assumed constraint on the fragment being further fragmented, or combined with other fragments if they are available.
* Bundle integrity via the BIB is always applied to each fragment.  If the library generates fragmented bundles, it will put a BIB in each fragment.  If a bundle with a BIB is received that is to be forwarded, and needs to be fragmented, then the BIB is checked upon receipt, and then that original BIB is dropped.  The outgoing fragmented bundles will all have their own BIB generated by the library.  This applies even when the BIB block flags indicate that the block is to be kept.

#### 4.11 Routing
* If a BP library channel receives bundles to be forwarded then the pre-build bundle headers are marked as dirty and rebuilt when the local node generates bundles of its own to be sent.  The result is a temporary loss of performance. 
* When a call to the BP library returns a bundle, it will set a flag if the bundle needs to be routed to a different destination than the channel’s default destination.
* When routing bundles, the BP library will associate any bundle with a destination {node} that matches its local {node} number as being destined for itself and will then require that the {service} number matches the channel's {service} number that it is being processed on.  Any bundle with a {node} that is different than its local {node} number is treated as a bundle that needs to be routed.
* A service number of 0 is considered a global service and is allowed to be passed to any channel associated with the node.

#### 4.12 Payload Block
* The library assumes that the last block of every bundle is the payload block and stops processing the bundle when the payload block is reached.

----------------------------------------------------------------------
## 5. Bundle Formats
----------------------------------------------------------------------

#### 5.1 Data Bundle 
```
 -------------------------------------------------
                    Data Bundle
 -------------------------------------------------
 |-----------------------------------------------|    
 |                                               |
 |              Primary Bundle Block             |
 |                                               |
 |-----------------------------------------------|    
 |                                               |
 |        Custody Transfer Extension Block       |
 |                                               |
 |-----------------------------------------------|    
 |                                               |
 |             Bundle Integrity Block            |
 |                                               |
 |-----------------------------------------------|    
 |                                               | 
 |              Bundle Payload Block             |
 |                                               |
 |-----------------------------------------------| 
```

#### 5.2 DTN Aggregate Custody Signal (DACS)
```
 -------------------------------------------------
             Aggregate Custody Signal
 -------------------------------------------------
 |-----------------------------------------------|    
 |                                               |
 |              Primary Bundle Block             |
 |                                               |
 |-----------------------------------------------|
 |                                               |
 |             Bundle Integrity Block            |
 |                                               |
 |-----------------------------------------------|   
 |                                               |
 |              Bundle Payload Block             |
 |    (ACS as administrative record in payload)  |
 |                                               |
 |-----------------------------------------------|
```

#### 5.3 Primary Bundle Block: Compressed Bundle Header Encoding (CBHE)
```
 -------------------------------------------------
               Primary Bundle Block
 -------------------------------------------------
 |    MSB    |           |           |    LSB    |
 | (8 bits)  | (8 bits)  | (8 bits)  | (8 bits)  |
 |-----------|-----------|-----------|-----------| 0
 |  Version  |     Processing Control Flags      | 
 |-----------|-----------------------------------| 4
 |  Blk Len  |         Destination Node ...       
 |-----------|-----------------------|-----------| 8
      ...    |  Destination Service  |    ...     
 |-----------|-----------------------|-----------| 12
          ... Source Node            |   Source     
 |-----------|-----------------------|-----------| 16
    Service  |         Report To Node ...         
 |-----------|-----------------------|-----------| 20
      ...    |   Report To Service   |    ...     
 |-----------|-----------------------|-----------| 24
          ... Custody Node           |  Custody     
 |-----------|-----------------------|-----------| 28
    Service  |    Creation Timestamp Seconds             
 |-----------|-----------------------------------| 32
             |   Timestamp Sequence  |    ...
 |-----------|-----------------------------------| 36
                 ... Lifetime ...
 |-----------|-----------|-----------------------| 40
     ...     |  Dict Len |  Fragment Offset ...
 |-----------|-----------|-----------------------| 44
     ...                 | Payload Length ... 
 |-----------------------|-----------------------| 48
     ...                 |
 |-----------------------| 52
```

#### 5.4 Custody Transfer Enhancement Block (CTEB)
```
 -------------------------------------------------
        Custody Transfer Enhancement Block
 -------------------------------------------------
 |    MSB    |           |           |    LSB    |
 | (8 bits)  | (8 bits)  | (8 bits)  | (8 bits)  |
 |-----------|-----------|-----------|-----------|
 |                                               |
 |              Primary Bundle Block             |
 |                                               |
 |-----------|-----------|-----------------------|
 .           .           .                       .
 |-----------|-----------|-----------|-----------| 0 --> Custody Transfer Enhancement Block
 |    0xA    | Blk Flags | Blk Len   |  ...
 |-----------|-----------|-----------|-----------| 4
              Custody ID             |  ...
 |-----------------------|-----------------------| 8
 |    Custodian EID (null-terminated string)     |
 |-----------------------------------------------| Variable
```

#### 5.5 Bundle Integrity Block (BIB)
```
 -------------------------------------------------
              Bundle Integrity Block
 -------------------------------------------------
 |    MSB    |           |           |    LSB    |
 | (8 bits)  | (8 bits)  | (8 bits)  | (8 bits)  |
 |-----------|-----------|-----------|-----------|
 |                                               |
 |              Primary Bundle Block             |
 |                                               |
 |-----------|-----------|-----------------------|
 .           .           .                       .
 |-----------|-----------|-----------------------| 0 --> Bundle Integrity Block
 |    0xD    | Blk Flags |     Block Length      | 
 |-----------|-----------|-----------------------| 4 
 |    Payload CRC Type   |      Payload CRC      |
 |-----------------------------------------------| 8
```

#### 5.6 Aggregate Custody Signal Block
```
 -------------------------------------------------
         DTN Aggregate Custody Signal Block
 -------------------------------------------------
 |    MSB    |           |           |    LSB    |
 | (8 bits)  | (8 bits)  | (8 bits)  | (8 bits)  |
 |-----------|-----------|-----------|-----------|
 |                                               |
 |              Primary Bundle Block             |
 |                                               |
 |-----------|-----------|-----------------------|
 .           .           .                       .
 |-----------|-----------|-----------------------| 0 --> Payload BLock
 |    0x1    | Blk Flags |     Block Length ...    
 |-----------|-----------|-----------|-----------| 4 --> Aggregate Custody Signal
            ...          |    0x40   |  Status   | 
 |-----------|-----------|-----------|-----------| 8
 |                First Custody ID               |
 |-----------------------|-----------------------| 12
 |  Number Valid CIDs    |  Number Skipped CIDs  |
 |-----------------------|-----------------------| 
 |  Number VAlid CIDs    |  Number Skipped CIDs  |
 |-----------------------|-----------------------| 
 .                       .                       .
 |-----------------------|-----------------------|
 |  Number Valid CIDs    |  Number Skipped CIDs  |
 |-----------------------------------------------| Variable
```

----------------------------------------------------------------------
## 6. Application Programming Interface
----------------------------------------------------------------------

#### 6.1 Functions

| Function        | Purpose |
| --------------- | ------- | 
| [bplib_init](#initialize)             | Initialize the BP library - called once at program start |
| [bplib_open](#open-channel)           | Open a channel - provides handle to channel for future channel operations |
| [bplib_close](#close-channel)         | Close a channel |
| [bplib_flush](#flush-channel)         | Flush active bundles on a channel |
| [bplib_config](#config-channel)       | Change and retrieve channel settings |
| [bplib_latchstats](#latch-statistics) | Read out bundle statistics for a channel |
| [bplib_store](#store-payload)         | Create a bundle from application data and queue in storage for transmission |
| [bplib_load](#load-bundle)            | Retrieve the next available bundle from storage to transmit |
| [bplib_process](#process-bundle)      | Process a bundle for data extraction, custody acceptance, and/or forwarding |
| [bplib_accept](#accept-payload)       | Retrieve the next available data payload from a received bundle |
| [bplib_routeinfo](#route-information) | Parse bundle and return routing information |
| [bplib_eid2ipn](#eid-to-ipn)          | Utility function to translate an EID string into node and service numbers |
| [bplib_ipn2eid](#ipn-to-eid)          | Utility function to translate node and service numbers into an EID string |
| [bplib_attrinit](#attr-init)          | Utility to initialize a channel attribute structure with default values.  Useful if the calling application only wants to change a few attributes without setting them all. |

__Note__: _functions that operate on a channel are thread-safe with other functions that operate on channels, but they are not thread-safe with the open and close functions.  A channel can only be closed when no other operations are being performed on it._


----------------------------------------------------------------------
##### Initialize

`void bplib_init (void)`

Initializes the BP library.  This must be called before any other call to the library is made.  It calls the operating system layer initialization routine.

----------------------------------------------------------------------
##### Open Channel

`bp_desc_t bplib_open (bp_route_t route, bp_store_t store, bp_attr_t* attributes)`

Opens a bundle channel that uses the provided endpoint IDs, storage service, and attributes. 

This function returns a channel handle that is used for all future operations on the channel.  The open and close calls are mutex'ed against other open and close calls, but once a channel is created, operations on that channel are only mutex'ed against other operations on the same channel.  A channel persists until it is closed.

`route` - a set of endpoing IDs defining the source, destination, and report to endpoints

* __local node__: The {node} number of the ipn:{node}.{service} endpoint ID used for the source and custody endpoints of bundles generated on the channel.

* __local service__: The {service} number of the ipn:{node}.{service} endpoint ID used for the source and custody enpoints of bundles generated on the channel.

* __destination node__: The {node} number of the ipn:{node}.{service} endpoint ID used for the destination enpoint of bundles generated on the channel.

* __destination service__: The {service} number of the ipn:{node}.{service} endpoint ID used for the destination enpoint of bundles generated on the channel.

* __report to node__: The {node} number of the ipn:{node}.{service} endpoint ID used for the report to enpoint of bundles generated on the channel.

* __report to service__: The {service} number of the ipn:{node}.{service} endpoint ID used for the report to enpoint of bundles generated on the channel.

`store` - a set of callbacks that provide access to the desired storage service.  See [Storage Service](#storage-service) section for more details.

`attributes` - set of characteristics and settings for the channel that trade memory usage and performance

* __lifetime__: Bundle generation parameter - the number of seconds since its creation that the bundle is valid.  Once the lifetime of a bundle expires, the bundle can be deleted by the bundle agent.

* __request_custody__: Bundle generation parameter - if set then the bundle request custody transfer and includes a CTEB extension block.

* __admin_record__: Bundle generation parameter - if set then the bundle is set as an adminstrative record.  The library handles this setting automatically for Aggregate Custody Signals that it generates; but if the user wants to create their own adminstrative record, then this attribute provides that option.

* __integrity_check__: Bundle generation parameter - if set then the bundle includes a BIB extension block.

* __allow_fragmentation__: Bundle generation parameter - if set then any generated or forwarded bundles on the channel will be fragmented if the size of the bundle exceeds the __max_length__ attribute of the channel; if not set, then any bundle generated or forwarded that exceeds the __max_length__ will be dropped.

* __cipher_suite__: Bundle generation parameter - provides the CRC type used inside the BIB extension block.  If the __integrity_check__ attribute is not set, then this setting is ignored.  IF the __integrity_check__ attribute is set and this attribute is set to 0, then a BIB is included but the cipher result length is zero (this provide unambigous indication that no integrity check is included).

* __timeout__: The number of seconds the library waits before re-loading an unacknowledged bundle.

* __max_length__: The maximum size in bytes that a bundle can be, both on receipt and on transmission.

* __wrap_response__: The library's behavior when the active table wraps.  See the [Timeout](#4-8-timeouts) description in the Protocol Compliance and Tailoring section for additional details.

* __cid_reuse__: The library's behavior when a bundle times-out - if set, bundles that are retransmitted use the original Custody ID of the bundle when it was originally sent; if not set, then a new Custody ID is used when the bundle is retransmitted.  Re-using the Custody ID bounds the size of the Aggregrate Custody Signal coming back (worse-case gaps).  Using a new Custody ID makes the average size of the Aggregate Custody Signal smaller.

* __dacs_rate__: The maximum number of seconds to wait before an Aggregate Custody Signal which has accumulated acknowledgments is sent.  Every time a call to `bplib_load` is made, the code checks to see if there is an Aggregate Custody Signal which exists in memory but has not been sent for at least __dacs_rate__ seconds.

* __active_table_size__:  The number of unacknowledged bundles to keep track of. The larger this number, the more bundles can be sent before a "wrap" occurs (see BP_OPT_WRAP_RESPONSE).  But every unacknowledged bundle consumes 8 bytes of CPU memory making this attribute the primary driver for a channel's memory usage.

* __max_fills_per_dacs__: The maximum number of fills in the Aggregate Custody Signal.  An Aggregate Custody Signal is sent when the maximum fills are reached or the __dacs_rate__ period has expired (see BP_OPT_DACS_RATE).

* __max_gaps_per_dacs__: The maximum number of Custody ID gaps a channel can keep track up when receiving bundles requesting custody transfer.  If this gap limit is reached, the Aggregate Custody Signal is sent and a new one immediately begins to accumulate acknowledgments.

* __storage_service_parm__: A pass through to the storage service `create` function.

`returns` - channel descriptor. 

----------------------------------------------------------------------
##### Close Channel

`void bplib_close (bp_desc_t channel)`

Closes the specified bundle channel and releases all run-time resources associated with it; this does not include the bundles stored in the storage service; nor does it include bundles that have been transmitted but not yet acknowledged (active bundles).  The close call is not mutex'ed against other channel operations - it is the caller's responsibility that the close call is made non-concurrently with any other library function call on that channel.

`channel` - which channel to close

----------------------------------------------------------------------
##### Flush Channel

`int bplib_flush (bp_desc_t channel)`

Flushes all active bundles on a channel; this treats each bundle that has been transmitted but not yet acknowledged as if it was immediately acknowledged.  This function is separate from the bplib_close function because it is possible that a storage service supports resuming where it left off after a channel is closed.  In such a case, closing the channel would occur without flushing the data since the next time the channel was opened, the data that had not yet been relinquished would resume being sent.

`channel` - which channel to flush

----------------------------------------------------------------------
##### Config Channel

`int bplib_config (bp_desc_t channel, int mode, int opt, void* val, int len)`

Configures or retrieves an attribute on a channel.

`channel` - which channel to configure or retrieve attribute

`mode` - whether to read or write the attribute

* BP_OPT_MODE_READ: the attribute is read and placed into the memory localtion pointed to by _val_

* BP_OPT_MODE_WRITE: the attribute is written with the value stored at the memory location pointed to by _val_

`opt` - the attribute to perform the operation on, as described in the table below.  The different attributes that can be changed or read are further described in the [Open Channel](#open-channel) section.

| Option                 | Units    | Default | Description |
| ---------------------- | -------- | ------- | ----------- |
| BP_OPT_LIFETIME        | int      | 0 | Amount of time in seconds added to creation time specifying duration of time bundle is considered valid, 0: infinite |
| BP_OPT_REQUEST_CUSTODY | int      | 1 | Sets whether transmitted bundles request custody transfer, 0: false, 1: true |
| BP_OPT_ADMIN_RECORD    | int      | 0 | Sets whether generated bundles are administrative records, 0: false, 1: true |
| BP_OPT_INTEGRITY_CHECK | int      | 1 | Sets whether transmitted bundles include a BIB extension block, 0: false, 1: true |
| BP_OPT_ALLOW_FRAGMENTATION | int  | 1 | Sets whether transmitted bundles are allowed to be fragmented, 0: false, 1: true |
| BP_OPT_CIPHER_SUITE    | int      | BP_BIB_CRC16_X25 | The type of Cyclic Redundancy Check used in the BIB extension block - BP_BIB_NONE, BP_BIB_CRC16_X25, BP_BIB_CRC32_CASTAGNOLI |
| BP_OPT_TIMEOUT         | int      | 10 | Amount of time in seconds to wait for positive acknowledgment of transmitted bundles before retransmitting, 0: infinite |
| BP_OPT_MAX_LENGTH      | int      | 4096 | Maximum length of the transmitetd bundles |
| BP_OPT_WRAP_RESPONSE   | int      | BP_WRAP_RESEND | Sets behavior of library when the active table wraps - BP_WRAP_RESEND, BP_WRAP_BLOCK, BP_WRAP_DROP |
| BP_OPT_CID_REUSE       | int      | 0 | Sets whether retransmitted bundles reuse their original custody ID, 0: false, 1: true |
| BP_OPT_DACS_RATE       | int      | 5 | Sets minimum rate of ACS generation |

__NOTE__: _transmitted_ bundles include both bundles generated on the channel from local data that is stored, as well as bundles that are received and forwarded by the channel.

`val` - the value set or returned

`len` - the length in bytes of the memory pointed to by _val_

`returns` - [return code](#6.2-return-codes).

----------------------------------------------------------------------
##### Latch Statistics

`int bplib_latchstats (bp_desc_t channel, bp_stats_t* stats)`

Retrieve channel statistics populated in the structure pointed to by _stats_.

`channel` - channel to retrieve statistics on

`stats` - pointer to the statistics structure to be populated

* __lost__: number of deleted bundles due to: storage failure, and memory copy failure

* __expired__: number of deleted bundles due to their lifetime expiring

* __acknowledged__: number of deleted bundes due to a custody signal acknowledgment

* __transmitted__: number of bundles returned by the `bplib_load` function

* __retransmitted__: number of bundles returned by the `bplib_load` function because the bundle timed-out and is being resent

* __received__: number of bundles passed to the `bplib_process` function; includes bundles that are mal-formed and return an error

* __generated__: number of bundles successfully generated via the `bplib_store` function

* __delivered__: number of bundle payloads delivered to the application via the `bplib_accept` function

* __bundles__: number of data bundles currently in storage

* __payloads__: number of payloads currently in storage

* __records__: number of aggregate custody signal bundles currently in storage

* __active__: number of bundles that have been loaded for which no acknowledgment has been received

----------------------------------------------------------------------
##### Store Payload

`int bplib_store (bp_desc_t channel, void* payload, int size, int timeout, uint16_t* flags)`

Initiates sending the data pointed to by _payload_ as a bundle. The data will be encapsulated in a bundle (or many bundles if the channel allows fragmentation and the payload exceeds the maximum bundle length) and queued in storage for later retrieval and transmission.

`channel` - channel to create bundle on

`payload` - pointer to data to be bundled

`size` - size of payload in bytes

`timeout` - 0: check, -1: pend, 1 and above: timeout in milliseconds

`flags` - flags that provide additional information on the result of the store operation (see [flags](#6-3-flag-definitions)).  The flags variable is not initialized inside the function, so any value it has prior to the function call will be retained.

`returns` - size of bundle created in bytes, or [return code](#6.2-return-codes) on error.

----------------------------------------------------------------------
##### Load Bundle

`int bplib_load (bp_desc_t channel, void** bundle,  int size, int timeout, uint16_t* flags)`

Reads the next bundle from storage to be sent by the application over the convergence layer.  From the perspective of the library, once a bundle is loaded to the application, it is as good as sent.  Any failure of the application to send the bundle is treated no differently that a failure downstream in the bundle reaching its destination. 

`channel` - channel to retrieve bundle from

`bundle` - pointer to the buffer for bundle being loaded.  The pointer cannot be null, but if the dereferenced memory location for the pointer is null, then the library will dynamically allocate the memory needed for the bundle.  If the dereferenced memory location is not null, then it points to a buffer into which the bundle will be copied, and the __size__ value is used to indicate the size of the buffer.

`size` - the size in bytes of the bundle buffer being passed in, or zero if the bundle memory is to be dynamically allocated.

`timeout` - 0: check, -1: pend, 1 and above: timeout in milliseconds

`flags` - flags that provide additional information on the result of the load operation (see [flags](#6-3-flag-definitions)). The flags variable is not initialized inside the function, so any value it has prior to the function call will be retained.

`returns` - the size of the loaded bundle or [return code](#6.2-return-codes) on error.

----------------------------------------------------------------------
##### Process Bundle

`int bplib_process (bp_desc_t channel, void* bundle,  int size, int timeout, uint16_t* flags)`

Processes the provided bundle.

There are three types of bundles processed by this function:
(1) If the bundle is an aggregate custody signal, then any acknowledged bundles will be freed from storage.
(2) If the bundle is destined for the local node, then the payload data will be extracted and queued for retrieval by the application; and if custody is requested, then the current aggregate custody signal will be updated and queued for transmission if necessary.
(3) If the bundle is not destined for the local node, then the bundle will be queued for transmission as a forwarded bundle; and if custody is requested, then the current aggregate custody signal will be updated and queued for transmission if necessary.

`channel` - channel to process bundle on

`bundle` - pointer to a bundle

`size` - size of the bundle in bytes

`timeout` - 0: check, -1: pend, 1 and above: timeout in milliseconds

`flags` - flags that provide additional information on the result of the process operation (see [flags](#6-3-flag-definitions)). The flags variable is not initialized inside the function, so any value it has prior to the function call will be retained.

`returns` - [return code](#6.2-return-codes).

----------------------------------------------------------------------
##### Accept Payload

`int bplib_accept (bp_desc_t channel, void** payload, int size, int timeout, uint16_t* flags)`

Returns the next available bundle payload (from bundles that have been received and processed via the `bplib_process` function) to the application by populating the structure pointed to by the _payload_ pointer.

`channel` - channel to accept payload from

`payload` - pointer to the buffer for data being accepted.  The pointer cannot be null, but if the dereferenced memory location for the pointer is null, then the library will dynamically allocate the memory needed for the data.  If the dereferenced memory location is not null, then it points to a buffer into which the data will be copied, and the __size__ value is used to indicate the size of the buffer.

`size` - the size in bytes of the data buffer being passed in, or zero if the data memory is to be dynamically allocated.

`timeout` - 0: check, -1: pend, 1 and above: timeout in milliseconds

`flags` - flags that provide additional information on the result of the accept operation (see [flags](#6-3-flag-definitions)). The flags variable is not initialized inside the function, so any value it has prior to the function call will be retained.

`returns` - size of bundle payload in bytes, or [return code](#6.2-return-codes) on error.

----------------------------------------------------------------------
##### Route Information

`int bplib_routeinfo (void* bundle, int size, bp_route_t* route)`

Parses the provided bundle and supplies its endpoint ID node and service numbers.  Used to route a received bundle to the appropriate channel by looking up its destination endpoint ID prior to making other library calls that require a channel identifier.

`bundle` - pointer to a buffer of memory containing a properly formatted bundle

`size` - size of the bundle

`route` - pointer to a route structure that is populated by the function (see [Open Channel](#open-channel) for more details on the structure contents).

`returns` - [return code](#6.2-return-codes)

----------------------------------------------------------------------
##### EID to IPN

`int bplib_eid2ipn (const char* eid, int len, bp_ipn_t* node, bp_ipn_t* service)`

Convert a enpoint ID string into the IPN node and service numbers

`eid` - string containing the endpoint ID

`len` - length of the __eid__ string

`node` - pointer to variable that will be populated with the node number

`service` - pointer to the variable that will be populated with the service number

----------------------------------------------------------------------
##### IPN to EID

`int bplib_ipn2eid (char* eid, int len, bp_ipn_t node, bp_ipn_t service)`

Convert an IPN node and service number to an enpoint ID string

`eid` - pointer to a buffer that will be populated with the endpoint ID string

`len` - length of the __eid__ buffer

`node` - node number used to populate the endpoint ID string

`service` - service number used to populate the endpoint ID string

----------------------------------------------------------------------
##### Initialize Attributes

`int bplib_attrinit (bp_attr_t* attr)`

Initialize an attribute structure with the library default values.  This is useful when creating a channel where only a few attributes need to be changed.

`attr` - pointer to attributes structure populated by the function (see [Open Channel](#open-channel) for more details on the attributes structure contents).

----------------------------------------------------------------------
#### 6.2 Return Codes

| Code                    | Value | Description |
| ----------------------- | ----- | ----------- |
| BP_SUCCESS              | 1     | Operation successfully performed |
| BP_TIMEOUT              | 0     | A timeout occurred when a blocking operation was performed |
| BP_ERROR                | -1    | Generic error occurred; further information provided in flags to determine root cause |
| BP_PARMERR              | -2    | Parameter passed into function was invalid |
| BP_UNSUPPORTED          | -3    | A valid bundle could not be processed by the library because the requested functionality is not yet implemented |
| BP_EXPIRED              | -4    | A bundle expired due to its lifetime and was deleted |
| BP_DROPPED              | -5    | A bundle was dropped because it could not be processed |
| BP_INVALIDHANDLE        | -6    | The handle passed into a storage service function was invalid |
| BP_OVERFLOW             | -7    | An SDNV was not able to be written or read due to their being insufficient space in the variable or bundle buffer |
| BP_WRONGVERSION         | -8    | The primary block bundle version number did not match the CCSDS recommended version |
| BP_BUNDLEPARSEERR       | -9    | An error was encountered when trying to read or write a bundle, usually associated with either an SDNV overflow or a buffer that is too small |
| BP_UNKNOWNREC           | -10   | The administrative record type was unrecognized by the library |
| BP_BUNDLETOOLARGE       | -11   | The size of the bundle exceeded the maximum size bundle that is able to be processed by the library |
| BP_PAYLOADTOOLARGE      | -12   | The size of the payload exceeded the maximum size payload that is able to be processed by the library |
| BP_WRONGCHANNEL         | -13   | The destination service number did not match the channel's source service number when trying to processing a bundle destined for the local node |
| BP_FAILEDINTEGRITYCHECK | -14   | A bundle processed by the library contained a Bundle Integrity Block, but the checksum contained in that block did not match the calculated checksum |
| BP_FAILEDSTORE          | -15   | The library encountered an error originating from the storage service |
| BP_FAILEDOS             | -16   | The library encountered an error originating from the operation system abstraction layer |
| BP_FAILEDMEM            | -17   | The library encountered an error allocating memory for a channel |
| BP_FAILEDRESPONSE       | -18   | The library was unable to report back to another node, e.g. a DACS bundle could not be created or sent due to there being too many sources to track |
| BP_INVALIDEID           | -19   | An EID string did not contain a valid IPN address |
| BP_INVALIDCIPHERSUITEID   | -20   | A BIB block as an unrecognized Cipher Suite ID  |
| BP_PENDINGACKNOWLEDGMENT  | -21   | An aggregate custody signal was unable to be processed due to a library failure |
| BP_PENDINGCUSTODYTRANSFER | -22   | A bundle requesting custody transfer failed to be acknowledged due to a library failure |

----------------------------------------------------------------------
#### 6.3 Flag Definitions

| Flag                     | Value | Description |
| ------------------------ | ----- | ----------- |
| BP_FLAG_NONCOMPLIANT     | 0x0001 | Valid bundle but the library was not able to comply with the standard |
| BP_FLAG_INCOMPLETE       | 0x0002 | At least one block in bundle was not recognized |
| BP_FLAG_RESERVED01       | 0x0004 | Reserved |
| BP_FLAG_FILLOVERFLOW     | 0x0008 | A gap in the CIDs exceeds the max fill value allowed in an ACS bundle |
| BP_FLAG_TOOMANYFILLS     | 0x0010 | All the fills in the ACS are used |
| BP_FLAG_CIDWENTBACKWARDS | 0x0020 | The custody ID went backwards |
| BP_FLAG_ROUTENEEDED      | 0x0040 | The bundle returned needs to be routed before transmission |
| BP_FLAG_STOREFAILURE     | 0x0080 | Storage service failed to deliver data |
| BP_FLAG_RESERVED02       | 0x0100 | Reserved |
| BP_FLAG_SDNVOVERFLOW     | 0x0200 | There was insufficient room in 32-bit variable to read/write value |
| BP_FLAG_SDNVINCOMPLETE   | 0x0400 | There was insufficient room in block to read/write value |
| BP_FLAG_ACTIVETABLEWRAP  | 0x0800 | The active table wrapped; see BP_OPT_WRAP_RESPONSE |
| BP_FLAG_DUPLICATES       | 0x1000 | The custody ID was already acknowledged |
| BP_FLAG_RBTREEFULL       | 0x2000 | An aggregate custody signal was generated due the number of custody ID gaps exceeded the maximum allowed |

----------------------------------------------------------------------
## 7. Storage Service
----------------------------------------------------------------------

The application is responsible for providing the storage service to the library at run-time through call-backs passed to the `bplib_open` function.  

----------------------------------------------------------------------
##### Create Storage Service

`int create (void* parm)`

Creates a storage service.

`parm` - service specific parameters pass through library to this function.  See the storage_service_parm of the attributes structure passed to the `bplib_open` function.

`returns` - handle for storage service used in subsequence calls.

----------------------------------------------------------------------
##### Destroy Storage Service

`int destroy (int handle)`

Destroys a storage service.  This does not mean that the data stored in the service is freed - that is service specific.

`handle` - handle to the storage service

`returns` - [return code](#6.2-return-codes)

----------------------------------------------------------------------
##### Enqueue Storage Service

`int enqueue (int handle, void* data1, int data1_size, void* data2, int data2_size, int timeout)`

Stores the pointed to data into the storage service.

`handle` - handle to the storage service

`data1` - pointer to first block of memory to store.  This must be concatenated with __data2__ by the function into one continuous block of data.

`data1_size` - size of first block of memory to store.

`data2` - pointer to second block of memory to store.  This must be concatenated with __data1__ by the function into one continuous block of data.

`data2_size` - size of the second block of memory to store.

`timeout` - 0: check, -1: pend, 1 and above: timeout in milliseconds

`returns` - [return code](#6.2-return-codes)

----------------------------------------------------------------------
##### Dequeue Storage Service

`int dequeue (int handle, void** data, int* size, bp_sid_t* sid, int timeout)`

Retrieves the oldest data block stored in the storage service that has not yet been dequeued, and returns a _Storage ID_ that can be used to retrieve the data block in the future.

`handle` - handle to the storage service

`data` - the pointer that will be updated to point to the retrieved block of memory.  This function returns the data block via a pointer and performs no copy.  The data is still owned by the storage service and is only valid until the next dequeue or relinquish call.

`size` - size of data block being retrieved.

`sid` - pointer to a _Storage ID_ variable populated by the function.  The sid variable is used in future storage service functions to identify the retrieved data block.

`timeout` - 0: check, -1: pend, 1 and above: timeout in milliseconds

`returns` - [return code](#6.2-return-codes)

----------------------------------------------------------------------
##### Retrieve Storage Service

`int retrieve (int handle, void** data, int* size, bp_sid_t sid, int timeout)`

Retrieves the data block stored in the storage service identified by the _Storage ID_ sid parameter.

`handle` - handle to the storage service

`data` - the pointer that will be updated to point to the retrieved block of memory.  This function returns the data block via a pointer and performs no copy.  The data is still owned by the storage service and is only valid until the next dequeue or relinquish call.

`size` - size of data block being retrieved.

`sid` - the _Storage ID_ that identifies which data block to retrieve from the storage service

`timeout` - 0: check, -1: pend, 1 and above: timeout in milliseconds

`returns` - [return code](#6.2-return-codes)

----------------------------------------------------------------------
##### Relinquish Storage Service

`int relinquish (int handle, bp_sid_t sid)`

Deletes the stored data block identified by the _Storage ID_ sid parameter.

`handle` - handle to the storage service

`sid` - the _Storage ID_ that identifies which data block to delete from storage

`returns` - [return code](#6.2-return-codes)

----------------------------------------------------------------------
##### Get Count Storage Service

`int getcount (int handle)`

Returns the number of data blocks currently stored in the storage service.

`handle` - handle to the storage service

`returns` - number of data blocks

----------------------------------------------------------------------
The storage service call-backs must have the following characteristics:
* `enqueue`, `dequeue`, `retrieve`, and `relinquish` are expected to be thread safe against each other.
* `create` and `destroy` do not need to be thread safe against each other or any other function call - the application is responsible for calling them when it can complete atomically with respect to any other storage service call
* The memory returned by the dequeue function is valid only until the next `dequeue` function call or the next `relinquish` function call
* The _Storage ID (SID)_ returned by the storage service cannot be zero since that is marked as a _VACANT_ SID

 
