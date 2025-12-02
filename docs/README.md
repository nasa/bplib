# BPLib Documentation

Most official documentation on the GSFC Bundle Protocol v7 implementation can be found at: https://github.com/nasa/bp/tree/main/docs. This directory includes some legal forms, documentation for older versions of bplib, and doxygen source files.

## Building Doxygen API Guide

The BPLib Doxygen Build folder `bplib/docs/doc-src` contains the Doxygen source files for the API Guide.

The `bplib/CMakeLists.txt` file includes `docs/doc-src` as a subdirectory to prepare the build folder for the `bplibguide` make target.

The `dtn-cfs/Makefile` has a target `bplibguide` (consistent with the existing `osalguide` for the OSAL API Guide).

Build dtn-cfs with the usual steps, finishing with:

```
make
make install
```

Then run:

```
make bplibguide
```

There will be many lines of output ending with something similar to this:

```
Patching output file 167/168
Patching output file 168/168
lookup cache used 4435/65536 hits=19708 misses=4546
finished...
BPLib API Guide: file:///home/gskenned/repos/gsfc-dtn/dtn-cfs/build/native/default/apps/bplib/docs/doc-src/html/index.html
Built target bplib-apiguide
```

Open the BPLib API Guide: index.html URL from the make log to view the BPLib API Guide.
