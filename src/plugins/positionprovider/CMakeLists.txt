ADD_SUBDIRECTORY( flightgear )

macro_optional_find_package(libgps)
IF(LIBGPS_FOUND)
     MESSAGE( STATUS "Building with gpsd position provider")
     ADD_SUBDIRECTORY( gpsd )
ELSE(LIBGPS_FOUND)
     MESSAGE( STATUS "Not building with gpsd position provider")
ENDIF(LIBGPS_FOUND)

macro_optional_find_package(liblocation)
IF(liblocation_FOUND)
     MESSAGE( STATUS "Building with Maemo GPS support")
     ADD_SUBDIRECTORY( maemo )
ENDIF(liblocation_FOUND)

macro_optional_find_package(QtLocation)
IF(QTLOCATION_FOUND)
     MESSAGE( STATUS "Building with QtLocation support")
     ADD_SUBDIRECTORY( qtmobility )
ENDIF(QTLOCATION_FOUND)

ADD_SUBDIRECTORY( routesimulation )
ADD_SUBDIRECTORY( placemark )

# experimental implementation
# ADD_SUBDIRECTORY( geoclue )
