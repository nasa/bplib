# Parameter Configuration Notes

[1. Bundle Transmission](#1-bundle-transmission)  
[2. Bundle Acknowledgment](#2-bundle-acknowledgment)  
[3. Active Bundle Management](#3-active-bundle-management)  
[4. Example Configuration](#4-example-configuration)

Bplib provides a set of APIs that perform the delay-tolerant networking functions of a single node within a network.  So while bplib does not define the characteristics of the overall network, the configuration of the local node which uses bplib must be consistent with the characteristics of the network as a whole.  For this reason, many configuration options are available in bplib.  Nodes participating in different networks with different performance goals and physical constraints need to be configured differently in order to maximize the efficiency, reliability, and throughput of the overall network.

What follows is a series of explanations surrounding the key configuration parameters provided by bplib.  Each explanation is a discussion of a trade between resources and desired behaviors - these notes will help you to select which behaviors you want for your network given the resource constraints of your local node. 

## 1. Bundle Transmission

#### Attributes

 - lifetime
 - timeout

#### Options

 - BP_OPT_LIFETIME
 - BP_OPT_TIMEOUT

#### Description

Bplib is implemented as an "aggressive sender" for all outgoing bundles that request custody transfer.  What this means is that bplib will keep trying to send a bundle until it is positively acknowledged.  The two parameters that control this behavior are the bundle's `lifetime` and `timeout`.  The lifetime is how long bplib will keep trying to send a bundle; once the lifetime of a bundle is reached, even if the bundle has not been acknowledged, bplib will stop trying to resend it and will delete it.  The timeout is the rate at which bplib will keep trying send the bundle; a bundle that has been sent but not acknowledged will not be resent until the timeout period expires.  In summary, bplib will keep resending a bundle until it is acknowledged at a rate defined by the _timeout_, for a period of time defined by the _lifetime_.

One nuance for both lifetime and timeout is that they are only used when a bundle is retrieved to be sent.  So it is possible (and likely) for bundles to exist in both storage and in the active table that have expired (their lifetime has been reached), but they will remain there until it is their turn to be sent.  Similarly, there will be bundles in the active table which have timed-out and may not be resent for sometime due to other bundles taking priority over them.  Therefore it should be understood that the lifetime and timeout values of a bundle represent minimum periods of time.

All bundle transmissions begin with a call to the `bplib_load` function which returns the next bundle to be sent by the calling application.  The order in which bundles are returned are as follows:
- DTN Aggregate Custody Signals (DACS)
- Timed-out bundles
- New bundles retrieved from storage

>  
> Possible Issue #1: if the outgoing link rate for bundles is too small and the timeout period too short, the library may do nothing but resend timed-out bundles. 
>  

## 2. Bundle Acknowledgment

#### Attributes

 - dacs_rate
 - max_fills_per_dacs
 - max_gaps_per_dacs

#### Options

 - BP_OPT_DACS_RATE

#### Description

When a bundle received by bplib (via the `bplib_process` function) requests custody transfer, the library will remember that the bundle has been received and will include its acknowledgement in a DTN Aggregate Custody Signal (DACS) that is sent back to the sender.

From the perspective of the library, custody of the bundle has been transferred when the received bundle is successfully queued in the storage service.  Note that the acknowledgment of the custody transfer back to the sender does not occur until the DACS bundle which includes the acknowledgment is sent back and received by the sender.

There are three things that cause a DACS bundle to be sent:

1. The library has accepted custody of at least one bundle and has not yet sent an acknowledgment for any bundle for at least `dacs_rate` number of seconds.  When this occurs, the next time the bplib_load function is called, the library will generate DACS bundles acknowledging all bundles currently in custody.

2. The library has accepted custody of so many bundles that it no longer has any memory left to store all the necessary state information about those bundles.  When this occurs, the library will interrogate the existing state information stored for the bundles it has currently accepted custody of and will generate DACS bundles acknowledging those bundles, and place those DACS bundles in storage, ready to be sent the next time the bplib_load function is called.  The amount of memory allocated on creation of a channel for storing this state information is controlled by the `max_gaps_per_dacs` attribute.  As indicated by the name, memory is not needed per say to remember a specific bundle, but only to remember gaps in the sequence of Custody IDs used to identify bundles.

3. The library has accepted custody of a bundle, and upon storing the state information necessary to acknowledge the bundle, determines that the bundles currently being remembered for an acknowledgment have a Source Endpoint ID that differs from the current bundle's Source Endpoint ID.  When this occurs, the library will interrogate the existing state information stored for the bundles it has currently accepted custody of and will generate DACS bundles acknowledging those bundles, and place those DACS bundles in storage, ready to be sent the next time the bplib_load function is called.

In all cases, the generation of DACS bundles may produce more than one DACS bundle.  The DACS bundle is limited in size by the `max_fills_per_dacs` attribute, so that when the library interrogates the state information for the bundles it has accepted custody of, it may need to generate multiple DACS bundles in order to acknowledge all the bundles.

>  
> Possible Issue #2: If the DACS rate of the receiver is slower than the timeout period of the sender, then bundles will always timeout before they are acknowledged.  If timed-out bundles use the same custody ID (see `CID_REUSE`) then the sending node will be throttled down to the rate of the bundle acknowledgment (i.e. if a link allows 100 bundles per second to be sent, but the receiver is only able to acknowledge 10 bundles per second, then the sender will ultimately only be able to achieve sending 10 bundles per second).  If, on the other hand, timed-out bundles use new custody IDs (see `CID_REUSE`), then the acknowledged bundles will no longer reference valid custody IDs by the time they are received by the sender since the bundles will have already timed-out and been assigned new custody IDs.  The effect of this misconfiguration is that no bundles will ever be acknowledged. 
>  
> Possible Issue #3: If bundles from multiple senders are received by a single bplib channel, the benefits are aggregating acknowledgments within a single DACS bundle is greatly reduced as everytime there is a transition between receiving bundles from one sender to another the library will generate a new DACS bundle and start over aggregating the acknowledgments.
>  

## 3. Active Bundle Management

#### Attributes

 - cid_reuse
 - retransmit_order
 - active_table_size

#### Options

 - BP_OPT_CID_REUSE
 - BP_RETX_OLDEST_BUNDLE
 - BP_RETX_SMALLEST_CID

#### Description

An __active bundle__ is a bundle requesting custody transfer that has been sent for which no acknowledgment has been received.  All active bundles are maintained by the library in what is called the __active table__.  Each bundle in the active table has, at a minimum, three pieces of information associated with it:
1. The bundle's storage ID
2. The absolute time when the bundle was last sent
3. The bundle's custody ID

The purpose of the active table is two-fold: (1) to determine if a bundle has timed-out and needs to be resent, (2) to translate the custody ID provided in a bundle acknowedgment into the storage ID needed to relinquish its resources in the storage service.

The combined need of determining if a bundle has timed-out and which of the timed-out bundles should be sent first, along with the need to be able to randomly access a bundle given a custody ID, imposes a fundemental trade between CPU usage and RAM usage. In a system that had no constraint on CPU, a simple solution could be that every bundle in the active table is looked at each time the `bplib_load` function is called, and the bundle with the oldest transmission time that has timed-out would be returned.  On the other hand, in a system with no constraint on memory, a more sophisticated solution could be maintaining a perfect hash of bundles using their custody ID, superimposed on top of a linked list of the bundles kept in transmission order.  The implementation in bplib does neither, but instead provides two options via the `retransmit_order` attribute that attempt to balance both CPU and memory usage at the cost of complexity: `BP_RETX_OLDEST_BUNDLE`, and `BP_RETX_SMALLEST_CID`.
 
__Oldest Bundle__
- Behavior: 
  * When the retransmit order is set to `BP_RETX_OLDEST_BUNDLE` at channel creation, the library allocates a fixed size hash table (set via the `active_table_size` attribute) that also keeps track of the order in which the bundles were sent.
  * When a bundle is loaded, only the bundle with the oldest transmission time is checked for timing-out.
  * On acknowledgment, the custody ID is hashed and used to directly index the bundle's stored information.  
- Disadvantages:
  * Linking the bundles in time order, along with managing hash collisions requires more than twice the memory needed to store the essential information listed above.
  * The lookup time of an acknowledged bundle is subject to the number of collisions within the hash for the bundle's custody ID.
- Advantages:
  * You can safely reuse custody IDs in order to avoid the effects of mismatches in DACS rates and timeouts.
  * You can safely reuse custody IDs without making your system vulnerable to a single bundle which either fails or is delayed in being acknowledged

__Smallest Custody ID__
- Behavior:
  * When the retransmit order is set to `BP_RETX_SMALLEST_CID` at channel creation, the library allocates a fixed size circular buffer (set via the `active_table_size` attribute) that maintains a running FIFO of bundles based solely on their custody ID.
  * When a bundle is loaded, only the bundle with the smallest custody ID is checked for timing-out.  The design handles the rollover of custody IDs and it can be assumed that from the libraries perspective custody IDs grow infinitely for the purpose of storing them in the active table.
  * On acknowledgment, the custody ID is used to directly index the bundle's stored information.
- Disadvantages:
  * If custody IDs are reused, then the same bundle will keep timing-out, and no other bundles will be able to time-out, until it is acknowledged.
  * New bundles added to the active table cannot have a custody ID that is greater than the smallest custody ID plus the size of the active table.  For example, the active table could be mostly empty (due to bundles being acknowledged), but if there remains an old bundle which has not been acknowledged, and there have been enough bundles sent since that old bundle that the active table has been transversed, then the process of sending new bundles is held up until the old bundle is acknowledged.  This drives the active table size to be larger than what would otherwise be needed, limiting the memory gains otherwised realized by using a simple circular buffer data structure.
- Advantages
  * The check for which bundle needs to be resent as well as looking up a bundle that needs to be acknowledged is extremely efficient.
  * Only the three pieces of information listed above are stored for each bundle - there is no additional per-bundle overhead.

The size of the active table represents the maximum number of bundles that can be pending acknowledgment.  In other words, it is the maximum number of bundles that can be currently enroute.  Therefore, the size of the active table must correspond to the anticipated round-trip time of a bundle on the network between the local sender and the next custody accepting hop.

>  
> Possible Issue #4: Reusing custody IDs when retransmitting the smallest custody ID leaves the system vulnerable to a single bundle stopping the transmission of all bundles.
>  
> Possible Issue #5: Not reusing custody IDs leaves the system vulnerable to unanticipated mismatches between the DACS rate of the receiver and the timeout of the sender.
>  

## 4. Example Configuration

What follows is a notional configuration of a bplib channel which serves more to suggest the relationship between the different parameters  than recommending an actual setting.

__Rate Related Parameters__:
 
* DACS Rate = Round Trip Time x 1.5  
* Timeout = DACS Rate x 1.5  
* Active Table Size = Timeout x Bundle Rate x 1.5  
* Max Gaps per DACS = Active Table Size


__Volume Related Parameters__:
  
* Lifetime = Storage Service Partition Size / Bundle Rate / 1.5
  

__Behavior Related Parameters__:
  
*  CID Reuse = True
*  Retransmit Order = Oldest Bundle

