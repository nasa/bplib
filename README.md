# bplib

Bundle Protocol Library
----------------------------------------------------------------------

The Bundle Protocol library (bplib) implements a subset of the RFC5050 Bundle Protocol necessary for embedded space flight applications. The library uses the concept of a bundle channel to manage the process of encapsulating application data in bundles, and pulling application data out of bundles.  A channel specifies how the bundles are created (e.g. primary header block fields), and how bundles are processed. Bplib contains no threads and relies entirely on the calling application for its execution context and therefore implements a thread-safe synchronous blocking I/O model where no call-backs are necessary and requested operations will either block according to the provided timeout, or return an error code immediately if the operation cannot be performed.

![Figure 1](doc/bp_api_basic.png "BP Library API (Basic)")

The fundamental problem bplib solves is the receipt and transmission of data when there is the potential for substational differences in input and output rates.  Bplib is designed to handle the receipt of application data to be transmitted as bundles when that transmission occurs minutes, hours, or days later.  Likewise, bplib can receive bundles in long sustained bursts which may take the application minutes, or hours to process.  Therefore, in both cases, bplib assumes the availability of a persistent queued storage system for managing the rate buffering that must occur between data and bundle processing.

A possible application design for using bplib is to have four execution threads:
1. A __bundle reader__ thread that reads bundles from a convergence layer
2. A __data writer__ thread that writes the payload data read from bundles to the application layer
3. A __bundle writer__ thread that writes bundles to a convergence layer
4. A __data reader__ thread that reads data from the application layer that is then placed in bundles

The __bundle reader__ would use the `bplib_process` function to pass bundles read from the convergence layer into the library.  If those bundles contain payload data bound for the application, that data is pulled out of the bundles and queued in storage until the __data writer__ thread calls the `bplib_accept` function to dequeue the data out of storage and write it to the application.  Meanwhile, if the application is also producing data of its own to be bundled, the __data reader__ thread would call `bplib_store` to read data from the application and pass it into the library to be bundled.  Those bundles are queued in storage until the __bundle writer__ threads calls the `bplib_load` function to dequeue them out of storage and write them to the convergence layer.

## Protocol Compliance and Tailoring

__Endpoint IDs__ - Compressed Bundle Header Encoding (CBHE) is used on all generated data bundles.  As a result, the only addressing schema supported by the BP library is the "ipn" schema defined in RFC6260.  The format of endpoint IDs as defined by this schema are "ipn:{node}.{service}" where {node} and {service} are unsigned integers encoded as SDNVs in the bundle header block.  The BP library has constant static data structures declared in the code that sets the size of all {node} numbers to 28 bits (which occupies 32 bits of bundle memory encoded as an SDNV), and all {service} numbers to 14 bits (which occupies 16 bits of bundle memory encoded as an SDNV).  Futhermore, the convention used by the BP library is that {node} number identifies the endpoint application agent and the {service} number identifies a flow of data that is able to be processed by the application agent.  As a result, when routing bundles, the BP library will associate any bundle with a destination {node} that matches its local {node} number as being destined for itself and will then require that the {service} number matches the channel's {service} number that it is being processed on.  Any bundle with a {node} that is different than its local {node} number is treated as a bundle that needs to be routed.  The library will then attempt to queue the bundle for later transmission and handle any custody request contained in the bundle.

__Fixed Size SDNVs__ - The BP library uses fixed sized SDNVs for all fields that use SDNVs.  This optimizes the generation of data bundles and allows fields to be updated without changing the layout of the bundle in memory.  Bundles that are received by the library can have any size SDNV as long as the encoded value fits within 32-bits.
  
__Creation Time Sequence__ - The BP library never resets a creation time sequence number, but maintains an strictly incrementing value (stepped by one for each successive bundle created) per channel.  This allows receiving nodes to detect gaps in the data.  It also allows reconstruction of the original sending order of the data.

__DTN Aggregate Custody Signaling__ - The BP library only supports custody transfers when DTN Aggregate Custody Signals are used.  Regular RFC5050 custody signals are not supported.  A data bundle received by the library that is requesting custody transfer without a CTEB is dropped.  A custody signal bundle received by the library is ignored.

