# bplib

[1. Overview](#1-overview)
[1. Build with Make](#3-build-with-make)
[2. Application Design](#2-application-design)
[4. Application Programming Interface](#4-application-programming-interface)
[5. Storage Service](#5-storage-service)

[Note #1 - Bundle Protocol Version 6](doc/bpv6_notes.md)
[Note #2 - Library Development Guidelines](doc/dev_notes.md)
[Note #3 - Configuration Parameter Trades](doc/parm_notes.md)
[Note #4 - Bundle Flow Analysis for Intermittent Communication](doc/perf_analysis_ic.md)

----------------------------------------------------------------------
## 1. Overview
----------------------------------------------------------------------

The Bundle Protocol library (bplib) implements a subset of the RFC5050 Bundle Protocol and targets embedded space flight applications. The library uses the concept of a bundle channel to manage the process of encapsulating application data in bundles, and extracting application data out of bundles.  A channel specifies how the bundles are created (e.g. primary header block fields), and how bundles are processed (e.g. payloads extracted from payload block). Bplib contains no threads and relies entirely on the calling application for its execution context and implements a thread-safe blocking I/O model where requested operations will either block according to the provided timeout, or return an error code immediately if the operation cannot be performed.

Bplib assumes the availability of a persistent queued storage system for managing the rate buffering that must occur between data and bundle processing. This storage system is provided at run-time by the application, which can either use its own or can use one of the included storage services. In addition to the storage service, bplib needs an operating system interface provided at compile-time. By default a POSIX compliant operating systems interface is built with the included makefile - see below for further instructions on changing the operating system interface.

----------------------------------------------------------------------
## 2. Build with Make
----------------------------------------------------------------------

#### Prerequisites

1. To build the static and shared libraries, the only prerequisites are the __make__ build system and a compiler toolchain (by default __gcc__).

2. To build the Lua extension used for unit testing (and useful for any user implemented Lua applications), then you need to have Lua installed on your system.  For Ubuntu, type `sudo apt install lua5.3`, and for CentOS, type `yum install epel-release && yum install lua`. Only Lua version 5.3 has been tested.

3. Tailoring the build to provide unique system prerequisites is accomplished by providing a custom configuration makefile.  See `posix.mk` as an example. If you custom file is called `{my_config_makefile}` then the following commands would be used:
   * `make CONFIG={my_config_makefile}`
   * `sudo make CONFIG={my_config_makefile} install`

#### Building

To build everything, including the language bindings, go to repository root directory and execute the following commands:
* `make`
* `sudo make install`

To build only the static and shared libraries, use the following commands:
* `make lib`
* `sudo make install-lib`

The default makefile produces the following binaries:
* `build/libbp.so.<version>` - shared library
* `build/libbp.a` - static library
* `bindings/lua/build/bplib.so` - lua extension module

And performs the following installations:
* `/usr/local/lib`: bplib libraries
* `/usr/local/inc`: bplib includes
* `/usr/local/lib/lua/5.3`: lua extensions and helper scripts

Additional make commands are as follows:
* `make clean` will remove all generated files and directories
* `make testmem` will call valgrind for detecting memory leaks
* `make testcpu` will call valgrind/callgrind for detecting cpu bottlenecks
* `make testheap` will call valgrind/massif for detecting sources of memory bloat
* `make testcov` will generate a line coverage report (if built and run with gcov, which is enabled by default)

On CentOS you may need to create a file with the conf extension in /etc/ld.so.conf.d that contains the line '/usr/local/lib'.
* `sudo echo "/usr/local/lib" > /etc/ld.so.conf.d/local.conf`
* `sudo ldconfig`

#### Unit Tests

To manually run the unit test suite:
* `lua5.3 binding/lua/test/test_runner.lua`

To run a specific unit test using one of the test targets provided in the makefile:
* make test{mem|cpu|heap|cov} testcase=binding/lua/test/ut_{test}.lua

#### Releases

The default `posix.mk` configuration makefile is for development and builds additional C unit tests, code coverage profiling, stack protector, and uses minimum compiler optimizations. When releasing the code, the library should be built with `release.mk` as follows:
* `make CONFIG=release.mk`

----------------------------------------------------------------------
## 3. Application Design
----------------------------------------------------------------------

![Figure 1](doc/bp_api_architecture.png "BP Library API (Architecture)")

Bplib is written in "vanilla C" and is intended to be linked in as either a shared or static library into an application with an API for reading/writing application data and reading/writing bundles.

Conceptually, the library is meant to exist inside a board support package for an operating system and be presented to the application as a service.  In such a design only the interface for reading/writing data would be provided to the application, and the interface for reading/writing bundles would be kept inside the board support package.  This use case would look a lot like a typical socket application where a bundle channel (socket) is opened, data is read/written, and then at some later time the channel is closed.  Underneath, the operating system would take care of sending and receiving bundles.

In order to support bplib being used directly by the application, both the data and the bundle interfaces are provided in the API. In these cases, the application is also responsible for sending and receiving the bundles.

An example application design that manages both the data and bundle interfaces could look as follows:
1. A __bundle reader__ thread that receives bundles from a convergence layer and calls bplib to _process_ them
2. A __data writer__ thread that _accepts_ application data from bplib
3. A __bundle writer__ thread that _loads_ bundles from bplib and sends bundles over a convergence layer
4. A __data reader__ thread that _stores_ application data to bplib

The stream of bundles received by the application is handled by the bundle reader and data writer threads. The __bundle reader__ uses the `bplib_process` function to pass bundles read from the convergence layer into the library.  If those bundles contain payload data bound for the application, that data is pulled out of the bundles and queued in storage until the __data writer__ thread calls the `bplib_accept` function to dequeue the data out of storage and write it to the application.

Conversely, the stream of bundles sent by the application is handled by the data reader and bundler writer threads. The __data reader__ thread calls `bplib_store` to pass data from the application into the library to be bundled.  Those bundles are queued in storage until the __bundle writer__ threads calls the `bplib_load` function to dequeue them out of storage and write them to the convergence layer.

----------------------------------------------------------------------
## 4. Application Programming Interface
----------------------------------------------------------------------

#### 4.1 Functions

| Function        | Purpose |
| --------------- | ------- |
| [bplib_init](#initialize)                | Initialize the BP library - called once at program start |
| [bplib_open](#open-channel)              | Open a channel - provides handle to channel for future channel operations |
| [bplib_close](#close-channel)            | Close a channel |
| [bplib_flush](#flush-channel)            | Flush active bundles on a channel |
| [bplib_config](#config-channel)          | Change and retrieve channel settings |
| [bplib_latchstats](#latch-statistics)    | Read out bundle statistics for a channel |
| [bplib_store](#store-payload)            | Create a bundle from application data and queue in storage for transmission |
| [bplib_load](#load-bundle)               | Retrieve the next available bundle from storage to transmit |
| [bplib_process](#process-bundle)         | Process a bundle for data extraction, custody acceptance, and/or forwarding |
| [bplib_accept](#accept-payload)          | Retrieve the next available data payload from a received bundle |
| [bplib_ackbundle](#acknowledge-bundle)   | Release bundle memory pointer for reuse (needed after bplib_load) |
| [bplib_ackpayload](#acknowledge-payload) | Release payload memory pointer for reuse (needed after bplib_accept) |
| [bplib_routeinfo](#route-information)    | Parse bundle and return routing information |
| [bplib_display](#display-bundle)         | Parse bundle and log a break-down of the bundle elements |
| [bplib_eid2ipn](#eid-to-ipn)             | Utility function to translate an EID string into node and service numbers |
| [bplib_ipn2eid](#ipn-to-eid)             | Utility function to translate node and service numbers into an EID string |
| [bplib_attrinit](#attr-init)             | Utility to initialize a channel attribute structure with default values.  Useful if the calling application only wants to change a few attributes without setting them all. |

__Note__: _functions that operate on a channel are thread-safe with other functions that operate on channels, but they are not thread-safe with the open and close functions.  A channel can only be closed when no other operations are being performed on it._


----------------------------------------------------------------------
##### Initialize

`void bplib_init (void)`

Initializes the BP library.  This must be called before any other call to the library is made.  It calls the operating system layer initialization routine.

----------------------------------------------------------------------
##### Open Channel

`bp_desc_t* bplib_open (bp_route_t route, bp_store_t store, bp_attr_t* attributes)`

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

* __cipher_suite__: Bundle generation parameter - provides the CRC type used inside the BIB extension block.  If the __integrity_check__ attribute is not set, then this setting is ignored.  If the __integrity_check__ attribute is set and this attribute is set to BP_BIB_NONE, then a BIB is included but the cipher result length is zero (this provide unambigous indication that no integrity check is included). Currently supported cipher suites are: BP_BIB_CRC16_X25, and BP_BIB_CRC32_CASTAGNOLI.

* __timeout__: The number of seconds the library waits before re-loading an unacknowledged bundle.

* __max_length__: The maximum size in bytes that a bundle can be, both on receipt and on transmission.

* __cid_reuse__: The library's behavior when a bundle times-out - if set, bundles that are retransmitted use the original Custody ID of the bundle when it was originally sent; if not set, then a new Custody ID is used when the bundle is retransmitted.  Re-using the Custody ID bounds the size of the Aggregrate Custody Signal coming back (worse-case gaps).  Using a new Custody ID makes the average size of the Aggregate Custody Signal smaller.

* __dacs_rate__: The maximum number of seconds to wait before an Aggregate Custody Signal which has accumulated acknowledgments is sent.  Every time a call to `bplib_load` is made, the code checks to see if there is an Aggregate Custody Signal which exists in memory but has not been sent for at least __dacs_rate__ seconds.

* __protocol_version__: Which version of the bundle protocol to use; currently the library only supports version 6.

* __retransmit_order__: The order in which bundles that have timed-out are retransmitted. There are currently two retransmission orders supported: BP_RETX_OLDEST_BUNDLE, and BP_RETX_SMALLEST_CID.

* __active_table_size__:  The number of unacknowledged bundles to keep track of. The larger this number, the more bundles can be sent before a "wrap" occurs (see BP_OPT_WRAP_RESPONSE).  But every unacknowledged bundle consumes 8 bytes of CPU memory making this attribute the primary driver for a channel's memory usage.

* __max_fills_per_dacs__: The maximum number of fills in the Aggregate Custody Signal.  An Aggregate Custody Signal is sent when the maximum fills are reached or the __dacs_rate__ period has expired (see BP_OPT_DACS_RATE).

* __max_gaps_per_dacs__: The maximum number of Custody ID gaps a channel can keep track up when receiving bundles requesting custody transfer.  If this gap limit is reached, the Aggregate Custody Signal is sent and a new one immediately begins to accumulate acknowledgments.

* __storage_service_parm__: A pass through to the storage service `create` function.

`returns` - pointer to a channel descriptor.  On error, NULL is returned.

----------------------------------------------------------------------
##### Close Channel

`void bplib_close (bp_desc_t* desc)`

Closes the specified bundle channel and releases all run-time resources associated with it; this does not include the bundles stored in the storage service; nor does it include bundles that have been transmitted but not yet acknowledged (active bundles).  The close call is not mutex'ed against other channel operations - it is the caller's responsibility that the close call is made non-concurrently with any other library function call on that channel.

`desc` - a descriptor for which channel to close

----------------------------------------------------------------------
##### Flush Channel

`int bplib_flush (bp_desc_t* desc)`

Flushes all active bundles on a channel; this treats each bundle that has been transmitted but not yet acknowledged as if it was immediately acknowledged.  This function is separate from the bplib_close function because it is possible that a storage service supports resuming where it left off after a channel is closed.  In such a case, closing the channel would occur without flushing the data since the next time the channel was opened, the data that had not yet been relinquished would resume being sent.

`channel` - a descriptor for which channel to flush

----------------------------------------------------------------------
##### Config Channel

`int bplib_config (bp_desc_t* desc, int mode, int opt, void* val, int len)`

Configures or retrieves an attribute on a channel.

`desc` - a descriptor for which channel to configure or retrieve attribute

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
BP_WRAP_BLOCK, BP_WRAP_DROP |
| BP_OPT_CID_REUSE       | int      | 0 | Sets whether retransmitted bundles reuse their original custody ID, 0: false, 1: true |
| BP_OPT_DACS_RATE       | int      | 5 | Sets minimum rate of ACS generation |

__NOTE__: _transmitted_ bundles include both bundles generated on the channel from local data that is stored, as well as bundles that are received and forwarded by the channel.

`val` - the value set or returned

`len` - the length in bytes of the memory pointed to by _val_

`returns` - [return code](#4-2-return-codes).

----------------------------------------------------------------------
##### Latch Statistics

`int bplib_latchstats (bp_desc_t* desc, bp_stats_t* stats)`

Retrieve channel statistics populated in the structure pointed to by _stats_.

`desc` - a descriptor for channel to retrieve statistics on

`stats` - pointer to the statistics structure to be populated

* __lost__: number of deleted bundles due to: storage failure, and memory copy failure

* __expired__: number of deleted bundles due to their lifetime expiring

* __unrecognized__: number of bundles that were attempted to be processed but either could not be parsed or were of an unsupported type

* __transmitted_bundles__: number of bundles returned by the `bplib_load` function for the first time (does not include retransmissions)

* __transmitted_dacs__: number of dacs returned by the `bplib_load` function

* __retransmitted_bundles__: number of bundles returned by the `bplib_load` function because the bundle timed-out and is being resent

* __delivered_payloads__: number of bundle payloads delivered to the application via the `bplib_accept` function

* __received_bundles__: number of bundles destined for the local node that were successfully processed by the `bplib_process` function; the payload was successfully stored by the storage service and is awaiting acceptance

* __forwarded_bundles__: number of bundles destined for the another node that were successfully processed by the `bplib_process` function; this does not indicated that the forwarded bundle was transmitted, only that it was successfully stored by the storage service and is awaiting transmission.

* __received_dacs__: number of DACS destined for the local node that were successfully processed by the `bplib_process` function; this only counts the DACS bundles received by the local node, not the bundles acknowledged by the DACS - that is represented in the acknowledged_bundles statistic.

* __stored_bundles__: number of data bundles currently in storage

* __stored_payloads__: number of payloads currently in storage

* __stored_dacs__: number of aggregate custody signal bundles currently in storage

* __acknowledged_bundles__: number of locally stored bundles positively acknowleged and deleted due to a custody signal acknowledgment

* __active_bundles__: number of bundles that have been loaded for which no acknowledgment has been received

----------------------------------------------------------------------
##### Store Payload

`int bplib_store (bp_desc_t* desc, void* payload, int size, int timeout, uint32_t* flags)`

Initiates sending the data pointed to by _payload_ as a bundle. The data will be encapsulated in a bundle (or many bundles if the channel allows fragmentation and the payload exceeds the maximum bundle length) and queued in storage for later retrieval and transmission.

`desc` - a descriptor for channel to create bundle on

`payload` - pointer to data to be bundled

`size` - size of payload in bytes

`timeout` - 0: check, -1: pend, 1 and above: timeout in milliseconds

`flags` - flags that provide additional information on the result of the store operation (see [flags](#6-3-flag-definitions)).  The flags variable is not initialized inside the function, so any value it has prior to the function call will be retained.

`returns` - size of bundle created in bytes, or [return code](#4-2-return-codes) on error.

----------------------------------------------------------------------
##### Load Bundle

`int bplib_load (bp_desc_t* desc, void** bundle,  int* size, int timeout, uint32_t* flags)`

Reads the next bundle from storage to be sent by the application over the convergence layer.  From the perspective of the library, once a bundle is loaded to the application, it is as good as sent.  Any failure of the application to send the bundle is treated no differently that a failure downstream in the bundle reaching its destination.  On the other hand, the memory containing the bundle returned by the library is kept valid until the `bplib_ackbundle` function is called, which must be called once for every returned bundle.  So while subsequent calls to `bplib_load` will continue to provide the next bundle the library determines should be sent, the application is free to hold onto the bundle buffer and keep trying to send it until it acknowledges the bundle to the library.

`desc` - a descriptor for channel to retrieve bundle from

`bundle` - pointer to a bundle buffer pointer; on success, the library will populate this pointer with the address of a buffer containing the bundle that is loaded.

`size` - pointer to a variable holding the size in bytes of the bundle buffer being returned, populated on success.

`timeout` - 0: check, -1: pend, 1 and above: timeout in milliseconds

`flags` - flags that provide additional information on the result of the load operation (see [flags](#6-3-flag-definitions)). The flags variable is not initialized inside the function, so any value it has prior to the function call will be retained.

`returns` - the bundle reference, the size of the bundle, and [return code](#4-2-return-codes)

----------------------------------------------------------------------
##### Process Bundle

`int bplib_process (bp_desc_t* desc, void* bundle,  int size, int timeout, uint32_t* flags)`

Processes the provided bundle.

There are three types of bundles processed by this function:
(1) If the bundle is an aggregate custody signal, then any acknowledged bundles will be freed from storage.
(2) If the bundle is destined for the local node, then the payload data will be extracted and queued for retrieval by the application; and if custody is requested, then the current aggregate custody signal will be updated and queued for transmission if necessary.
(3) If the bundle is not destined for the local node, then the bundle will be queued for transmission as a forwarded bundle; and if custody is requested, then the current aggregate custody signal will be updated and queued for transmission if necessary.

`desc` - a descriptor for channel to process bundle on

`bundle` - pointer to a bundle

`size` - size of the bundle in bytes

`timeout` - 0: check, -1: pend, 1 and above: timeout in milliseconds

`flags` - flags that provide additional information on the result of the process operation (see [flags](#6-3-flag-definitions)). The flags variable is not initialized inside the function, so any value it has prior to the function call will be retained.

`returns` - [return code](#4-2-return-codes).

----------------------------------------------------------------------
##### Accept Payload

`int bplib_accept (bp_desc_t* desc, void** payload, int* size, int timeout, uint32_t* flags)`

Returns the next available bundle payload (from bundles that have been received and processed via the `bplib_process` function) to the application. The memory containing the payload returned by the library is kept valid until the `bplib_ackpayload` function is called, which must be called once for every returned payload.  So while subsequent calls to `bplib_accept` will continue to provide the next payload the library determines should be accepted, the payload will not be deleted from the library's storage service until it is acknowledged by the application.

`desc` - a descriptor for channel to accept payload from

`payload` - pointer to a payload buffer pointer; on success, the library will populate this pointer with the address of a buffer containing the payload that is accepted.

`size` - pointer to a variable holding the size in bytes of the payload buffer being returned, populated on success.

`timeout` - 0: check, -1: pend, 1 and above: timeout in milliseconds

`flags` - flags that provide additional information on the result of the accept operation (see [flags](#6-3-flag-definitions)). The flags variable is not initialized inside the function, so any value it has prior to the function call will be retained.

`returns` - the payload reference, the size of the payload, and [return code](#4-2-return-codes)

----------------------------------------------------------------------
##### Acknowledge Bundle

`int bplib_ackbundle (bp_desc_t* desc, void* bundle)`

Informs the library that the memory and storage used for the payload can be freed.  The memory will be immediately freed, the storage will be freed immediately only if the bundle is not requesting custody transfer (otherwise, if the bundle is requesting custody transfer, then the ACS acknowledgment frees the storage).  This must be called at some point after every bundle that is loaded.

`desc` - a descriptor for channel to acknowlwedge bundle

`bundle` - pointer to the bundle buffer to be acknowledged

`returns` - [return code](#4-2-return-codes)

----------------------------------------------------------------------
##### Acknowledge Payload

`int bplib_ackpayload (bp_desc_t* desc, void* payload)`

Informs the library that the memory and storage used for the payload can be freed.  This must be called at some point after every payload that is accepted.

`desc` - a descriptor for channel to acknowlwedge payload

`payload` - pointer to the payload buffer to be acknowledged

`returns` - [return code](#4-2-return-codes)

----------------------------------------------------------------------
##### Route Information

`int bplib_routeinfo (void* bundle, int size, bp_route_t* route)`

Parses the provided bundle and supplies its endpoint ID node and service numbers.  Used to route a received bundle to the appropriate channel by looking up its destination endpoint ID prior to making other library calls that require a channel identifier.

`bundle` - pointer to a buffer of memory containing a properly formatted bundle

`size` - size of the bundle

`route` - pointer to a route structure that is populated by the function (see [Open Channel](#open-channel) for more details on the structure contents).

`returns` - [return code](#4-2-return-codes)

----------------------------------------------------------------------
##### Display Bundle

`int bplib_display (void* bundle, int size, uint32_t* flags)`

Parses the provided bundle (transversing the primary block, extension blocks, and payload block), and logs debug information about the bundle.

`bundle` - pointer to a buffer of memory containing a properly formatted bundle

`size` - size of the bundle

`flags` - flags that provide additional information on the result of the accept operation (see [flags](#6-3-flag-definitions)). The flags variable is not initialized inside the function, so any value it has prior to the function call will be retained.

`returns` - [return code](#4-2-return-codes)

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
#### 4.2 Return Codes

| Code                    | Value | Description |
| ----------------------- | ----- | ----------- |
| BP_SUCCESS              | 0     | Operation successfully performed |
| BP_ERROR                | -1    | Generic error occurred; further information provided in flags to determine root cause |
| BP_TIMEOUT              | -2    | A timeout occurred when a blocking operation was performed |

----------------------------------------------------------------------
#### 4.3 Flag Definitions

| Flag                           | Value | Description |
| ------------------------------ | ----- | ----------- |
| BP_FLAG_DIAGNOSTIC             | 0x00000000 | No event issued - diagnostic message only |
| BP_FLAG_NONCOMPLIANT           | 0x00000001 | Valid bundle but the library was not able to comply with the standard |
| BP_FLAG_INCOMPLETE             | 0x00000002 | At least one block in bundle was not recognized |
| BP_FLAG_UNRELIABLE_TIME        | 0x00000004 | The time returned by the O.S. preceded the January 2000 epoch, or went backwards |
| BP_FLAG_DROPPED                | 0x00000008 | A bundle was dropped because a required extension block could not be processed |
| BP_FLAG_FAILED_INTEGRITY_CHECK | 0x00000010 | A bundle with a BIB failed the integrity check on the payload |
| BP_FLAG_BUNDLE_TOO_LARGE       | 0x00000020 | The size of a bundle exceeds the capacity allowed by library |
| BP_FLAG_ROUTE_NEEDED           | 0x00000040 | A bundle needs to be routed before transmission |
| BP_FLAG_STORE_FAILURE          | 0x00000080 | Storage service failed to deliver data |
| BP_FLAG_UNKNOWN_CID            | 0x00000100 | An ACS bundle acknowledged a CID for which no bundle was found |
| BP_FLAG_SDNV_OVERFLOW          | 0x00000200 | The local variable used to read/write and the value was of insufficient width |
| BP_FLAG_SDNV_INCOMPLETE        | 0x00000400 | There was insufficient room in block to read/write value |
| BP_FLAG_ACTIVE_TABLE_WRAP      | 0x00000800 | The active table wrapped; see BP_OPT_WRAP_RESPONSE |
| BP_FLAG_DUPLICATES             | 0x00001000 | The custody ID was already acknowledged |
| BP_FLAG_CUSTODY_FULL           | 0x00002000 | An aggregate custody signal was generated due the number of custody ID gaps exceeded the maximum allowed |
| BP_FLAG_UNKNOWNREC             | 0x00004000 | A bundle contained unknown adminstrative record |
| BP_FLAG_INVALID_CIPHER_SUITEID | 0x00008000 | An invalid cipher suite ID was found in a BIB |
| BP_FLAG_INVALID_BIB_RESULT_TYPE| 0x00010000 | An invalid result type was found in a BIB |
| BP_FLAG_INVALID_BIB_TARGET_TYPE| 0x00020000 | An invalid target type was found in a BIB |
| BP_FLAG_FAILED_TO_PARSE        | 0x00040000 | Unable to parse a bundle due to internal inconsistencies in bundle |

----------------------------------------------------------------------
## 5. Storage Service
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

`returns` - [return code](#4-2-return-codes)

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

`returns` - [return code](#4-2-return-codes)

----------------------------------------------------------------------
##### Dequeue Storage Service

`int dequeue (int handle, void** data, int* size, bp_sid_t* sid, int timeout)`

Retrieves the oldest data block stored in the storage service that has not yet been dequeued, and returns a _Storage ID_ that can be used to retrieve the data block in the future.

`handle` - handle to the storage service

`data` - the pointer that will be updated to point to the retrieved block of memory.  This function returns the data block via a pointer and performs no copy.  The data is still owned by the storage service and is only valid until the next dequeue or relinquish call.

`size` - size of data block being retrieved.

`sid` - pointer to a _Storage ID_ variable populated by the function.  The sid variable is used in future storage service functions to identify the retrieved data block.

`timeout` - 0: check, -1: pend, 1 and above: timeout in milliseconds

`returns` - [return code](#4-2-return-codes)

----------------------------------------------------------------------
##### Retrieve Storage Service

`int retrieve (int handle, void** data, int* size, bp_sid_t sid, int timeout)`

Retrieves the data block stored in the storage service identified by the _Storage ID_ sid parameter.

`handle` - handle to the storage service

`data` - the pointer that will be updated to point to the retrieved block of memory.  This function returns the data block via a pointer and performs no copy.  The data is still owned by the storage service and is only valid until the next dequeue or relinquish call.

`size` - size of data block being retrieved.

`sid` - the _Storage ID_ that identifies which data block to retrieve from the storage service

`timeout` - 0: check, -1: pend, 1 and above: timeout in milliseconds

`returns` - [return code](#4-2-return-codes)

----------------------------------------------------------------------
##### Release Storage Service

`int release (int handle, bp_sid_t sid)`

Releases any in-memory resources associated with the dequeueing or retrieval of a bundle.

`handle` - handle to the storage service

`sid` - the _Storage ID_ that identifies the data block for which memory resources are released.

`returns` - [return code](#4-2-return-codes)

----------------------------------------------------------------------
##### Relinquish Storage Service

`int relinquish (int handle, bp_sid_t sid)`

Deletes the stored data block identified by the _Storage ID_ sid parameter.

`handle` - handle to the storage service

`sid` - the _Storage ID_ that identifies which data block to delete from storage

`returns` - [return code](#4-2-return-codes)

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
* The memory returned by the dequeue and retrieve function is valid until the release function call.  Every dequeue and retrieve issued by the library will be followed by a release.
* The _Storage ID (SID)_ returned by the storage service cannot be zero since that is marked as a _VACANT_ SID


