# Install script for directory: /home/gskenned/repos/nasa-gitlab/gsfc-dtn/dtn-cfs/libs/bplib/QCBOR

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/gskenned/repos/nasa-gitlab/gsfc-dtn/dtn-cfs/libs/bplib/QCBOR/build-folder/libqcbor.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/qcbor" TYPE FILE FILES
    "/home/gskenned/repos/nasa-gitlab/gsfc-dtn/dtn-cfs/libs/bplib/QCBOR/inc/qcbor/qcbor.h"
    "/home/gskenned/repos/nasa-gitlab/gsfc-dtn/dtn-cfs/libs/bplib/QCBOR/inc/qcbor/qcbor_common.h"
    "/home/gskenned/repos/nasa-gitlab/gsfc-dtn/dtn-cfs/libs/bplib/QCBOR/inc/qcbor/qcbor_private.h"
    "/home/gskenned/repos/nasa-gitlab/gsfc-dtn/dtn-cfs/libs/bplib/QCBOR/inc/qcbor/qcbor_encode.h"
    "/home/gskenned/repos/nasa-gitlab/gsfc-dtn/dtn-cfs/libs/bplib/QCBOR/inc/qcbor/qcbor_decode.h"
    "/home/gskenned/repos/nasa-gitlab/gsfc-dtn/dtn-cfs/libs/bplib/QCBOR/inc/qcbor/qcbor_spiffy_decode.h"
    "/home/gskenned/repos/nasa-gitlab/gsfc-dtn/dtn-cfs/libs/bplib/QCBOR/inc/qcbor/UsefulBuf.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/gskenned/repos/nasa-gitlab/gsfc-dtn/dtn-cfs/libs/bplib/QCBOR/build-folder/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