__Bundle Integrity__ - The BP library inserts a BIB in all data bundles it generates and all data bundles it accepts custody of for forwarding.  Bundles received by the library are not required to have BIBs, but if they do, then the bundle is verified against the BIB prior to accepting custody (if requested) and processing the payload (if destined for the local node).

__Fragmentation__ - In order to preserve the layout of generated data bundles in memory, if a channel allows fragmentation, all bundles created on that channel will be fragmented, even if the entire payload fits within a single bundle.

__Timeouts__ - Timeout processing is not be fully implemented.  The library keeps track of active bundles (i.e. bundles that have been sent but not acknowledged) in a circular buffer of bundle references that maintain strictly incrementing custody IDs. When the application requests a bundle to be loaded, only the oldest active bundle is checked for a timeout.  If the circular buffer wraps and the oldest active bundle has not timed out, then onboard transmission of bundles will either: 1) stop until the oldest bundle times out, 2) force an immediate timeout of the oldest bundle regardless of its actual timeout period, 3) delete the oldest bundle and continue transmitting new bundles.  The implication of this approach is that if large timeouts are used, it is possible for the entire process to be held up by a bundle that fhas not yet been acknowledged even though many other bundles sent after it have.  In this case, there would be potentially plenty of agent resources to keep sending bundles if the oldest bundle could be skipped over, but given processing resource constraints, that is not an option provided.

__Class of Service__ - The class of service of generated data bundles and received bundles is completely ignored.

__Storage Service__ - The BP library requires that a storage service is provided outside the library.  When a channel is created, a set of function pointers are supplied to provide access to the storage service at runtime.  The library comes pre-built with a POSIX file system storage service available to applications.

## Bundle Formats

### Data Bundle 
```
 -------------------------------------------------
                    Data Bundle
 -------------------------------------------------
 |    MSB    |           |           |    LSB    |
 | (8 bits)  | (8 bits)  | (8 bits)  | (8 bits)  |
 |-----------|-----------|-----------|-----------|    0
 |                                               |
 |              Primary Bundle Block             |
 |                                               |
 |-----------------------------------------------|    52
 |                                               |
 |        Custody Transfer Extension Block       |
 |                                               |
 |-----------------------------------------------|    64
 |                                               |
 |             Bundle Integrity Block            |
 |                                               |
 |-----------------------------------------------|    72
 |                                               |    76 --> start of the payload
 |              Bundle Payload Block             |
 |                                               |
 |-----------------------------------------------|    4096 --> variable, not to exceed
```

### DTN Aggregate Custody Signal (DACS)
```
 -------------------------------------------------
             Aggregate Custody Signal
 -------------------------------------------------
 |    MSB    |           |           |    LSB    |
 | (8 bits)  | (8 bits)  | (8 bits)  | (8 bits)  |
 |-----------|-----------|-----------|-----------|    0
 |                                               |
 |              Primary Bundle Block             |
 |                                               |
 |-----------------------------------------------|    52
 |                                               |
 |             Bundle Integrity Block            |
 |                                               |
 |-----------------------------------------------|    60
 |                                               |    64 --> start of the payload
 |              Bundle Payload Block             |
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
 |-----------|-----------|-----------|-----------|
 |  Version  |     Processing Control Flags      |
 |-----------|-----------------------------------|
 |          Block Length of Rest of Block        |
 |-----------------------|-----------------------|
 |   Destination Node    |  Destination Service  |
 |-----------------------|-----------------------|
 |     Source Node       |    Source Service     |
 |-----------------------|-----------------------|
 |    Report To Node     |   Report To Service   |
 |-----------------------|-----------------------|
 |    Custodian Node     |   Custodian Service   |
 |-----------------------|-----------------------|
 |                Creation Seconds               |
 |-----------------------------------------------|
 |               Creation Sequence               |
 |-----------------------------------------------|
 |                   Lifetime                    |
 |-----------------------------------------------|
 |             Dictionary Length = 0             |
 |-----------------------------------------------|
 |               Fragment Offset                 |
 |-----------------------------------------------|
 |                Payload Length                 |
 -------------------------------------------------
```

