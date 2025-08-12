PROJECT( ShpPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${LIBSHP_INCLUDE_DIR}
)

set(shp_SRCS ShpPlugin.cpp ShpRunner.cpp)

set(ShpPlugin_LIBS ${LIBSHP_LIBRARIES})

marble_add_plugin( ShpPlugin ${shp_SRCS})

macro_optional_find_package(KF6 ${REQUIRED_KF6_MIN_VERSION} QUIET COMPONENTS KIO)
if(NOT KF6_FOUND)
    return()
endif()

# register marble part as handler
# install again once part is fixed to load and show the passed file
# install(FILES marble_part_shp.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})

kcoreaddons_add_plugin(marble_thumbnail_shp SOURCES thumbnail.cpp INSTALL_NAMESPACE "kf6/thumbcreator")
target_link_libraries(marble_thumbnail_shp PRIVATE marblethumbnail)

if(NOT WIN32 AND NOT APPLE)
    # register marble app as handler
    install(FILES marble_shp.desktop DESTINATION ${KDE_INSTALL_APPDIR})
endif()
