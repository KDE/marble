# QUAZIP_FOUND - system has the QUAZIP library
# QUAZIP_INCLUDE_DIR - the QUAZIP include directory
# QUAZIP_LIBRARIES - The libraries needed to use QUAZIP

if(QUAZIP_INCLUDE_DIR AND QUAZIP_LIBRARIES)
  set(QUAZIP_FOUND TRUE)
else(QUAZIP_INCLUDE_DIR AND QUAZIP_LIBRARIES)

  if(QT5BUILD)
  find_path(QUAZIP_INCLUDE_DIR NAMES quazip.h PATH_SUFFIXES quazip5 quazip)
  find_library(QUAZIP_LIBRARIES NAMES quazip5 quazip-qt5)
  else(QT5BUILD)
  find_path(QUAZIP_INCLUDE_DIR NAMES quazip.h PATH_SUFFIXES quazip)
  find_library(QUAZIP_LIBRARIES NAMES quazip)
  endif(QT5BUILD)

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(quazip DEFAULT_MSG QUAZIP_INCLUDE_DIR QUAZIP_LIBRARIES)

  mark_as_advanced(QUAZIP_INCLUDE_DIR QUAZIP_LIBRARIES)
endif(QUAZIP_INCLUDE_DIR AND QUAZIP_LIBRARIES)
