ADD_SUBDIRECTORY( flightgear)

macro_optional_find_package(libgps)
marble_set_package_properties( libgps PROPERTIES DESCRIPTION "communicating with the GPS daemon")
marble_set_package_properties( libgps PROPERTIES URL "https://gpsd.gitlab.io/gpsd/index.html")
marble_set_package_properties( libgps PROPERTIES TYPE OPTIONAL PURPOSE "position information via gpsd")
IF(LIBGPS_FOUND)
     ADD_SUBDIRECTORY( gpsd)
ENDIF(LIBGPS_FOUND)

find_package(Qt6 ${REQUIRED_QT_VERSION}
    OPTIONAL_COMPONENTS
        Positioning
)
marble_set_package_properties( Qt6Positioning PROPERTIES DESCRIPTION "a collection of APIs and frameworks")
marble_set_package_properties( Qt6Positioning PROPERTIES URL "https://www.qt.io/developers/")
marble_set_package_properties( Qt6Positioning PROPERTIES TYPE OPTIONAL PURPOSE "position information via Qt6Positioning")
if(Qt6Positioning_FOUND)
   ADD_SUBDIRECTORY( qtpositioning)
endif()


macro_optional_find_package(libwlocate)
marble_set_package_properties( libwlocate PROPERTIES DESCRIPTION "WLAN-based geolocation")
marble_set_package_properties( libwlocate PROPERTIES URL "https://sourceforge.net/p/libwlocate/wiki/Home/")
marble_set_package_properties( libwlocate PROPERTIES TYPE OPTIONAL PURPOSE "Position information based on neighboring WLAN networks")
IF(LIBWLOCATE_FOUND)
     ADD_SUBDIRECTORY( wlocate)
ENDIF()

# experimental implementation
# ADD_SUBDIRECTORY( geoclue)
