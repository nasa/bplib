# bplib

[Overview](#overview)  
[Build with Make](#build-with-make)  
[Protocol Compliance and Tailoring](#protocol-compliance-and-tailoring)  
[Bundle Formats](#bundle-formats)  
[Application Programming Interface](#application-programming-interface)  
[Storage Service](#storage-service)  

Overview
----------------------------------------------------------------------

The Bundle Protocol library (bplib) implements a subset of the RFC5050 Bundle Protocol necessary for embedded space flight applications. The library uses the concept of a bundle channel to manage the process of encapsulating application data in bundles, and extracting application data out of bundles.  A channel specifies how the bundles are created (e.g. primary header block fields), and how bundles are processed. Bplib contains no threads and relies entirely on the calling application for its execution context and implements a thread-safe synchronous blocking I/O model where no call-backs are necessary and requested operations will either block according to the provided timeout, or return an error code immediately if the operation cannot be performed.

![Figure 1](doc/bp_api_basic.png "BP Library API (Basic)")

The fundamental problem bplib solves is the receipt and transmission of data when there is the potential for substantial differences in input and output rates.  Bplib is designed to handle the receipt of application data to be transmitted as bundles when that transmission occurs minutes, hours, or days later.  Likewise, bplib can receive bundles in long sustained bursts which may take the application minutes, or hours to process.  Therefore, in both cases, bplib assumes the availability of a persistent queued storage system for managing the rate buffering that must occur between data and bundle processing.

A possible application design for using bplib is to have four execution threads:
1. A __bundle reader__ thread that reads bundles from a convergence layer
2. A __data writer__ thread that writes the payload data read from bundles to the application layer
3. A __bundle writer__ thread that writes bundles to a convergence layer
4. A __data reader__ thread that reads data from the application layer that is then placed in bundles

The __bundle reader__ would use the `bplib_process` function to pass bundles read from the convergence layer into the library.  If those bundles contain payload data bound for the application, that data is pulled out of the bundles and queued in storage until the __data writer__ thread calls the `bplib_accept` function to dequeue the data out of storage and write it to the application.  Meanwhile, if the application is also producing data of its own to be bundled, the __data reader__ thread would call `bplib_store` to pass data from the application into the library to be bundled.  Those bundles are queued in storage until the __bundle writer__ threads calls the `bplib_load` function to dequeue them out of storage and write them to the convergence layer.


Build with Make
----------------------------------------------------------------------

1. Go to repository root directory
2. `make`
3. `sudo make install`

This will build the following binaries:
* `build/bpc` - console application
* `build/libbp.so.<version>` - shared library
* `build/libbp.a` - static library

And perform the following installations:
* `/usr/local/bin`: console application
* `/usr/local/lib`: bplib library
* `/usr/local/inc`: bplib includes

Additional make commands are as follows:
* `make clean` will remove all generated files and directories
* `make prep` will create the subdirectories used when building the code
* `make lib` will compile just the library with dependencies only mapping to the source files.
* `make console` will build the `bpc` binary

On CentOS you may need to create a file with the conf extension in /etc/ld.so.conf.d that contains the line '/usr/local/lib'.
* `sudo echo "/usr/local/lib" > /etc/ld.so.conf.d/local.conf`
* `sudo ldconfig` 


Protocol Compliance and Tailoring
----------------------------------------------------------------------

__Endpoint IDs__
* Compressed Bundle Header Encoding (CBHE) is used on all generated data bundles.  
* The only addressing schema supported by the BP library is the "ipn" schema defined in RFC6260.  
* The format of endpoint IDs as defined by this schema are "ipn:{node}.{service}" where {node} and {service} are unsigned integers encoded as SDNVs in the bundle header block. 
Constant static data structures declared in the code set the size of all {node} numbers to 28 bits (which occupies 32 bits of bundle memory encoded as an SDNV), and all {service} numbers to 14 bits (which occupies 16 bits of bundle memory encoded as an SDNV).
* The convention used by the BP library is that {node} number identifies the endpoint application agent and the {service} number identifies a flow of data that is able to be processed by the application agent.
* All endpoints are treated as singletons and marked as such in the PCF, even for forwarded bundles.

__Fixed Size SDNVs__
* The BP library uses fixed sized SDNVs for all SDNV fields of generated bundles. 
* Bundles that are received by the library can have any size SDNV as long as the encoded value fits within 32-bits (future enhancement to make compile time switch for 64-bits).

__Creation Time Sequence__
* The BP library does not reset the creation time sequence number, but maintains a strictly incrementing value (stepped by one for each successive bundle created) per channel.
* This allows receiving nodes to detect gaps in the data and allows reconstruction of the original sending order of the data.

__Lifetimes__
* The BP library calculates an absolute time for a bundles expiration at the time the bundle is created.
* The expiration time is checked prior to transmission and on receipt (in `bplib_load` and `bplib_process` functions).

__Administrative Records__
* The only supported administrative record type is the aggregate custody signal, all other record types are ignored.
* The report to EID for bundles generated by this library is set to null

__DTN Aggregate Custody Signaling__
* The BP library only supports custody transfers when DTN Aggregate Custody Signals are used.  Regular RFC5050 custody signals are not supported. A custody signal bundle received by the library is ignored.
* A data bundle received by the library that is requesting custody transfer without a CTEB is dropped.  
* ACS acknowledgments are delivered to the “custodian” node and service provided in the bundle
* The rate that ACS bundles are sent is controlled via a receiver node configuration setting.  The BP library has a compile-time defined maximum number of fills per ACS, and a runtime setting for the longest time a partially full ACS bundle can remain unsent.
* Only positive acknowledgement are used meaning that the lifetime of a bundle and the timeout setting of the sending bundle agent completely define the bundle’s retransmission behavior.

__Bundle Integrity__
* The BP library inserts a BIB in all data bundles it generates and all data bundles it accepts custody of for forwarding.  
* Bundles received by the library are not required to have BIBs, but if they do, then the bundle is verified against the BIB prior to accepting custody (if requested) and processing the payload (if destined for the local node).

__Timeouts__
* Timeout processing is not be fully implemented.  The library keeps track of active bundles (i.e. bundles that have been sent but not acknowledged) in a circular buffer of bundle references that maintain strictly incrementing custody IDs. When the application requests a bundle to be loaded, only the oldest active bundle is checked for a timeout.  
* If the circular buffer wraps and the oldest active bundle has not timed out, then onboard transmission of bundles will either: 
  1. Stop until the oldest bundle times out, 
  2. Force an immediate timeout of the oldest bundle regardless of its actual timeout period
  3. Delete the oldest bundle and continue transmitting new bundles.  
* The implication of this approach is that if large timeouts are used, it is possible for the entire process to be held up by a bundle that has not yet been acknowledged even though many other bundles sent after it have.

__Class of Service__
* The class of service of generated data bundles and received bundles is completely ignored.

__Storage Service__
* The BP library requires that a storage service is provided outside the library. 

__Fragmentation__
* In order to preserve the layout of generated data bundles in memory, if a channel allows fragmentation, all bundles created on that channel will be fragmented, even if the entire payload fits within a single bundle.
* Custody transfer is requested per fragment.  In other words, each fragment has its own CTEB.
* A fragment of a bundle that requests custody transfer, and is sent to an intermediary hop, is acknowledged and forwarded by that hop without requiring the entire original bundle to be reconstituted.  Even though it is not required, there is also no assumed constraint on the fragment being further fragmented, or combined with other fragments if they are available.

__Routing__
* If a BP library channel is used to forward bundles (i.e. both receive data bundles and send data bundles), then it cannot be used to generate bundles.  This nuance is due to pre-building the bundle primary block, CTEB, BIB, and payload block header fields.
* When a call to the BP library returns a bundle, it will set a flag if the bundle needs to be routed to a different destination than the channel’s default destination.
* When routing bundles, the BP library will associate any bundle with a destination {node} that matches its local {node} number as being destined for itself and will then require that the {service} number matches the channel's {service} number that it is being processed on.  Any bundle with a {node} that is different than its local {node} number is treated as a bundle that needs to be routed.
* A service number of 0 is considered a global service and is allowed to be passed to any channel associated with the node

__Storage Service__ - The BP library requires that a storage service is provided outside the library.  When a channel is created, a set of function pointers are supplied to provide access to the storage service at runtime.  The library comes pre-built with a POSIX RAM storage service and file system storage service available to applications.


Bundle Formats
----------------------------------------------------------------------

### Data Bundle 
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

### DTN Aggregate Custody Signal (DACS)
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

### Primary Bundle Block: Compressed Bundle Header Encoding (CBHE)
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
             |   Creation Timestamp Sequence ...
 |-----------|-----------------------------------| 36
     ...     |            Lifetime ...
 |-----------|-----------|-----------------------| 40
     ...     |  Dict Len |  Fragment Offset ...
 |-----------|-----------|-----------------------| 44
     ...                 | Payload Length ... 
 |-----------------------|-----------------------| 48
     ...                 |
 |-----------------------| 52
```

### Custody Transfer Enhancement Block (CTEB)
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

### Bundle Integrity Block (BIB)
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

### Aggregate Custody Signal Block
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

Application Programming Interface
----------------------------------------------------------------------

| Function        | Purpose |
| --------------- | ------- | 
| [bplib_init](#initialize)             | Initialize the BP library - called once at program start |
| [bplib_open](#open-channel)           | Open a channel - provides handle to channel for future channel operations |
| [bplib_close](#close-channel)         | Close a channel |
| [bplib_getopt](#get-and-set-options)  | Get the setting of a channel option |
| [bplib_setopt](#get-and-set-options)  | Set the setting of a channel option |
| [bplib_latchstats](#latch-statistics) | Read out bundle statistics for a channel |
| [bplib_store](#store-payload)         | Create a bundle from application data and queue in storage for transmission |
| [bplib_load](#load-bundle)            | Retrieve the next available bundle from storage to transmit |
| [bplib_process](#process-bundle)      | Process a bundle for data extraction, custody acceptance, and/or forwarding |
| [bplib_accept](#accept-payload)       | Retrieve the next available data payload from a received bundle |
| [bplib_routeinfo](#route-information) | Parse bundle and return routing information |
| [bplib_eid2ipn](#eid-to-ipn)          | Utility function to translate an EID string into node and service numbers |
| [bplib_ipn2eid](#ipn-to-eid)          | Utility function to translate node and service numbers into an EID string |

Note: functions that operate on a channel are thread-safe with other functions that operate on channels, but they are not thread-safe with the open and close functions.  A channel can only be closed when no other operations are being performed on it.

### Initialize

`void bplib_init (void)`

Initializes the BP library.  This must be called before any other call to the library is made.  It also calls the operating system layer initialization routine.

### Open Channel

`int bplib_open (bp_store_t store, bp_ipn_t local_node, bp_ipn_t local_service, bp_ipn_t destination_node, bp_ipn_t destination_service, bp_attr_t* attributes)`

Opens a bundle channel that uses the provided storage service and endpoint IDs. 

This function returns a channel handle that is used for all future operations on the channel.  The open and close calls are mutex'ed against other open and close calls, but once a channel is created, operations on that channel are only mutex'ed against other operations on the same channel.  A channel persists until it is closed.

_store_ - a set of callbacks that provide access to the desired storage service

_local_node_ - node component of endpoint ID used by the library for the local agent.  This is used for the source, report to, and custody endpoint IDs.

_local_service_ - service component of endpoint ID used by the library for the local agent.  This is used for the source, report to, and custody endpoint IDs.

_destination_node_ - node component of endpoint ID used by the library for the destination of the bundles transmitted on the channel. 

_destination_service_ - service component of endpoint ID used by the library for the destination of the bundles transmitetd on the channel.

_attributes_ - structure of attributes for the channel that trade memory usage and performance

* active_table_size:  The number of unacknowledged bundles to keep track of. The larger this number, the more bundles can be sent before a "wrap" occurs (see BP_OPT_WRAPRSP).  But every unacknowledged bundle consumes 8 bytes of CPU memory making this attribute the primary driver for a channel's memory usage.
* max_concurrent_dacs: The number of dacs to maintain at one time.  Every time a bundle is received from a unique sender requesting custody transfer, the library must begin maintaining a list of custody IDs to populate an aggregate custody signal response bundle.  This attribute sets the maximum number of custody ID lists the library must be able to keep track of for a given channel.
* max_fills_per_dacs: The maximum number of fills in the aggregate custody signal.  A DACS is sent when the maximum fills are reached or the DACS rate period has expired (see BP_OPT_ACSRATE).


_returns_ - channel ID. 

### Close Channel

`void bplib_close (int channel)`

Closes the specified bundle channel and releases all run-time resources associated with it; this does not include the bundles stored in the storage service.  The close call is not mutex'ed against other channel operations - it is the caller's responsibility that the close call is made non-concurrent with any other library function call on that channel.

_channel_ - which channel to close

### Get and Set Options

`int bplib_getopt (int channel, int opt, void* val, int len)`
`int bplib_setopt (int channel, int opt, void* val, int len)`

Configures a channel (or retrieves the configuration of a channel).  Note that all options that have a "D" suffix only apply to data bundles that are generated by the BP library through the `bplib_store` function.

_channel_ - which channel to apply option operation on

_opt_ - the option as described in the table below

| Option                 | Units    | Default | Description |
| ---------------------- | -------- | ------- | ----------- |
| BP_OPT_DSTNODE_D       | bp_ipn_t | destination node (set on channel creation) | Destination node ID of generated data bundles (effectively 28 bits) |
| BP_OPT_DSTSERV_D       | bp_ipn_t | destination node (set on channel creation) | Destination service ID of generated data bundles (effectively 14 bits) |
| BP_OPT_RPTNODE_D       | bp_ipn_t | local node (set on channel creation) |Report To node ID of generated data bundles (effectively 28 bits) |
| BP_OPT_RPTSERV_D       | bp_ipn_t | local node (set on channel creation) |Report To service ID of generated data bundles (effectively 14 bits) |
| BP_OPT_CSTNODE_D       | bp_ipn_t | local node (set on channel creation) |Custody node ID of generated data bundles (effectively 28 bits) |
| BP_OPT_CSTSERV_D       | bp_ipn_t | local node (set on channel creation) |Custody service ID of generated data bundles (effectively 14 bits) |
| BP_OPT_SETSEQUENCE_D   | int      | 0 | Jams the creation sequence number |
| BP_OPT_LIFETIME_D      | int      | 0 | Amount of time in seconds added to creation time specifying duration of time bundle is considered valid, 0: infinite |
| BP_OPT_CSTRQST_D       | int      | true | Custody request - false: leave flag unset in generated data bundles, true: set custody request flag in generated data bundles |
| BP_OPT_ALLOWFRAG_D     | int      | true | Allow fragmentation - false: bundles are not allowed to be fragmented and the allow fragmentation flag is unset in generated data bundles, true: bundles are allowed to be fragmented and the allow fragmentation flag is set in generated data bundles |
| BP_OPT_PAYCRC_D        | int      | BP_BIB_CRC16 | Integrity check type used for payload.  Currently only BP_BIB_CRC16 and BP_BIB_NONE are supported. |
| BP_OPT_TIMEOUT         | int      | 10 | Amount of time in seconds to wait for positive acknowledgment of bundle before retransmitting, 0: infinite |
| BP_OPT_BUNDLELEN       | int      | 4096 | Maximum length of the bundle payload (primary block, extension blocks, and payload block header excluded); if length exceeded, then a fragmented bundle is created |
| BP_OPT_ORIGINATE       | int      | true | Channel originates the data.  When set to true, the channel can only generate bundles and receive bundles as a destination node; it cannot forward bundles.  When set to false, the channel can only forward bundles and cannot generate bundles from application data.  This functional partition is necessary to allow the optimization of prebuilding the bundle headers. |
| BP_OPT_PROCADMINONLY   | int      | false | Configure channel to process administrative records only - true: enable, false: disable |
| BP_OPT_WRAPRSP         | int      | BP_WRAP_RESEND | Configure channel's response when the active table wraps - BP_WRAP_RESEND, BP_WRAP_BLOCK, BP_WRAP_DROP |
| BP_OPT_CIDREUSE        | int      | false | When enabled, a restransmitted bundle reuses its custody IDs when it timesout.  When disabled, a restransmitted bundle uses the next available custody ID when it timesout. |
| BP_OPT_ACSRATE         | int      | 5 | Minimum rate of ACS generation; various circumstances can cause an ACS to be generated, but this rate sets the maximum about time that can elapse before an ACS is automatically generated |

_val_ - the value set or returned

_len_ - the length in bytes of the memory pointed to by _val_

_returns_ - return code (see below).

### Latch Statistics

`int bplib_latchstats (int channel, bp_stats_t* stats)`

### Store Payload

`int bplib_store (int channel, void* payload, int size, int timeout, uint16_t* storflags)`

Initiates sending the data pointed to by _payload_ as a bundle. The data will be encapsulated in a bundle, or many bundles if the channel allows fragmentation and the payload exceeds the maximum bundle length, and queued in storage for later retrieval and transmission.

_channel_ - channel to create bundle on

_payload_ - pointer to data to be bundled

_size_ - size of payload in bytes

_timeout_ - 0: check, -1: pend, 1 and above: timeout in milliseconds

_storflags_ - flags that provide additional information on the result of the store operation (see below)

_returns_ - size of bundle created in bytes, or return code on error (see below).

### Load Bundle

`int bplib_load (int channel, void* bundle,  int size, int timeout, uint16_t* loadflags)`

Reads the next bundle from storage to be sent by the application over the convergence layer.  From the perspective of the BP library, once a bundle is loaded to the application, it is as good as sent.  Any failure of the application to send the bundle is treated no differently that a failure downstream in the bundle reaching its destination. 

_channel_ - channel to retrieve bundle from

_bundle_ - pointer to the buffer for bundle being loaded to be copied into

_size_ - size of the bundle buffer

_timeout_ - 0: check, -1: pend, 1 and above: timeout in milliseconds

_loadflags_ - flags that provide additional information on the result of the load operation (see below)

_returns_ - size of bundle in bytes, or return code on error (see below)

### Process Bundle

`int bplib_process (int channel, void* bundle,  int size, int timeout, uint16_t* procflags)`

Processes the provided bundle.

There are three types of bundles processed by this function:
(1) If the bundle is an aggregate custody signal, then any acknowledged bundles will be freed from storage.
(2) If the bundle is destined for the local node, then the payload data will be extracted and queued for retrieval by the application; and if custody is requested, then the current aggregate custody signal will be updated and queued for transmission if necessary.
(3) If the bundle is not destined for the local node, then the bundle will be queued for transmission; and if custody is requested, then the current aggregate custody signal will be updated and queued for transmission if necessary.

_channel_ - channel to process bundle on

_bundle_ - pointer to a properly formated bundle

_size_ - size of the bundle in bytes

_timeout_ - 0: check, -1: pend, 1 and above: timeout in milliseconds

_procflags_ - flags that provide additional information on the result of the process operation (see below)

_returns_ - return code (see below)

### Accept Payload

`int bplib_accept (int channel, void* payload, int size, int timeout, uint16_t* acptflags)`

Returns the next available bundle payload (from bundles that have been received and processed via the `bplib_process` function) to the application by populating the structure pointed to by the _payload_ pointer.

_channel_ - channel to accept payload from

_payload_ - pointer to buffer that the payload will be copied into

_size_ - size of the payload buffer

_timeout_ - 0: check, -1: pend, 1 and above: timeout in milliseconds

_acptflags_ - flags that provide additional information on the result of the accept operation (see below)

_returns_ - size of bundle payload in bytes, or return code on error (see below).

### Route Information

`int bplib_routeinfo (void* bundle, int size, bp_ipn_t* destination_node, bp_ipn_t* destination_service)`

Parses the provided bundle and supplies its destination node and service numbers.  Used to route a received bundle to the appropriate channel by looking up its destination endpoint ID prior to making other library calls that require a channel identifier.

_bundle_ - pointer to a buffer of memory containing a properly formatted bundle

_size_ - size of the bundle

_destination_node_ - pointer to a variable that is populated by the function with the bundle's destination node number

_destination_service_ - pointer to a variable that is populated by the function with the bundle's destination service number

_returns_ - return code (see below)

### EID to IPN

`int bplib_eid2ipn (const char* eid, int len, bp_ipn_t* node, bp_ipn_t* service)`

### IPN to EID

`int bplib_ipn2eid (char* eid, int len, bp_ipn_t node, bp_ipn_t service)`

### Return Codes

| Code                    | Value | Description |
| ----------------------- | ----- | ----------- |
| BP_SUCCESS              | 1     | Operation successfully performed |
| BP_TIMEOUT              | 0     | A timeout occurred when a blocking operation was performed |
| BP_PARMERR              | -1    | Parameter passed into function was invalid |
| BP_UNSUPPORTED          | -2    | A valid bundle could not be processed by the library because the requested functionality is not yet implemented |
| BP_EXPIRED              | -3    | A bundle expired due to its lifetime and was deleted |
| BP_IGNORE               | -4    | A bundle or a block of a bundle was ignored because the channel was configured to do so based on the type of bundle or type of block |
| BP_DROPPED              | -5    | A bundle was dropped because it could not be processed |
| BP_CHANNELSFULL         | -6    | A channel could not be opened because the maximum number of open channels was reached |
| BP_INVALIDHANDLE        | -7    | The handle passed into a storage service function was invalid |
| BP_OVERFLOW             | -8    | An SDNV was not able to be written or read due to their being insufficient space in the variable or bundle buffer |
| BP_WRONGVERSION         | -9    | The primary block bundle version number did not match the CCSDS recommended version |
| BP_BUNDLEPARSEERR       | -10   | An error was encountered when trying to read or write a bundle, usually associated with either an SDNV overflow or a buffer that is too small |
| BP_UNKNOWNREC           | -11   | The administrative record type was unrecognized by the library |
| BP_BUNDLETOOLARGE       | -12   | The size of the bundle exceeded the maximum size bundle that is able to be processed by the library |
| BP_PAYLOADTOOLARGE      | -13   | The size of the payload exceeded the maximum size payload that is able to be processed by the library |
| BP_WRONGORIGINATION     | -14   | The channel was used to forward data when its origination was set to true, or it was used to generate bundles when its origination was set to false. |
| BP_WRONGCHANNEL         | -15   | The destination service number did not match the channel's source service number when trying to processing a bundle destined for the local node |
| BP_FAILEDINTEGRITYCHECK | -16   | A bundle processed by the library contained a Bundle Integrity Block, but the checksum contained in that block did not match the calculated checksum |
| BP_FAILEDSTORE          | -17   | The library encountered an error originating from the storage service |
| BP_FAILEDRESPONSE       | -18   | The library was unable to report back to another node, e.g. a DACS bundle could not be created or sent due to there being too many sources to track |
| BP_FAILEDOS             | -19   | The library encountered an error originating from the operation system abstraction layer |
| BP_FAILEDMEM            | -20   | The library encountered an error allocating memory for a channel |
| BP_INVALIDEID           | -21   | An EID string did not contain a valid IPN address |

### Flags

| Flag                     | Value | Description |
| ------------------------ | ----- | ----------- |
| BP_FLAG_NONCOMPLIANT     | 0x0001 | Valid bundle but the library was not able to comply with the standard |
| BP_FLAG_INCOMPLETE       | 0x0002 | At least one block in bundle was not recognized |
| BP_FLAG_TOOMANYSOURCES   | 0x0004 | There are too many sources to keep track of for ACS bundles |
| BP_FLAG_FILLOVERFLOW     | 0x0008 | A gap in the CIDs exceeds the max fill value allowed in an ACS bundle |
| BP_FLAG_TOOMANYFILLS     | 0x0010 | All the fills in the ACS are used |
| BP_FLAG_CIDWENTBACKWARDS | 0x0020 | The custody ID went backwards |
| BP_FLAG_ROUTENEEDED      | 0x0040 | The bundle returned needs to be routed before transmission |
| BP_FLAG_STOREFAILURE     | 0x0080 | Storage service failed to deliver data |
| BP_FLAG_MIXEDRESPONSE    | 0x0100 | Aggregate acknowledgement must have uniform delivery vs. forward |
| BP_FLAG_SDNVOVERFLOW     | 0x0200 | There was insufficient room in 32-bit variable to read/write value |
| BP_FLAG_SDNVINCOMPLETE   | 0x0400 | There was insufficient room in block to read/write value |
| BP_FLAG_ACTIVETABLEWRAP  | 0x0800 | The active table wrapped; see BP_OPT_WRAPRSP |

Storage Service
----------------------------------------------------------------------

* Storage service functions do not need to be reentrant.
* Enqueue, Dequeue, Retrieve, and Relinquish are expected to be thread safe against each other.
* Create and Destroy do not need to be thread safe against each other or any other function call - the application is responsible for call them when it can complete atomically with respect to any other storage service call
* The memory returned by the dequeue function is valid only until the next dequeue function call or the next relinquish function call
 