### Custody Transfer Extension Block (CTEB)
```
 -------------------------------------------------
         Custody Transfer Extension Block
 -------------------------------------------------
 |    MSB    |           |           |    LSB    |
 | (8 bits)  | (8 bits)  | (8 bits)  | (8 bits)  |
 |-----------|-----------|-----------|-----------|
 |                                               |
 |              Primary Bundle Block             |
 |                                               |
 |-----------|-----------|-----------------------|
 .           .           .                       .
 |-----------|-----------|-----------------------|
 |    0xA    | Blk Flags |     Block Length      | +4 --> Custody Transfer Extension Block
 |-----------|-----------|-----------------------|
 |                  Custody ID                   | +8
 |-----------------------|-----------------------|
 |    Custodian Node     |   Custodian Service   | +12
 |-----------------------------------------------|
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
 |-----------|-----------|-----------------------|
 |    0xD    | Blk Flags |     Block Length      | +4 --> Bundle Integrity Block
 |-----------|-----------|-----------------------|
 |    Payload CRC Type   |      Payload CRC      | +8
 -------------------------------------------------
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
 |-----------|-----------|-----------------------|
 |    0x1    | Blk Flags |     Block Length      | --> Payload BLock
 |-----------|-----------|-----------------------|
 |    0x40   |  Status   |  First Custody ID...  | --> Aggregate Custody Signal
 |-----------|-----------|-----------------------|
 |  ...First Custody ID  |   Number Valid CIDs   |
 |-----------------------|-----------------------|
 |  Number Skipped CIDs  |   Number Valid CIDs   |
 |-----------------------|-----------------------|
 |  Number Skipped CIDs  |   Number Valid CIDs   |
 |-----------------------|-----------------------|
 .                       .                       .
 |-----------------------|-----------------------|
 |  Number Skipped CIDs  |   Number Valid CIDs   |
 |-----------------------------------------------|    Variable
```

## Application Programming Interface (API)

