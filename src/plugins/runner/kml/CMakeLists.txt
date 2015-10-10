PROJECT( KmlPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${CMAKE_SOURCE_DIR}/src/lib/marble/geodata/handlers/kml
 ${QT_INCLUDE_DIR}
)
if( QT4BUILD )
  INCLUDE(${QT_USE_FILE})
endif()

set( kml_SRCS KmlDocument.cpp KmlParser.cpp KmlPlugin.cpp KmlRunner.cpp KmzHandler.cpp )

marble_add_plugin( KmlPlugin ${kml_SRCS} )

if(WIN32 OR APPLE)
  # nothing to do
else()
  install(FILES marble_kml.desktop DESTINATION ${APPS_INSTALL_DIR})
  install(FILES marble_kmz.desktop DESTINATION ${APPS_INSTALL_DIR})
endif()

set(KDE_SKIP_UNINSTALL_TARGET ON CACHE BOOL "KDE uninstall target must be disabled")
find_package(ECM QUIET)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})
if(ECM_FOUND)
  include(KDEInstallDirs)
endif()
macro_optional_find_package(KF5 QUIET COMPONENTS KIO)
if(KF5_FOUND)
  install(FILES marble_part_kml.desktop DESTINATION ${SERVICES_INSTALL_DIR})
  install(FILES marble_thumbnail_kml.desktop DESTINATION ${SERVICES_INSTALL_DIR})
  install(PROGRAMS marble_kmz.desktop DESTINATION ${APPS_INSTALL_DIR})
  install(FILES marble_part_kmz.desktop DESTINATION ${SERVICES_INSTALL_DIR})
  install(FILES marble_thumbnail_kmz.desktop DESTINATION ${SERVICES_INSTALL_DIR})
endif()
