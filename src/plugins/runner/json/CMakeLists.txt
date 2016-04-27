PROJECT( JsonPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( json_SRCS JsonRunner.cpp JsonPlugin.cpp JsonParser.cpp )

set( JsonPlugin_LIBS Qt5::Script )

marble_add_plugin( JsonPlugin ${json_SRCS} )

if(WIN32 OR APPLE)
  # nothing to do
else()
  install(FILES marble_geojson.desktop DESTINATION ${APPS_INSTALL_DIR})
endif()

set(KDE_SKIP_UNINSTALL_TARGET ON CACHE BOOL "KDE uninstall target must be disabled")
find_package(ECM QUIET)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})
if(ECM_FOUND)
  include(KDEInstallDirs)
endif()
macro_optional_find_package(KF5 QUIET COMPONENTS KIO)
if(KF5_FOUND)
  install(FILES marble_part_geojson.desktop DESTINATION ${SERVICES_INSTALL_DIR})
  install(FILES marble_thumbnail_geojson.desktop DESTINATION ${SERVICES_INSTALL_DIR})
endif()
