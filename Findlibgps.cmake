# - Find Gpsd libs - gps daemon library
#
# This module finds if Gpsd is installed.
#
# Copyright (c) 2006, Andrew Manson, <g.real.ate@gmail.com>
#

FIND_PATH(libgps_INCLUDES libgpsmm.h)


FIND_LIBRARY(libgps_LIBRARIES gps)

IF( NOT libgps_LIBRARIES)
    MESSAGE(STATUS "Not building with Gpsd Support")
ENDIF( NOT libgps_LIBRARIES)

IF (libgps_LIBRARIES AND libgps_INCLUDES)
   SET(libgps_FOUND TRUE)
ENDIF (libgps_LIBRARIES AND libgps_INCLUDES)

IF (libgps_FOUND)
   IF (NOT libgps_FIND_QUIETLY)
      MESSAGE(STATUS "Found libgps: ${libgps_LIBRARIES}")
   ENDIF (NOT libgps_FIND_QUIETLY)
ELSE (libgps_FOUND)
   IF (libgps_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find libgps")
   ENDIF (libgps_FIND_REQUIRED)
ENDIF (libgps_FOUND)

