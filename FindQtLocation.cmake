# - Find Qt Location libs
#
# This module determines whether QtLocation is installed.
#
# Copyright (c) 2011, Daniel Marth, <danielmarth@gmx.at>
# Copyright (c) 2011, Bernhard Beschow, <bbeschow@cs.tu-berlin.de>
#

# try using pkg-config first
include(FindPkgConfig)

pkg_check_modules(QtLocation QtLocation>=1.0.1)

IF(QtLocation_FOUND)
  SET(QTLOCATION_FOUND ${QtLocation_FOUND})
  SET(QTLOCATION_INCLUDE_DIR ${QtLocation_INCLUDE_DIRS})
  SET(QTLOCATION_LIBRARIES ${QtLocation_LIBRARIES})
ENDIF(QtLocation_FOUND)

# no success using pkg-config, try manually
IF(NOT QTLOCATION_FOUND)
  FIND_PACKAGE(QtMobility)
  IF(QTMOBILITY_FOUND)
    FIND_PATH(QTLOCATION_INCLUDE_DIR NAMES QGeoPositionInfoSource PATH_SUFFIXES QtLocation)
    FIND_LIBRARY(QTLOCATION_LIBRARIES NAMES QtLocation)
  ENDIF(QTMOBILITY_FOUND)

  FIND_PACKAGE_HANDLE_STANDARD_ARGS( QtLocation DEFAULT_MSG QTLOCATION_INCLUDE_DIR QTLOCATION_LIBRARIES )
ENDIF(NOT QTLOCATION_FOUND)