| Function        | Purpose |
| --------------- | ------- | 
| [bplib_init](#initialize)             | Initialize the BP library - called once at program start |
| [bplib_open](#open-channel)           | Open a channel - provides handle to channel for future channel operations |
| [bplib_close](#close-channel)         | Close a channel |
| [bplib_getopt](#get-and-set-options)  | Get the setting of a channel option |
| [bplib_setopt](#get-and-set-options)  | Set the setting of a channel option |
| [bplib_store](#store-payload)         | Create a bundle from application data and queue in storage for transmission |
| [bplib_load](#load-bundle)            | Retrieve the next available bundle from storage to transmit |
| [bplib_process](#process-bundle)      | Process a bundle for data extraction, custody acceptance, and/or forwarding |
| [bplib_accept](#accept-payload)       | Retrieve the next available data payload from a received bundle |
| [bplib_routeinfo](#route-information) | Parse bundle and return routing information |

### Initialize

`void bplib_init (void)`

Initializes the BP library.  This must be called before any other call to the library is made.  It also calls the operating system layer initialization routine.

### Open Channel

`int bplib_open(bp_store_t store, bp_ipn_t local_node, bp_ipn_t local_service, bp_ipn_t destination_node, bp_ipn_t destination_service)`

Opens a bundle channel that uses the provided storage service and endpoint IDs. 

This function returns a channel handle that is used for all future operations on the channel.  The open and close calls are mutex'ed against other open and close class, but once a channel is created, operations on that channel are only mutex'ed against other operations on the same channel.  A channel persists until it is closed.

_store_ - a set of callbacks that provide access to the desired storage service

_local_node_ - node component of endpoint ID used by the library for the local agent.  This is used for the source, report to, and custody endpoint IDs.  

_local_service_ - service component of endpoint ID used by the library for the local agent.  This is used for the source, report to, and custody endpoint IDs.  

_destination_node_ - node component of endpoint ID used by the library for the destination of the bundles transmitted on the channel. 

_destination_service_ - service component of endpoint ID used by the library for the destination of the bundles transmitetd on the channel.

_returns_ - channel ID. 

### Close Channel

`void bplib_close(int channel)`

Closes the specified bundle channel and releases all run-time resources associated with it; this does not include the bundles stored in the storage service.

_channel_ - which channel to close

### Get and Set Options

`int bplib_getopt(int channel, int opt, void* val, int len)`
`int bplib_setopt(int channel, int opt, void* val, int len)`

Configures a channel (or retrieves the configuration of a channel).  Note that all options that have a "D" suffix only apply to data bundles that are generated by the BP library through the `bplib_store` function.  

_channel_ - which channel to apply option operation on

_opt_ - the option as described in the table below

| Option                 | Units    | Default | Description |
| ---------------------- | -------- | ------- | ----------- |
| BP_OPT_DSTNODE_D       | bp_ipn_t | destination node (set on channel creation) | Destination node ID of generated data bundles (effectively 28 bits) |
| BP_OPT_DSTSERV_D       | bp_ipn_t | destination node (set on channel creation) | Destination service ID of generated data bundles (effectively 14 bits) |
| BP_OPT_SRCNODE_D       | bp_ipn_t | local node (set on channel creation) | Source node ID of generated data bundles (effectively 28 bits) |
| BP_OPT_SRCSERV_D       | bp_ipn_t | local node (set on channel creation) |Source service ID of generated data bundles (effectively 14 bits) |
| BP_OPT_RPTNODE_D       | bp_ipn_t | local node (set on channel creation) |Report To node ID of generated data bundles (effectively 28 bits) |
| BP_OPT_RPTSERV_D       | bp_ipn_t | local node (set on channel creation) |Report To service ID of generated data bundles (effectively 14 bits) |
| BP_OPT_CSTNODE_D       | bp_ipn_t | local node (set on channel creation) |Custody node ID of generated data bundles (effectively 28 bits) |
| BP_OPT_CSTSERV_D       | bp_ipn_t | local node (set on channel creation) |Custody service ID of generated data bundles (effectively 14 bits) |
| BP_OPT_CREATETIMESYS_D | int      | BP_FALSE | Creation time population - BP_FALSE: use system time at bundle creation, BP_TRUE: use application supplied value |
| BP_OPT_CREATETIMEVAL_D | uint32_t | 0 | Creation time value to use when the channel is configured to use application supplied value; the time is in seconds since 2000:01:01:00:00. |
| BP_OPT_SETSEQUENCE_D   | int      | 0 | Creation sequence number |
| BP_OPT_LIFETIME_D      | int      | 0 | Amount of time in seconds added to creation time specifying duration of time bundle is considered valid, 0: infinite |
| BP_OPT_CSTRQST_D       | int      | BP_TRUE | Custody request - BP_FALSE: leave flag unset in generated data bundles, BP_TRUE: set custody request flag in generated data bundles |
| BP_OPT_ALLOWFRAG_D     | int      | BP_TRUE | Allow fragmentation - BP_FALSE: bundles are not allowed to be fragmented and the allow fragmentation flag is unset in generated data bundles, BP_TRUE: bundles are allowed to be fragmented and the allow fragmentation flag is set in generated data bundles |
| BP_OPT_PAYCRC_D        | int      | BP_BIB_CRC16 | Integrity check type used for payload.  Currently only BP_BIB_CRC16 and BP_BIB_NONE are supported. |
| BP_OPT_TIMEOUT         | int      | 10 | Amount of time in seconds to wait for positive acknowledgment of bundle before retransmitting, 0: infinite |
| BP_OPT_BUNDLELEN       | int      | 4096 | Maximum length of the bundle (including header and extension blocks); if length exceeded, then a fragmented bundle is created |
| BP_OPT_PROCADMINONLY   | int      | BP_FALSE | Configure channel to process administrative records only - BP_TRUE: enable, BP_FALSE: disable |
| BP_OPT_WRAPRSP         | int      | BP_WRAP_RESEND | Configure channel's response when the active table wraps - BP_WRAP_RESEND, BP_WRAP_BLOCK, BP_WRAP_DROP |
| BP_OPT_ACSRATE         | int      | 5 | Minimum rate of ACS generation; various cirucmstances can cause an ACS to be generated, but this rate sets the maximum about time that can elapse before an ACS is automatically generated |

_val_ - the value set or returned

_len_ - the length in bytes of the memory pointed to by _val_

_returns_ - return code (see below).

### Store Payload

`int bplib_store(int channel, void* payload, int size, int timeout)`

Initiates sending the data pointed to by _payload_ as a bundle. The data will be encapsulated in a bundle, or many bundles if the channel allows fragmentation and the payload exceeds the maximum bundle length, and queued in storage for later retrieval and transmission.

_channel_ - channel to create bundle on

_payload_ - pointer to data to be bundled

_size_ - size of payload in bytes

_timeout_ - 0: check, -1: pend, 1 and above: timeout in milliseconds

_returns_ - size of bundle created in bytes, or return code on error (see below).

### Load Bundle

`int bplib_load(int channel, void* bundle,  int size, int timeout, uint32_t* loadflags)`

Reads the next bundle from storage to be sent by the application over the convergence layer.  From the perspective of the BP library, once a bundle is loaded to the application, it is as good as sent.  Any failure of the application to send the bundle is treated no differently that a failure downstream in the bundle reaching its destination. 

_channel_ - channel to retrieve bundle from

_bundle_ - pointer to the buffer for bundle being loaded to be copied into

_size_ - size of the bundle buffer

_timeout_ - 0: check, -1: pend, 1 and above: timeout in milliseconds

_loadflags_ - flags that provide additional information on the result of the load operation (see below)

_returns_ - size of bundle in bytes, or return code on error (see below)

### Process Bundle

`int bplib_process(int channel, void* bundle,  int size, int timeout, uint32_t* procflags)`

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

`int bplib_accept(int channel, void* payload, int size, int timeout, uint32_t* acptflags)`

Returns the next available bundle payload (from bundles that have been received and processed via the `bplib_process` function) to the application by populating the structure pointed to by the _payload_ pointer.

_channel_ - channel to accept payload from

_payload_ - pointer to buffer that the payload will be copied into

_size_ - size of the payload buffer

_timeout_ - 0: check, -1: pend, 1 and above: timeout in milliseconds

_acptflags_ - flags that provide additional information on the result of the accept operation (see below)

_returns_ - size of bundle payload in bytes, or return code on error (see below).

### Route Information

`int bplib_routeinfo(void* bundle, int size, bp_ipn_t* destination_node, bp_ipn_t* destination_service)`

Parses the provided bundle and supplies its destination node and service numbers.  Used to route a received bundle to the appropriate channel by looking up its destination endpoint ID prior to making other library calls that require a channel identifier.

_bundle_ - pointer to a buffer of memory containing a properly formatted bundle

_size_ - size of the bundle

_destination_node_ - pointer to a variable that is populated by the function with the bundle's destination node number

_destination_service_ - pointer to a variable that is populated by the function with the bundle's destination service number

_returns_ - return code (see below)

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
| BP_WRONGCHANNEL         | -14   | The destination service number did not match the channel's source service number when trying to processing a bundle destined for the local node |
| BP_FAILEDINTEGRITYCHECK | -15   | A bundle processed by the library contained a Bundle Integrity Block, but the checksum contained in that block did not match the calculated checksum |
| BP_FAILEDSTORE          | -16   | The library encountered an error originating from the storage service |
| BP_FAILEDRESPONSE       | -17   | The library was unable to report back to another node, e.g. a DACS bundle could not be created or sent due to there being too many sources to track |
| BP_FAILEDOS             | -18   | THe library encountered an error originating from the operation system abstraction layer |
 
### Flags

| Flag                     | Value | Description |
| ------------------------ | ----- | ----------- |
| BP_FLAG_NONCOMPLIANT     | 0x00000001 | Valid bundle but the library was not able to comply with the standard |
| BP_FLAG_INCOMPLETE       | 0x00000002 | At least one block in bundle was not recognized |
| BP_FLAG_REPORTDELETE     | 0x00000004 | A status report must be generated if the bundle is dropped |
| BP_FLAG_TOOMANYSOURCES   | 0x00000008 | There are too many sources to keep track of for ACS bundles |
| BP_FLAG_FILLOVERFLOW     | 0x00000010 | A gap in the CIDs exceeds the max fill value allowed in an ACS bundle |
| BP_FLAG_TOOMANYFILLS     | 0x00000020 | All the fills in the ACS are used |
| BP_FLAG_CIDWENTBACKWARDS | 0x00000040 | The custody ID went backwards |
| BP_FLAG_UNABLETOSTORE    | 0x00000080 | Unable to store the ACS bundle for later transmission |
| BP_FLAG_ROUTENEEDED      | 0x00000100 | The bundle returned needs to be routed before transmission |
| BP_FLAG_STOREFAILURE     | 0x00000200 | Storage service failed to deliver data |
| BP_FLAG_PARSEFAILURE     | 0x00000400 | Storage service failed to deliver data |
| BP_FLAG_MIXEDRESPONSE    | 0x00000800 | Aggregate acknowledgement must have uniform delivery vs. forward |

