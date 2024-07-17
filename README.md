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

1. The build requires the __cmake__ build system and a compiler toolchain (by default __gcc__).

Additionally, the __pkg-config__ tool is used to manage the flags required for dependencies.

These can typically be installed via the built-in package management system on most Linux
distributions.

On Debian/Ubuntu and derivatives:

Note: The scripts on this page are available as example scripts in bplib/doc/example-scripts. The example scripts mimic the BPLib GitHub Actions and Workflows. The scripts have bash `trap` and `set -o pipefail` statements to help if errors occur. The names of the example scripts are used as labels here. For example `bplib/doc/example-scripts/install-toolchain` contains `install-toolchain` (part 1) and `install-toolchain` (part 2).

`install-toolchain` (part 1)
```sh
   sudo apt install cmake pkg-config build-essential
```

2. For BPv7 this uses the TinyCBOR library at https://github.com/intel/tinycbor.  As any
distribution-packaged version may be outdated, it is recommended to compile this from source.
As of this writing, the library uses a simple Makefile that will install into `/usr/local`.
This installation prefix can be changed by editing the Makefile before building.  Otherwise,
to install into the default location, steps are as follows:

`install-toolchain` (part 2)
```sh
   git clone https://github.com/intel/tinycbor.git tinycbor-source
   mkdir tinycbor-staging
   pushd tinycbor-source
   make all && make DESTDIR=../tinycbor-staging install
   popd
   # Install from tinycbor-staging to /usr/local/include, lib, and bin
   sudo cp -rv -t / tinycbor-staging/*
```

3. Setup OSAL. See `bplib/doc/example-scripts/setup-osal`.

`setup-osal`
```sh
   cd $CFS_HOME/../cfs-bundle/osal
   # cmake options from .github/actions/setup-osal/action.yml
   CMAKE_OSAL_DEFS="-DCMAKE_INSTALL_PREFIX=/usr/local -DOSAL_SYSTEM_BSPTYPE=generic-linux "
   # config-options:
   CMAKE_OSAL_DEFS+="-DCMAKE_BUILD_TYPE=Release -DOSAL_OMIT_DEPRECATED=TRUE "
   CMAKE_OSAL_DEFS+="-DENABLE_UNIT_TESTS=TRUE -DOSAL_CONFIG_DEBUG_PERMISSIVE_MODE=ON "
   cmake $CMAKE_OSAL_DEFS -B $CFS_HOME/osal-build
   cd $CFS_HOME/osal-build
   make DESTDIR=$CFS_HOME/osal-staging install
```

#### Build bplib
4. Create a subdirectory for building bplib. Run CMake to set up the build tree. Build bplib by running __make__ in the build subdirectory:

The bplib/doc/example-scripts/cfs-env-vars may be helpful during building.

```sh
   export CFS_HOME=~/cfs-home
   export CFS_REPO=~/repos/cfs-bundle
   export BP_SOURCE=$CFS_REPO/apps/bp
   export BPLIB_SOURCE=$CFS_REPO/libs/bplib
```

The cfs-env-vars file is just an example. In practice, if ~/.profile defines CFS_HOME, then `source $CFS_HOME/cfs-env-vars` will always set the environment variables.

CFS_REPO is the folder containing the working copy of the https://github.com/nasa/cFS repository. In this case it was cloned and initialized with:

`initialize-cfs-bundle`
```sh
   source "${CFS_HOME}/cfs-env-vars"
   git clone https://github.com/nasa/cFS "${CFS_REPO}"
   cd "${CFS_REPO}"
   git submodule init
   git submodule update
   git clone https://github.com/nasa/bp "${BP_SOURCE}"
   git clone https://github.com/nasa/bpib "${BPLIB_SOURCE}"
```

Most of the example scripts include `cd $CFS_HOME` because the example scripts should be copied to $CFS_HOME to be run.

`bplib-testdriver` 
See `bplib/doc/example-scripts/bplib-testdriver`.
The bplib build/unit test script
Excerpted from `bplib/doc/example-scripts/bplib-unit-test-functional`
```sh
   # Create the build folder based on Debug/Release and OSAL/POSIX
   # MATRIX_BUILD_TYPE=[Debug|Releas]
   # MATRIX_OS_LAYER=[OSAL|POSIX]
   # BPLIB_SOURCE=<path>/cfs/libs/bplib
   # BPLIB_BUILD=$CFS_HOME/bplib-build-matrix-<MATRIX_BUILD_TYPE>-<MATRIX_OS_LAYER>
   #   one of:
   # BPLIB_BUILD=$CFS_HOME/bplib-build-matrix-Debug-OSAL
   # BPLIB_BUILD=$CFS_HOME/bplib-build-matrix-Debug-POSIX
   # BPLIB_BUILD=$CFS_HOME/bplib-build-matrix-Release-OSAL
   # BPLIB_BUILD=$CFS_HOME/bplib-build-matrix-Release-POSIX
   
   cmake \
          -DCMAKE_BUILD_TYPE="${MATRIX_BUILD_TYPE}" \
          -DBPLIB_OS_LAYER="${MATRIX_OS_LAYER}" \
          -DCMAKE_PREFIX_PATH=/usr/local/lib/cmake \
          -S "${BPLIB_SOURCE}" -B "${BPLIB_BUILD}"

   # Build bplib
   cd "${BPLIB_BUILD}"
   make all
```

4. Test bplib
#### Example Application

For those that learn better through examples, an example application is provided in the  
`bplib-build-matrix-Debug-POSIX/app`  
directory.  This example program is not intended to be complete, but provides a quick way to
see how to use the library.  After building and installing bplib on your system, the `bpcat`
program provides a functionality similar to netcat for bplib.

test-bpcat (not in example-scripts)
```sh
   cd $CFS_HOME/bplib-build-matrix-Debug-POSIX/app # Must be POSIX.

   mkdir storage
   ./bpcat -l ipn://101.1 -r ipn://201.1
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
