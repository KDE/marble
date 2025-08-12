PROJECT( KmlPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${CMAKE_SOURCE_DIR}/src/lib/marble/geodata/handlers/kml
)

set(kml_SRCS KmlParser.cpp KmlPlugin.cpp KmlRunner.cpp KmlParser.h KmlPlugin.h KmlRunner.h)

marble_add_plugin( KmlPlugin ${kml_SRCS})

macro_optional_find_package(KF6 ${REQUIRED_KF6_MIN_VERSION} QUIET COMPONENTS KIO)
if(NOT KF6_FOUND)
    return()
endif()

# register marble part as handler
# install again once part is fixed to load and show the passed file
# install(FILES marble_part_kml.desktop DESTINATION ${KDE_INSTALL_FULL_KSERVICESDIR})
# install(FILES marble_part_kmz.desktop DESTINATION ${KDE_INSTALL_FULL_KSERVICESDIR})

kcoreaddons_add_plugin(marble_thumbnail_kml SOURCES thumbnail_kml.cpp INSTALL_NAMESPACE "kf6/thumbcreator")
target_link_libraries(marble_thumbnail_kml PRIVATE marblethumbnail)

kcoreaddons_add_plugin(marble_thumbnail_kmz SOURCES thumbnail_kmz.cpp INSTALL_NAMESPACE "kf6/thumbcreator")
target_link_libraries(marble_thumbnail_kmz PRIVATE marblethumbnail)

if(NOT WIN32 AND NOT APPLE)
    # register marble app as handler
    install(FILES marble_kml.desktop DESTINATION ${KDE_INSTALL_FULL_APPDIR})
    install(FILES marble_kmz.desktop DESTINATION ${KDE_INSTALL_FULL_APPDIR})
endif()
