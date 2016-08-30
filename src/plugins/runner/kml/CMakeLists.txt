PROJECT( KmlPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${CMAKE_SOURCE_DIR}/src/lib/marble/geodata/handlers/kml
)

set( kml_SRCS KmlDocument.cpp KmlParser.cpp KmlPlugin.cpp KmlRunner.cpp KmzHandler.cpp )

marble_add_plugin( KmlPlugin ${kml_SRCS} )


find_package(ECM ${REQUIRED_ECM_VERSION} QUIET)
if(NOT ECM_FOUND)
    return()
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs)

macro_optional_find_package(KF5 ${REQUIRED_KF5_VERSION} QUIET COMPONENTS KIO)
if(NOT KF5_FOUND)
    return()
endif()

# register marble part as handler
# install again once part is fixed to load and show the passed file
# install(FILES marble_part_kml.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
# install(FILES marble_part_kmz.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})

# register thumbnail plugin as handler
install(FILES marble_thumbnail_kml.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
install(FILES marble_thumbnail_kmz.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})

if(NOT WIN32 AND NOT APPLE)
    # register marble app as handler
    install(FILES marble_kml.desktop DESTINATION ${KDE_INSTALL_APPDIR})
    install(FILES marble_kmz.desktop DESTINATION ${KDE_INSTALL_APPDIR})
endif()
