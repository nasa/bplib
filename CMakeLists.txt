project(BPLIB C)

# SANITY CHECK:
# at this point this CMake receipe is only intended for integration with Core Flight System
# This check just confirms that this is being used in that environment.
# BUT - This should be updated to allow standalone build via CMake too.
if (NOT IS_CFS_ARCH_BUILD)
  message(FATAL_ERROR "This CMake build script is intended for use with CFE")
endif()

# Create the app module
add_cfe_app(bplib

  # library objects
  lib/bplib.c

  # OSAL adapter
  os/cfe.c

  # common objects
  common/crc.c
  common/rb_tree.c
  common/rh_hash.c
  common/cbuf.c
  common/lrc.c

  # version 6 objects
  v6/v6.c
  v6/bib.c
  v6/cteb.c
  v6/pay.c
  v6/pri.c
  v6/dacs.c
  v6/sdnv.c

  # storage service objects
  store/file.c
  store/ram.c
  store/flash.c
  store/flash_sim.c
)

# Internal/private header files exist within the implementation directories
target_include_directories(bplib PRIVATE lib store common v6)

# The API to this library (which may be invoked/referenced from other apps)
# is stored in fsw/public_inc.  Using "target_include_directories" is the
# preferred method of indicating this (vs. directory-scope "include_directories").
target_include_directories(bplib PUBLIC inc)
