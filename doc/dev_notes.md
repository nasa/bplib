# Developer Notes

## 1.0 Directory Structure

````
bplib               // library build system
|-- binding         // language bindings
    |-- lua         // Lua scripting language
        |-- test    // library unit test framework
|-- common          // utilities and data structures
|-- doc             // documentation
|-- inc             // public API headers
|-- lib             // high level protocol implementation
|-- os              // operating system abstractions
|-- store           // storage services
|-- unittest        // c-code unit tests
|-- v6              // version 6 protocol implementation
````

## 2.0 Layered Architecture

````
+-----------+
|   bplib   |
+-----------+
      |
      |
+-----------+
|    v6     |
+-----------+
      |________________________________________________
      ||              ||              |               |
+-----------+   +-----------+   +-----------+   +-----------+
|    pri    |   |    pay    |   |    bib    |   |   cteb    |
+-----------+   +-----------+   +-----------+   +-----------+
       |               |
 +-----------+   +-----------+
 |   dacs    |   |    sdnv   |
 +-----------+   +-----------+


+-----------+   +-----------+   +-----------+   +-----------+
|  rb_tree  |   |  rh_hash  |   |    cbuf   |   |    crc    |
+-----------+   +-----------+   +-----------+   +-----------+
````

## 3.0 Customization and Extension

#### Storage Service

The storage service is provided at __run-time__ by the application.  The storage service may be managed outside of the library and can contain its own application interfaces for
configuring and statusing.

#### Operating System Abstraction

The operating system abstraction is provided at __compile-time__ and must supply all the functions exported by "bplib_os.h".

Inside "bplib_os.h" there are two #defines that constrain the size of the numbers that the library supports:
* BP_VAL_TYPE - the C type used to represent the largest integer value that can be used by the library.
* BP_INDEX_TYPE - the C type used to represent an index into the active table.

## 4.0 Module Rules

#### Include Rules

1. An application may only include headers from the `inc/` directory.

2. All library modules may include "bplib.h" and "bundle_types.h".

3. The only inclusion of "bplib_os.h" occurs in "bplib.h"; library modules must include "bplib.h" to pull in "bplib_os.h".

4. A library module may only include headers from `common/` modules or from the modules in the layer directly below it in the architecture diagram.

5. No functions or data may be extern'ed in a header file (except in unit tests).  All functions and data types should be pushed to the smallest scope of exposure as possible (i.e. functions only called within a module should be static, typedefs and defines only used within a module's source file should be defined in the source file instead of the header).
