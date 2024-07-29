# bplib

[1. Overview](#1-overview)  
[2. Build with CMake](#2-build-with-cmake)  
[3. Application Design](#3-application-design)  
[4. Application Programming Interface](#4-application-programming-interface)  
[5. Storage Service](#5-storage-service)  

[Note #1 - Bundle Protocol Version 6](doc/bpv6_notes.md)  
[Note #2 - Library Development Guidelines](doc/dev_notes.md)  
[Note #3 - Configuration Parameter Trades](doc/parm_notes.md)  
[Note #4 - Bundle Flow Analysis for Intermittent Communication](doc/perf_analysis_ic.md)

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

2. Install tinycbor if it is not already installed.  
See https://github.com/intel/tinycbor.git.

- tinycbor version 0.6.0

#### Build bplib with cFS
3. Create a subdirectory for building bplib. Run CMake to set up the build tree. Build bplib by running __make__ in the build subdirectory:

```sh
   git clone https://github.com/nasa/cFS cfs_bundle
   cd ./cfs_bundle
   git submodule init
   git submodule update
   git clone https://github.com/nasa/bp ./apps/bp
   git clone https://github.com/nasa/bplib ./libs/bplib
```

3. Setup OSAL.

Define the OSAL definitions for CMake.
Run CMake for OSAL.
Run Make for OSAL with the destination directory `./osal-staging`.

```
   cd ./cfs-bundle/osal
   # cmake options from .github/actions/setup-osal/action.yml
   CMAKE_OSAL_DEFS="-DCMAKE_INSTALL_PREFIX=/usr/local -DOSAL_SYSTEM_BSPTYPE=generic-linux "
   # config-options:
   CMAKE_OSAL_DEFS+="-DCMAKE_BUILD_TYPE=Release -DOSAL_OMIT_DEPRECATED=TRUE "
   CMAKE_OSAL_DEFS+="-DENABLE_UNIT_TESTS=TRUE -DOSAL_CONFIG_DEBUG_PERMISSIVE_MODE=ON "
   cmake $CMAKE_OSAL_DEFS -B ../osal-build
   cd ../osal-build
   make DESTDIR=../osal-staging install
```

4. Build the test runners

Note that the possible build folders are one of <Debug/Release>-<OSAL/POSIX> for the build type and operating system layer respectively.

```sh
   # Create the build folder based on Debug/Release and OSAL/POSIX
   # MATRIX_BUILD_TYPE=[Debug|Release]
   # MATRIX_OS_LAYER=[OSAL|POSIX]
   # BPLIB_SOURCE=<path-to-cfs>/libs/bplib
   # BPLIB_BUILD=./bplib-build-matrix-<MATRIX_BUILD_TYPE>-<MATRIX_OS_LAYER>
   #   one of:
   # BPLIB_BUILD=./bplib-build-matrix-Debug-OSAL
   # BPLIB_BUILD=./bplib-build-matrix-Debug-POSIX
   # BPLIB_BUILD=./bplib-build-matrix-Release-OSAL
   # BPLIB_BUILD=./bplib-build-matrix-Release-POSIX
   
   cmake \
          -DCMAKE_BUILD_TYPE="${MATRIX_BUILD_TYPE}" \
          -DBPLIB_OS_LAYER="${MATRIX_OS_LAYER}" \
          -DCMAKE_PREFIX_PATH=/usr/local/lib/cmake \
          -S "${BPLIB_SOURCE}" -B "${BPLIB_BUILD}"

   # Build bplib
   cd "${BPLIB_BUILD}"
   make all
```

5. Test bplib

#### Example Test


```
   export NasaOsal_DIR=osal-staging/usr/local/lib/cmake
   cd ./bplib-build-matrix-Debug-OSAL
   common/ut-coverage/coverage-bplib_common-testrunner
```

#### Build bplib Stand Alone
6. Create a subdirectory for building bplib.

```sh
   git clone https://github.com/nasa/bplib ./bplib
```

7. Build the test runners

Note that the possible build folders are one of <Debug/Release>-<OSAL/POSIX> for the build type and operating system layer respectively.

```sh
   # Create the build folder based on Debug/Release and OSAL/POSIX
   # MATRIX_BUILD_TYPE=[Debug|Release]
   # MATRIX_OS_LAYER=[OSAL|POSIX]
   # BPLIB_SOURCE=<path-to-cfs>/libs/bplib
   # BPLIB_BUILD=./bplib-build-matrix-<MATRIX_BUILD_TYPE>-<MATRIX_OS_LAYER>
   #   one of:
   # BPLIB_BUILD=./bplib-build-matrix-Debug-OSAL
   # BPLIB_BUILD=./bplib-build-matrix-Debug-POSIX
   # BPLIB_BUILD=./bplib-build-matrix-Release-OSAL
   # BPLIB_BUILD=./bplib-build-matrix-Release-POSIX
   
   # Run cmake from the current directory with the CMake folder
   # indicated by -S "$BPLIB_SOURCE}".

   cmake \
          -DCMAKE_BUILD_TYPE="${MATRIX_BUILD_TYPE}" \
          -DBPLIB_OS_LAYER="${MATRIX_OS_LAYER}" \
          -DCMAKE_PREFIX_PATH=/usr/local/lib/cmake \
          -S "${BPLIB_SOURCE}" -B "${BPLIB_BUILD}"

   # Build bplib
   cd "${BPLIB_BUILD}"
   make all
```

8. Test bplib stand alone

The example program `bpcat` is available in the bplib stand alone build

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
that infrastructure.  In a CFE/CFS envirnomnent, build the "docs" target to generate this documentation.

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
