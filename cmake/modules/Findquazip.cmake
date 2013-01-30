# QUAZIP_FOUND - system has the QUAZIP library
# QUAZIP_INCLUDE_DIR - the QUAZIP include directory
# QUAZIP_LIBRARIES - The libraries needed to use QUAZIP

if(QUAZIP_INCLUDE_DIR AND QUAZIP_LIBRARIES)
  set(QUAZIP_FOUND TRUE)
else(QUAZIP_INCLUDE_DIR AND QUAZIP_LIBRARIES)

  find_path(QUAZIP_INCLUDE_DIR NAMES quazip.h PATH_SUFFIXES quazip)
  find_library(QUAZIP_LIBRARIES NAMES quazip)

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(quazip DEFAULT_MSG QUAZIP_INCLUDE_DIR QUAZIP_LIBRARIES)

  mark_as_advanced(QUAZIP_INCLUDE_DIR QUAZIP_LIBRARIES)
endif(QUAZIP_INCLUDE_DIR AND QUAZIP_LIBRARIES)
