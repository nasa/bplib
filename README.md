# Bundle Protocol Library (BPLib)

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

The Bundle Protocol library (BPLib) implements a subset of the [RFC-9171 Bundle Protocol](https://www.rfc-editor.org/rfc/rfc9171.html) and targets embedded space flight applications via [BPNode](https://github.com/nasa/bp), with the goal of implementing the entirety
of RFC-9171. The build 7.0 library uses a UDP convergance layer to manage the process of encapsulating
application data in bundles, and extracting application data out of bundles.

BPLib contains no threads and relies entirely on the calling application for its execution context
and implements a thread-safe blocking I/O model where requested operations will either block according
to the provided timeout, or return an error code immediately if the operation cannot be performed.

Bundles move between tasks using thread-safe queues to perform various ingress, processing, and
egress operations. A sqlite3 database provides persistent storage to bundles that cannot be
immediately delivered and monitors bundles in storage for potential delivery or lifetime
expiration

## 2. Prerequisites

1. The build has only been tested on Ubuntu 20.04.6 LTS, the __cmake__ build system and a compiler
toolchain (by default __gcc__). Additionally, the __pkg-config__ tool is used to manage the flags
required for dependencies. These can typically be installed via the built-in package management
system on most Linux distributions. The required packages and versions on Ubuntu are the following:
- cmake version 3.22.1
- pkg-config 0.29.1
- gcc 9.4.0
- libsqlite3-dev

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

## 3. Building BPLib with BPNode within cFS

To build BPLib within the cFS architecture, see the [BPNode](https://github.com/nasa/bp/tree/main/docs) documentation.

## 5. Building BPLib as a Standalone

1. Clone the repositories
```
# Navigate to the install directory
cd $INSTALL_DIR

# Clone the BPLib repository
git clone https://github.com/nasa/bplib

cd bplib

# Clone the OSAL repository
git clone https://github.com/nasa/OSAL osal

```

If you wish to modify the contact configurations, such as the UDP address or destination EID mappings, you should edit `$INSTALL_DIR/bplib/app/src/bpcat_nc.c`

3. Define build environment variables

```
export NasaOsal_DIR="${INSTALL_DIR}/bplib/osal-staging/usr/local/lib/cmake"
export BPLIB_OS_LAYER=OSAL

# Choose a build configuration - choose between Debug or Release
export MATRIX_BUILD_TYPE=Release

# Define bplib build directory
export BPLIB_BUILD="${INSTALL_DIR}/bplib/bplib-build-matrix-${MATRIX_BUILD_TYPE}-POSIX"
```

4. Build and Install OSAL

```
# Navigate to the bplib directory
cd $INSTALL_DIR/bplib

# Install OSAL
cd osal/
cmake -DCMAKE_INSTALL_PREFIX=/usr/local \
      -DOSAL_SYSTEM_BSPTYPE=generic-linux \
      -DCMAKE_BUILD_TYPE="${MATRIX_BUILD_TYPE}" \
      -DOSAL_OMIT_DEPRECATED=TRUE \
      -DENABLE_UNIT_TESTS=TRUE \
      -DOSAL_CONFIG_DEBUG_PERMISSIVE_MODE=ON -B ../osal-build
cd ../osal-build
make DESTDIR=../osal-staging install
```

4. Build and Install BPLib

```
# Navigate to the cFS directory
cd $INSTALL_DIR/bplib

# Build OSAL with debug configurations
cmake -DCMAKE_VERBOSE_MAKEFILE=ON \
      -DCMAKE_BUILD_TYPE="${MATRIX_BUILD_TYPE}" \
      -DBPLIB_OS_LAYER=OSAL \
      -DCMAKE_PREFIX_PATH=/usr/local/lib/cmake -B "${BPLIB_BUILD}"
cd $BPLIB_BUILD
make all
```

5. Run unit tests
```
ctest --output-on-failure 2>&1 | tee ctest.log
```

## 6. Building a Standalone
```
# Run bpcat executable
cd $INSTALL_DIR/bplib/app
./bpcat
```
