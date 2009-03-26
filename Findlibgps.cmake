# - Find Gpsd libs - gps daemon library
#
# This module finds if Gpsd is installed.
#
# Copyright (c) 2006, Andrew Manson, <g.real.ate@gmail.com>
#

FIND_PATH(libgps_INCLUDES libgpsmm.h)
FIND_PATH(libgpsconf_INCLUDES gpsd_config.h)

FIND_LIBRARY(libgps_LIBRARIES gps)

IF (libgps_LIBRARIES AND libgps_INCLUDES AND libgpsconf_INCLUDES)
   SET(libgps_FOUND TRUE)
ELSE (libgps_LIBRARIES AND libgps_INCLUDES AND libgpsconf_INCLUDES)
    MESSAGE(STATUS "Not building with Gpsd Support")
    IF (libgps_INCLUDES AND NOT libgpsconf_INCLUDES)
        MESSAGE(STATUS "Installed gpsd does not provide gpsd_config.h - see http://lists.berlios.de/pipermail/gpsd-users/2007-August/002819.html for details on how to fix this.")
    ENDIF(libgps_INCLUDES AND NOT libgpsconf_INCLUDES)
ENDIF (libgps_LIBRARIES AND libgps_INCLUDES AND libgpsconf_INCLUDES)

IF (libgps_FOUND)
   IF (NOT libgps_FIND_QUIETLY)
      MESSAGE(STATUS "Found libgps: ${libgps_LIBRARIES}")
   ENDIF (NOT libgps_FIND_QUIETLY)
ELSE (libgps_FOUND)
   IF (libgps_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find libgps")
   ENDIF (libgps_FIND_REQUIRED)
ENDIF (libgps_FOUND)

