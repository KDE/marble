# - Find Qt Location libs
#
# This module determines whether QtLocation is installed.
#
# Copyright (c) 2011, Daniel Marth, <danielmarth@gmx.at>
#

FIND_PACKAGE( QtMobility )
IF( QTMOBILITY_FOUND )
  FIND_PATH( QTLOCATION_INCLUDE_DIR NAMES QGeoPositionInfoSource PATH_SUFFIXES QtLocation )
  FIND_LIBRARY( QTLOCATION_LIBRARIES NAMES QtLocation )
ENDIF( QTMOBILITY_FOUND )

FIND_PACKAGE_HANDLE_STANDARD_ARGS( QtLocation DEFAULT_MSG QTLOCATION_INCLUDE_DIR QTLOCATION_LIBRARIES )