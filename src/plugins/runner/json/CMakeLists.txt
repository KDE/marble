PROJECT( JsonPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set(json_SRCS JsonRunner.cpp JsonPlugin.cpp JsonParser.cpp JsonRunner.h JsonPlugin.h JsonParser.h)

marble_add_plugin( JsonPlugin ${json_SRCS})

macro_optional_find_package(KF6 ${REQUIRED_KF6_MIN_VERSION} QUIET COMPONENTS KIO)
if(NOT KF6_FOUND)
    return()
endif()

# register marble part as handler
# install again once part is fixed to load and show the passed file
# install(FILES marble_part_geojson.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})

if(NOT WIN32 AND NOT APPLE)
    # register marble app as handler
    install(FILES marble_geojson.desktop DESTINATION ${KDE_INSTALL_APPDIR})
endif()

