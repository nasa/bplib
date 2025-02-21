# bplib

[1. Overview](#1-overview)  
[2. Build with CMake](#2-build-with-cmake)  
[3. Application Design](#3-application-design)  
[4. Application Programming Interface](#4-application-programming-interface)  
[5. Storage Service](#5-storage-service)  

[Note #1 - Bundle Protocol Version 6](docs/bpv6_notes.md)  
[Note #2 - Library Development Guidelines](docs/dev_notes.md)  
[Note #3 - Configuration Parameter Trades](docs/parm_notes.md)  
[Note #4 - Bundle Flow Analysis for Intermittent Communication](docs/perf_analysis_ic.md)

----------------------------------------------------------------------
## 1. Overview
----------------------------------------------------------------------

The Bundle Protocol library (bplib) implements a subset of the RFC9171 Bundle Protocol and targets
embedded space flight applications. The library uses the concept of a bundle socket to manage the
process of encapsulating application data in bundles, and extracting application data out of
bundles.  A socket works much like the standard BSD socket paradigm, and controls how the bundles
are created (e.g. primary header block fields), and how bundles are processed (e.g. payloads
extracted from payload block).

Bplib contains no threads and relies entirely on the calling application for its execution context
and implements a thread-safe blocking I/O model where requested operations will either block according
to the provided timeout, or return an error code immediately if the operation cannot be performed.

Bplib assumes the availability of a persistent queued storage system for managing the rate buffering
that must occur between data and bundle processing. This storage system is provided at run-time by
the application, which can either use its own or can use the included storage service. In addition
to the storage service, bplib needs an operating system interface provided at compile-time. The
source includes a mapping for POSIX compliant operating systems as well as for the NASA OSAL
library.

----------------------------------------------------------------------
## 2. Build with CMake
----------------------------------------------------------------------

#### Prerequisites

1. The build requires Ubuntu 22.04.4 LTS, the __cmake__ build system and a compiler toolchain (by default __gcc__).

Additionally, the __pkg-config__ tool is used to manage the flags required for dependencies.

These can typically be installed via the built-in package management system on most Linux
distributions.

The required packages on Ubuntu 22.04 are: cmake, pkg-config, and build-essential.

The versions used may change. The versions as of the last update of this document are:

- cmake version 3.22.1
- pkg-config 0.29.1
- gcc  (Ubuntu 9.4.0-1ubuntu1~20.04.2) 9.4.0

2. Install QCBOR 1.5.1 if it is not already installed.

- navigate to a directory you want QCBOR to be cloned to (home directory used for this example)
   - `cd ~/`
- clone the QCBOR repo
   - `git clone https://github.com/laurencelundblade/QCBOR.git`
- navigate into the QCBOR repo
   - `cd QCBOR`
- switch to the correct version of QCBOR
   - `git checkout v1.5.1`
- build and install QCBOR
   - `cmake -DBUILD_SHARED_LIBS=ON -S . -B build`
   - `cmake --build build`
   - `sudo make install`

#### Build bplib with cFS
3. Clone, init, and update cFS and all required submodules. Then clone bp and bplib to the cFS local repository.

```
   cd <chosen working directory>
   export CFS_HOME="$(pwd)" # Use CFS_HOME at your discretion
   git clone https://github.com/nasa/cFS "${CFS_HOME}"/cfs-bundle
   cd "${CFS_HOME}"/cfs-bundle
   git submodule init
   git submodule update
   git clone https://github.com/nasa/bp "${CFS_HOME}"/cfs-bundle/apps/bp
   git clone https://github.com/nasa/bplib "${CFS_HOME}"/cfs-bundle/libs/bplib
```

4. Setup OSAL.

Define the OSAL definitions for CMake.
Run CMake for OSAL.
Run Make for OSAL with the destination directory `./osal-staging`.

```
   cd "${CFS_HOME}"/cfs-bundle/osal
   # cmake options from .github/actions/setup-osal/action.yml:
   CMAKE_OSAL_DEFS="-DCMAKE_INSTALL_PREFIX=/usr/local -DOSAL_SYSTEM_BSPTYPE=generic-linux "
   # config-options:
   CMAKE_OSAL_DEFS+="-DCMAKE_BUILD_TYPE=Release -DOSAL_OMIT_DEPRECATED=TRUE "
   CMAKE_OSAL_DEFS+="-DENABLE_UNIT_TESTS=TRUE -DOSAL_CONFIG_DEBUG_PERMISSIVE_MODE=ON "
   cmake $CMAKE_OSAL_DEFS -B "${CFS_HOME}"/osal-build
   cd "${CFS_HOME}"/osal-build
   make DESTDIR="${CFS_HOME}"/osal-staging install
```

5. Build bplib and the test runners

Note that the possible build folders are one of <Debug,Release>-<OSAL,POSIX> for the build type and operating system layer respectively.

Setup the required environment variables for CMake, choosing between Debug or Release, and OSAL or POSIX.
    
```
   # MATRIX_BUILD_TYPE=[Debug|Release]
   # MATRIX_OS_LAYER=[OSAL|POSIX]
   # BPLIB_SOURCE="${CFS_HOME}"/cfs-bundle/libs/bplib
   # BPLIB_BUILD="${CFS_HOME}/bplib-build-matrix-${MATRIX_BUILD_TYPE}-${MATRIX_OS_LAYER}"
```

Run CMake and make all to build bplib and the bplib tests.

```
   export NasaOsal_DIR="${CFS_HOME}/osal-staging/usr/local/lib/cmake"
   cmake \
          -DCMAKE_BUILD_TYPE="${MATRIX_BUILD_TYPE}" \
          -DBPLIB_OS_LAYER="${MATRIX_OS_LAYER}" \
          -DCMAKE_PREFIX_PATH=/usr/local/lib/cmake \
          -S "${BPLIB_SOURCE}" -B "${BPLIB_BUILD}"

   # Build bplib
   cd "${BPLIB_BUILD}"
   make all
```

#### Example Test
6. Test bplib

```
   cd "${CFS_HOME}"/bplib-build-matrix-Debug-OSAL
   common/ut-coverage/coverage-bplib_common-testrunner
```

#### Build bplib Stand Alone
1. Clone bplib into a working directory. The build uses BPLIB_HOME rather than CFS_HOME for a stand alone build.

```
   cd <chosen working directory>
   export BPLIB_HOME="$(pwd)" # Use BPLIB_HOME at your discretion

   git clone https://github.com/nasa/bplib "${BPLIB_HOME}"/bplib
```

2. Build bplib and the test runners

Note that the possible build folders are one of <Debug,Release>-POSIX for the build type and operating system layer respectively.

Setup the required environment variables for CMake, choosing between Debug or Release. POSIX is the only operating system layer supported by the stand alone build.

```
   # MATRIX_BUILD_TYPE=[Debug|Release]
   # MATRIX_OS_LAYER=POSIX
   # BPLIB_SOURCE="${BPLIB_HOME}"/bplib
   # BPLIB_BUILD="${BPLIB_HOME}/bplib-build-matrix-${MATRIX_BUILD_TYPE}-POSIX"
```

Run CMake and make all to build bplib and the bplib tests.

```
   cmake \
          -DCMAKE_BUILD_TYPE="${MATRIX_BUILD_TYPE}" \
          -DBPLIB_OS_LAYER="${MATRIX_OS_LAYER}" \
          -DCMAKE_PREFIX_PATH=/usr/local/lib/cmake \
          -S "${BPLIB_SOURCE}" -B "${BPLIB_BUILD}"

   # Build bplib
   cd "${BPLIB_BUILD}"
   make all
```

#### Test bplib with bpcat
3. Test bplib stand alone

The example program `bpcat` referenced below is available in the bplib stand alone build.

```
$ ./bplib-build-matrix-Debug-POSIX/app/bpcat --help
Usage: ./bplib-build-matrix-Debug-POSIX/app/bpcat [options]
   -l/--local-addr=ipn://<node>.<service> local address to use
   -r/--remote-addr=ipn://<node>.<service> remote address to use
   -i/--input-file=<filename> read input from given file instead of stdin
   -o/--output-file=<filename> write output to given file instead of stdout
      --local-cla-uri=udp://<ip>:<port> Bind local CLA to given IP:port 
      --remote-cla-uri=udp://<ip>:<port> Send bundles to remote CLA at given IP:port
   -d/--delay=<msec> forced inter bundle send delay (20ms default)
   -s/--adu-size=stream chunk (ADU) size to pass to bplib (default and max=15864 bytes)

   Creates a local BP agent with local IPN address as specified.  All data
   received from standard input is forwarded over BP bundles, and all data
   received from bundles is forwarded to standard output.

Example:
   ./bplib-build-matrix-Debug-POSIX/app/bpcat -l ipn://101.1 -r ipn://201.1
```

To test with bpcat:

1. Open two terminal windows and have them both visible. The test runs `bpcat` as a receiver in one window and a sender in the other.
2. In both terminals run:
   `cd <same chosen working directory>/bplib-build-matrix-<Debug or Release>-POSIX/app`
3. The files `bpcat` and `Makefile` should be in the current directory.
4. In one terminal, create the `storage` folder required for the test.
 `mkdir storage`
5. In the same terminal, run the receiver with:
 `./bpcat -l ipn://101.1 -r ipn://201.1 |& tee recv.log`
6. In the other terminal, run the sender with:
 `./bpcat -l ipn://201.1 -r ipn://101.1 -i Makefile |& tee send.log`
7. For a successful test the contents of the Makefile appear in the receiver terminal.
8. Terminate `bpcat` in both terminal windows by pressing `CTRL-C` in each window.

----------------------------------------------------------------------
## 3. Application Design
----------------------------------------------------------------------

![Figure 1](doc/bp_api_architecture.png "BP Library API (Architecture)")

Bplib is written in "vanilla C" and is intended to be linked in as either a shared or static library
into an application with an API for reading/writing application data and reading/writing bundles.

Conceptually, the library is meant to exist inside a flight software framework (such as NASA CFE)
and be presented to the application as a service.  In such a design only the interface for
reading/writing data would be provided to the application, and the interface for reading/writing
bundles would be kept inside the service.  This use case would look a lot like a typical socket
application where a bundle channel (socket) is opened, data is read/written, and then at some
later time the socket is closed.  Underneath, the library takes care of sending and receiving bundles.

In order to support bplib being used directly by the application, both the data and the bundle
interfaces are provided in the API. In these cases, the application is also responsible for sending
and receiving the bundles via a convergence layer adapter (CLA).

An example application design that manages both the data and bundle interfaces could look as follows:
1. A __bundle reader__ thread that receives bundles from a convergence layer and calls bplib to _process_ them
2. A __data writer__ thread that _accepts_ application data from bplib
3. A __bundle writer__ thread that _loads_ bundles from bplib and sends bundles over a convergence layer
4. A __data reader__ thread that _stores_ application data to bplib

The stream of bundles received by the application is handled by the bundle reader and data writer threads.
The __bundle reader__ uses the `bplib_cla_ingress()` function to pass bundles read from the convergence layer into
the library.  If those bundles contain payload data bound for the application, that data is pulled out of the
bundles and queued in storage until the __data writer__ thread calls the `bplib_recv()` function to dequeue
the data out of storage and move it to the application.

Conversely, the stream of bundles sent by the application is handled by the data reader and bundler writer
threads. The __data reader__ thread calls `bplib_send()` to pass data from the application into the library
to be bundled.  Those bundles are queued in storage until the __bundle writer__ threads calls the
`bplib_cla_egress()` function to dequeue them out of storage and move them to the convergence layer.

----------------------------------------------------------------------
## 4. Application Programming Interface
----------------------------------------------------------------------

The application programming interface is documented in the header files, and the documentation can be
generated using the `doxygen` tool.

The NASA CFE/CFS infrastructure has scripts to build the documentation, and this component works with
that infrastructure.  In a CFE/CFS environment, build the "docs" target to generate this documentation.

### 4.1 Functions

| Function                   | Purpose |
| -------------------------- | ------- |
| `bplib_init`               | Initialize the BP library - called once at program start |
| `bplib_create_socket`      | Creates/opens a socket-like entity for application data |
| `bplib_close_socket`       | Closes/Deletes the socket-like entity for application data |
| `bplib_bind_socket`        | Logically binds the socket-like entity to a local IPN node number and service |
| `bplib_connect_socket`     | Logically connects the socket-like entity to a remote IPN node number and service |
| `bplib_create_ram_storage` | Creates a RAM storage (cache) logical entity |
| `bplib_create_node_intf`   | Creates a basic data-passing logical entity |
| `bplib_send`               | Send a single application PDU/datagram over the socket-like interface |
| `bplib_recv`               | Receive a single application PDU/datagram over the socket-like interface |
| `bplib_cla_ingress`        | Receive complete bundle from a remote system |
| `bplib_cla_egress`         | Send complete bundle to remote system |
| `bplib_query_integer`      | Get an operational value |
| `bplib_config_integer`     | Set an operational value |
| `bplib_display`            | Parse bundle and log a break-down of the bundle elements |
| `bplib_eid2ipn`            | Utility function to translate an EID string into node and service numbers |
| `bplib_ipn2eid`            | Utility function to translate node and service numbers into an EID string |


----------------------------------------------------------------------
## 5. Storage Service
----------------------------------------------------------------------

Storage services are still under active development, but the "offload" module
provides an example of a functional filesystem-based storage service.
