ADD_SUBDIRECTORY( flightgear )

FIND_PACKAGE(libgps)
IF(LIBGPS_FOUND)
     MESSAGE( STATUS "Building with gpsd position provider")
     ADD_SUBDIRECTORY( gpsd )
ELSE(LIBGPS_FOUND)
     MESSAGE( STATUS "Not building with gpsd position provider")
ENDIF(LIBGPS_FOUND)

FIND_PACKAGE(liblocation)
IF(liblocation_FOUND)
     MESSAGE( STATUS "Building with Maemo GPS support")
     ADD_SUBDIRECTORY( maemo )
ENDIF(liblocation_FOUND)

FIND_PACKAGE(QtLocation)
IF(QTLOCATION_FOUND)
     MESSAGE( STATUS "Building with QtLocation support")
     ADD_SUBDIRECTORY( qtmobility )
ENDIF(QTLOCATION_FOUND)

FIND_PACKAGE(libwlocate)
IF(LIBWLOCATE_FOUND)
     MESSAGE( STATUS "Building with libwlocate support")
     ADD_SUBDIRECTORY( wlocate )
ENDIF()

ADD_SUBDIRECTORY( routesimulation )
ADD_SUBDIRECTORY( placemark )

# experimental implementation
# ADD_SUBDIRECTORY( geoclue )
