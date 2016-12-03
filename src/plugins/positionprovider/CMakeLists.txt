ADD_SUBDIRECTORY( flightgear )

macro_optional_find_package(libgps)
marble_set_package_properties( libgps PROPERTIES DESCRIPTION "communicating with the GPS daemon" )
marble_set_package_properties( libgps PROPERTIES URL "http://catb.org/gpsd/" )
marble_set_package_properties( libgps PROPERTIES TYPE OPTIONAL PURPOSE "position information via gpsd" )
IF(LIBGPS_FOUND)
     ADD_SUBDIRECTORY( gpsd )
ENDIF(LIBGPS_FOUND)

find_package(Qt5 ${REQUIRED_QT_VERSION}
    COMPONENTS
        Positioning
)
marble_set_package_properties( Qt5Positioning PROPERTIES DESCRIPTION "a collection of APIs and frameworks" )
marble_set_package_properties( Qt5Positioning PROPERTIES URL "http://qt.digia.com/" )
marble_set_package_properties( Qt5Positioning PROPERTIES TYPE OPTIONAL PURPOSE "position information via Qt5Positioning" )
if(Qt5Positioning_FOUND)
   ADD_SUBDIRECTORY( qtpositioning )
endif()


macro_optional_find_package(libwlocate)
marble_set_package_properties( libwlocate PROPERTIES DESCRIPTION "WLAN-based geolocation" )
marble_set_package_properties( libwlocate PROPERTIES URL "http://www.openwlanmap.org/" )
marble_set_package_properties( libwlocate PROPERTIES TYPE OPTIONAL PURPOSE "Position information based on neighboring WLAN networks" )
IF(LIBWLOCATE_FOUND)
     ADD_SUBDIRECTORY( wlocate )
ENDIF()

# experimental implementation
# ADD_SUBDIRECTORY( geoclue )
