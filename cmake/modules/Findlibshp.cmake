# LIBSHP_FOUND - system has the LIBSHP library
# LIBSHP_INCLUDE_DIR - the LIBSHP include directory
# LIBSHP_LIBRARIES - The libraries needed to use LIBSHP

if(LIBSHP_INCLUDE_DIR AND LIBSHP_LIBRARIES)
  set(LIBSHP_FOUND TRUE)
else(LIBSHP_INCLUDE_DIR AND LIBSHP_LIBRARIES)

  find_path(LIBSHP_INCLUDE_DIR NAMES shapefil.h PATH_SUFFIXES libshp)
  find_library(LIBSHP_LIBRARIES NAMES shp)

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(libshp DEFAULT_MSG LIBSHP_INCLUDE_DIR LIBSHP_LIBRARIES)

  mark_as_advanced(LIBSHP_INCLUDE_DIR LIBSHP_LIBRARIES)
endif(LIBSHP_INCLUDE_DIR AND LIBSHP_LIBRARIES)
