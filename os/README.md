## README for the Temporary "os" folder tree

The files in the "os" folder tree are temporary to enable the STOR CACHE tests to work.

I intend to move the required files from the "os" tree to bplib/ci/mem or another appropriate location.

I intend to remove the "os" tree prior to the merge of the DTNN-339 branch into "main".

The files are from CAPSTONE and I mainly use the "heap" code for memory allocation in unit tests.

```
os/src/cfe.c
os/src/heap.c
os/src/osal.c
os/src/posix.c
os/ut-stubs/CMakeLists.txt
os/ut-stubs/bplib_os_stubs.c
os/ut-stubs/bplib_os_handlers.c
os/CMakeLists.txt
os/ut-coverage/CMakeLists.txt
os/ut-coverage/libc_stdlib_stubs.c
os/ut-coverage/inc/bplib_cs_stdlib.h
os/ut-coverage/override_inc/stdlib.h
os/ut-coverage/test_bplib_os.c
os/inc/bplib_os.h
```