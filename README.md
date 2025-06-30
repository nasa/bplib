# core Flight System (cFS) Bundle Protocol Library (BPLib)

## Introduction

The Bundle Protocol library ([BPLib](https://github.com/nasa/bp)) implements a subset of the RFC9171 Bundle Protocol and targets
embedded space flight applications. The library uses the concept of a bundle socket to manage the
process of encapsulating application data in bundles, and extracting application data out of
bundles. A socket works much like the standard BSD socket paradigm, and controls how the bundles
are created (e.g. primary header block fields), and how bundles are processed (e.g. payloads
extracted from payload block).

BPLib contains no threads and relies entirely on the calling application for its execution context
and implements a thread-safe blocking I/O model where requested operations will either block according
to the provided timeout, or return an error code immediately if the operation cannot be performed.

BPLib assumes the availability of a persistent queued storage system for managing the rate buffering
that must occur between data and bundle processing. This storage system is provided at run-time by
the application, which can either use its own or can use the included storage service. In addition
to the storage service, BPLib needs an operating system interface provided at compile-time. The
source includes a mapping for POSIX compliant operating systems as well as for the NASA OSAL
library.

## Software Required

cFS Framework (cFE, OSAL, PSP)
An integrated bundle including the cFE, OSAL, and PSP can be obtained at https://github.com/nasa/cfs