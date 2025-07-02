# Bundle Protocol Library (BPLib)

[1. Overview](#1-overview)
[2. Prerequisites](#2-prerequisites)
[3. Installing cFS](#3-installing-cfs)
[4. Building BPLib with OSAL](#4-building-bplib-with-osal)
[5. Building a Standalone](#5-building-a-standalone)

## 1. Overview

```
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@...............@@@@@@@@@@@@@@@ @@@@@@@................@@@@@@@@@@ @@@@@@@@@@.......@@@@@@@@.....@@
@@................@@@@@@@@@@@@@@  @@@@@@................@@@@@@@@@@  @@@@@@@@.........@@@@@@@.....@@
@@.....@@@@@@......@@@@@@@@@@ @@@   @@@@@@@@@@.....@@@@@@@@@@@@ @@@   @@@@@@..........@@@@@@.....@@
@@.....@@@@@@@......@@@@@@@@@  @@@   @@@@@@@@@.....@@@@@@@@@@@@  @@@   @@@@@......@....@@@@@.....@@
@@.....@@@@@@@@.....@@@@@  @@@  @@@  @@@@@@@@@.....@@@@@@@@  @@@  @@@  @@@@@......@.....@@@@.....@@
@@.....@@@@@@@@.....@@@@@@  @@@  @@  @@@@@@@@@.....@@@@@@@@@  @@@  @@  @@@@@......@@.....@@@.....@@
@@.....@@@@@@@@.....@@@@@@  @@@  @@  @@@@@@@@@.....@@@@@@@@@  @@@  @@  @@@@@......@@@.....@@.....@@
@@.....@@@@@@@......@@@@@  @@@  @@   @@@@@@@@@.....@@@@@@@@  @@@  @@   @@@@@......@@@.....@@.....@@
@@.....@@@@@........@@@@@@@@@  @@   @@@@@@@@@@.....@@@@@@@@@@@@  @@   @@@@@@......@@@@.....@.....@@
@@.................@@@@@@@@@@ @@  @@@@@@@@@@@@.....@@@@@@@@@@@@ @@  @@@@@@@@......@@@@@..........@@
@@...............@@@@@@@@@@@@@@@ @@@@@@@@@@@@@.....@@@@@@@@@@@@@@@ @@@@@@@@@......@@@@@@........@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
```

The Bundle Protocol library (BPLib) implements a subset of the [RFC9171 Bundle Protocol](https://www.rfc-editor.org/rfc/rfc9171.html) and
targets embedded space flight applications via [BPNode](https://github.com/nasa/bp), with the goal of implementing the entirety
of RFC9171. The build 7.0 library uses UDP convergance layer to manage the process of encapsulating
application data in bundles, and extracting application data out of bundles.

BPLib contains no threads and relies entirely on the calling application for its execution context
and implements a thread-safe blocking I/O model where requested operations will either block according
to the provided timeout, or return an error code immediately if the operation cannot be performed.

Bundles move between tasks using thread-safe queues to perform various ingress, processing, and
egress operations. A sqlite3 database provides persistent storage to bundles that cannot be
immediately delivered and monitors bundles in storage for potential delivery or lifetime
expiration

## 2. Prerequisites

1. The build has only been tested on Ubuntu 22.04.4 LTS, the __cmake__ build system and a compiler
toolchain (by default __gcc__). Additionally, the __pkg-config__ tool is used to manage the flags
required for dependencies. These can typically be installed via the built-in package management
system on most Linux distributions. The required packages on Ubuntu 22.04 are: cmake, pkg-config,
and build-essential. The versions used may change. The versions as of the last update of this
document are:
- cmake version 3.22.1
- pkg-config 0.29.1
- gcc  (Ubuntu 9.4.0-1ubuntu1~20.04.2) 9.4.0
- libsqlit3-dev

2. Create and store an install directory
Location and install directory name are customizable. The intent here is to make the various code
snippets easy to copy-paste.

```
cd ~
mkdir bp_install
cd bp_install
export INSTALL_DIR=$(pwd)
```

3. QCBOR 1.5.1
```
# Navigate to the install directory
cd $INSTALL_DIR

# Clone the QCBOR repo
git clone https://github.com/laurencelundblade/QCBOR.git

# Navigate to the QCBOR repo
cd QCBOR

# Switch to the correct version of QCBOR
git checkout v1.5.1

# Build and install QCBOR
sudo make uninstall
cmake -DBUILD_SHARED_LIBS=ON -S . -B build
cmake --build build
sudo make install
```

## 3. Installing cFS

1. cFS

```
# Navigate to the install directory
cd $INSTALL_DIR

# Clone the cFS repository
git clone https://github.com/nasa/cFS

# Initialize and update the cFS submodules
cd cFS
git submodule init
git submodule update

# Store the location of the cFS install
export CFS_HOME=$(pwd)
```

2. BPLib
```
# Navigate to the cFS library directory
cd $CFS_HOME/libs

# Clone the BPLib repository
git clone git@aetd-git.gsfc.nasa.gov:gsfc-dtn/forks/bp/bplib.git
```

3. BPNode
```
# Navigate to the cFS applications directory
cd $CFS_HOME/apps

# Clone the BPNode repository
git clone git@aetd-git.gsfc.nasa.gov:gsfc-dtn/forks/bp/bpnode.git
```

4. OSAL
```
# Define OSAL environment variables
export NasaOsal_DIR=osal-staging/usr/local/lib/cmake
export BPLIB_OS_LAYER=OSAL

export BPLIB_DEBUG_BUILD=bplib-build-matrix-Debug-OSAL
export BPLIB_RELEASE_BUILD=bplib-build-matrix-Release-OSAL

export CMAKE_BPLIB_DEBUG_DEFS="-DCMAKE_VERBOSE_MAKEFILE=ON \
                               -DCMAKE_BUILD_TYPE=Debug \
                               -DBPLIB_OS_LAYER=OSAL \
                               -DCMAKE_PREFIX_PATH=/usr/local/lib/cmake"

export CMAKE_BPLIB_RELEASE_DEFS="-DCMAKE_VERBOSE_MAKEFILE=ON \
                                 -DCMAKE_BUILD_TYPE=Release \
                                 -DBPLIB_OS_LAYER=OSAL \
                                 -DCMAKE_PREFIX_PATH=/usr/local/lib/cmake"

export CMAKE_OSAL_DEFS="-DCMAKE_INSTALL_PREFIX=/usr/local \
                        -DOSAL_SYSTEM_BSPTYPE=generic-linux \
                        -DCMAKE_BUILD_TYPE=Release \
                        -DOSAL_OMIT_DEPRECATED=TRUE \
                        -DENABLE_UNIT_TESTS=TRUE \
                        -DOSAL_CONFIG_DEBUG_PERMISSIVE_MODE=ON"

# Navigate to the cFS directory
cd $CFS_HOME

# Install OSAL
cd osal/
cmake $CMAKE_OSAL_DEFS -B ../osal-build
cd ../osal-build
make DESTDIR=../osal-staging install
```

## 4. Building BPLib with OSAL

- OSAL with Debug Configurations
```
# Navigate to the cFS directory
cd $CFS_HOME

# Build OSAL with debug configurations
cmake $CMAKE_BPLIB_DEBUG_DEFS -B $BPLIB_DEBUG_BUILD
cd $BPLIB_DEBUG_BUILD
make all
```

- OSAL with Release Configurations
```
# Navigate to the cFS directory
cd $CFS_HOME

# Build OSAL with release configurations
cmake $CMAKE_BPLIB_RELEASE_DEFS -B $BPLIB_RELEASE_BUILD
cd $BPLIB_RELEASE_BUILD
make all
```

- Run unit tests
```
ctest --output-on-failure 2>&1 | tee ctest.log
```

## 5. Building a Standalone
Create a standalone directory for bpcat to run in

```
# Navigate to the install directory
cd $INSTALL_DIR

# Clone the BPLib repository as BPCat
git clone git@aetd-git.gsfc.nasa.gov:gsfc-dtn/forks/bp/bplib.git bpcat
```

At this point, if you wish to modify the UDP configurations for the contact, as well as the
destination EID configurations, you'll should edit `bpcat/app/src/bpcat_nc.c` before moving on

Clone the OSAL repository within the BPLib/BPCat directory. The environment variables for OSAL
should still be defined from the OSAL installation instructions

```
# Clone OSAL
cd bpcat
git clone https://github.com/nasa/osal.git

# Install OSAL
cd osal
cmake $CMAKE_OSAL_DEFS -B ../osal-build
cd ../osal-build
make DESTDIR=../osal-staging install

# Build OSAL
cd ..
cmake $CMAKE_BPLIB_RELEASE_DEFS -B $BPLIB_RELEASE_BUILD
cd $BPLIB_RELEASE_BUILD
make all

# Run unit tests
ctest --output-on-failure 2>&1 | tee ctest.log
```

Run BPCat to start processing bundles sent over UDP by some separate transmitting node. If the
destination EIDs are configured correctly, bundles will be forwarded to a receiving node.

```
# Run bpcat executable
cd app
./bpcat
```