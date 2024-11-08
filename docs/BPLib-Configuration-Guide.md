# Introduction

## Scope

The purpose of this document is to provide guidelines and conventions
for the configuration and deployment of the Bundle Protocol Library (BPLib).

## Background

The goal of BPLib is to provide a portable and reusable library of Bundle
Protocol APIs for real time embedded system software.

## Applicable Documents

| **Document ID** | **Document Title** |
| --- | --- |
| | |


## Acronyms

|   **Acronym**  |  **Description** |
|---|---|
| OS  | Operating System  |
|  API | Application Programming Interface  |
| CM  | Configuration Management  |
| CPU | Central Processing Unit  |
|  EEPROM |  Electrically Erasable Programmable Read-Only Memory |
|  HW, H/W | Hardware  |
|  RAM |  Random-Access Memory |
|  SW, S/W | Software |
|  TBD | To Be Determined |

## Glossary of Terms

The following table defines the terms used throughout this document.
These terms are identified as proper nouns and are capitalized.

| **Term**    |  **Definition**     |
|---|---|
| Application  (APP)   |  A generic term for a computer program in a desktop or embedded system.  An Application is generally not part of the operating system.  |
|  Application  Programmer's  Interface (API) | A set of routines, protocols, and tools for building software applications  |
| Board Support  Package (BSP)  | A collection of user-provided facilities that interface an OS and the cFE  with a specific hardware platform. The BSP is responsible for hardware initialization.  |
| Core Flight  Executive (cFE)  |  A runtime environment and a set of services for hosting FSW Applications  |
|  Cyclic  Redundancy  Check | A polynomial based method for checking that a data set has remained unchanged from one time period to another.    |
|  Developer  | Anyone who is coding a software Application. |
|  Hardware  Platform  | The target hardware that hosts the Operating System and Application. |
| Interface  Control  Document |  A document that describes the software interface, in detail, to another piece  of software or hardware.  |
|  I/O Data     | Any data being written to and read from an I/O port. No structure is placed  on the data and no distinction as  to the type of I/O device. I/O data is defined separately  from memory data because it has a separate API and it's an optional interface of the cFE.  |
| Log   |   A collection of data that an application stores that provides information to diagnose  and debug FSW problems.  |
|  Memory Data |  Any data being written to and read from memory. No structure is placed  on the data and no distinction as to the type of memory is made.  |
| MMU   |  Memory Management Unit. A piece of hardware that manages virtual memory systems.  It automatically translates addresses into physical addresses so that an application can  be linked with one set of addresses but actually reside in a different part of memory. |
| Network | A connection between subsystems used for communication purposes.  |
| Platform  | See "Hardware Platform" above.   |
| User  | Anyone who interacts with the Software Application or system in its operational state.  A user can be a developer, a tester, an operator, or a maintainer.

# How to Configure, Build, and Run BPLib


The BPLib distribution includes a complete development environment intended for
integration with the Core Flight System (cFS) or a standalone build.

The following sections provide instructions on how to:

-   Setup the build environment

-   Configure the build directory for BPLib integration with an application

-   Configure BPLib for a standalone build

-   Build BPLib integrated with an application

-   Build BPLib as a standalone build

In the current BPLib release, the CMake tool is used to generate makefiles
to perform the actual build.  The "cmake" build offers increased features by
introducing additional scripting and build-time configurability,
allowing direct inclusion into larger projects with less need to modify
files to support a specific target or configuration.

## Setup the Build Environment

This section details the steps needed to setup the BPLib source
distribution and prepare the host development environment to build
BPLib.

### Setup the BPLib Source Distribution

Get a copy of the BPLib source distribution directory on your build
machine. The source distribution has the following directories:

#### BPLib source distribution directories

| **Directory** |  **Description**  |
|---|---|
| `bplib`                        | The top level BPLib source distribution directory.  |
| `bplib/inc`                    | The inc directory contains the top level header files. |
| TBD                            | TBD |


## Configure the BPLib Parameter File

TBD

#### BPLib configuration parameters

TBD

## Setting up the "cmake" build

TBD

Typically, BPLib is not built by itself, but rather as a library to be
used within a larger application. The BPLib cmake build system allows
both options; building as a standalone entity for testing purposes, or
building as a sub-component within a larger project. The same scripts
are used in both cases, and no modification is required.

### Prerequisites

TBD - Needs editing

In order to build BPLib using cmake, the "cmake" package must be
installed for the host/development machine. On Linux, this is generally
available via the respective Linux distribution package management
system, i.e. "yum" on RedHat and derivatives, or "apt-get" on Debian and
derivatives. For other operating systems, the cmake tool is available in
source and binary form from <http://cmake.org>. BPLib requires at least
version 3.5 of the cmake tool.

**RHEL/CentOS users**: For RHEL releases prior to 8.0, a compatible
cmake version is available via the EPEL repository in a package called
"cmake3".  If using this package, the `cmake` command in the examples in
this document should be replaced with `cmake3`.

### Variables that must be specified

The BPLib cmake build is controlled by several user-supplied variables
when the build is first provisioned:

| **CMake variable**  | **Valid selections**   |  **Notes** |
|---|---|---|
| `TBD`  | Any directory name  | Optional. (or Mandatory.) <Description> |


It is important to note that the values specified for these variables
are **automatically cached** by the cmake build system. It is only
necessary to specify these values when first provisioning/configuring a
build; these are *not* required when simply building the binaries.

This caching function removes the need for environment variables to be
set in the development shell. The generated makefiles do not require the
user to set any environment variables, as all necessary context information
is automatically stored in the cache when the build is first provisioned.

### Setting up a standalone BPLib build manually

The BPLib may be built standalone in order to evaluate the library for a
particular target and/or execute the included unit tests.

In the cmake build system, all generated files are placed in a dedicated
"binary" directory that is separate from the source tree. To provision a
build, the user must first create the binary directory by issuing the
"mkdir" command (on Linux), preferably outside the BPLib source tree.
Then, the "cmake" provisioning tool is invoked to generate the actual
makefiles, supplying values for the required variables.

The following example will generate a build tree that allows testing
of the BPLib library on a standard Linux workstation:

TBD

EXAMPLE ONLY - Needs editing

```bash
$ mkdir build
$ cd build
$ cmake \
    -DCMAKE_BUILD_TYPE=debug \
    -DENABLE_UNIT_TESTS=true \
    -DBPLib_SYSTEM_BSPTYPE=generic-linux \
    -DBPLib_CONFIG_DEBUG_PERMISSIVE_MODE=true \
     /path/to/bplib/source
```

The cmake provisioning tool generates standard makefiles in the build
directory. To build the BPLib binaries, simply run "make" in the build
directory.

### Setting up a standalone BPLib build using the Makefile wrapper

TBD Copy in from elsewhere

### Integrating BPLib into a larger build

TBD Copy in from elsewhere

#### Linking BPLib into another target

TBD

# Revision History

| Revision Number |  Release Date | Changes to Prior Revision  | Approval |
|---|---|---|---|
| 1.0  | TBD | Initial Release.  | <Approver Name>  |
| 1.1 | TBD | Updates for ... | <Approver Name>  |
