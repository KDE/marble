PROJECT( ShpPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
 ${LIBSHP_INCLUDE_DIR}
)

if( QT4BUILD )
  INCLUDE(${QT_USE_FILE})
endif()

set( shp_SRCS ShpPlugin.cpp ShpRunner.cpp )

set( ShpPlugin_LIBS ${LIBSHP_LIBRARIES} )

marble_add_plugin( ShpPlugin ${shp_SRCS} )

if(WIN32 OR APPLE)
  # nothing to do
else()
  install(FILES marble_shp.desktop DESTINATION ${APPS_INSTALL_DIR})
endif()

set(KDE_SKIP_UNINSTALL_TARGET ON CACHE BOOL "KDE uninstall target must be disabled")
find_package(ECM QUIET)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})
if(ECM_FOUND)
  include(KDEInstallDirs)
endif()
macro_optional_find_package(KF5 QUIET COMPONENTS KIO)
if(KF5_FOUND)
  install(FILES marble_part_shp.desktop DESTINATION ${SERVICES_INSTALL_DIR})
  install(FILES marble_thumbnail_shp.desktop DESTINATION ${SERVICES_INSTALL_DIR})
endif()
