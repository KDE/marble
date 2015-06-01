set(KDE_SKIP_UNINSTALL_TARGET ON CACHE BOOL "KDE uninstall target must be disabled")
find_package(ECM QUIET)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})
if(ECM_FOUND)
  include(KDEInstallDirs)
endif()
macro_optional_find_package(KF5 QUIET COMPONENTS Config Plasma Service)

if (NOT KF5_FOUND)
  return()
endif()

include_directories(
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR}/../lib
   ${CMAKE_CURRENT_SOURCE_DIR}/../lib/projections
   ${CMAKE_CURRENT_SOURCE_DIR}/../lib/geodata
   ${CMAKE_CURRENT_SOURCE_DIR}/../lib/geodata/data
   ${CMAKE_CURRENT_SOURCE_DIR}/../lib/geodata/parser
   ${CMAKE_CURRENT_SOURCE_DIR}/../lib/geodata/scene
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_CURRENT_BINARY_DIR}
   ${CMAKE_BINARY_DIR}
   ${KDE4_INCLUDES}
   )

set(worldclock_SRCS worldclock.cpp )

qt_add_resources(worldclock_SRCS ../apps/marble-ui/marble.qrc)

qt_wrap_ui( worldclock_SRCS worldclockConfig.ui )

add_library(plasma_applet_worldclock MODULE ${worldclock_SRCS})
target_link_libraries(plasma_applet_worldclock
                      KF5::ConfigCore
                      KF5::Plasma
                      KF5::Service
                      ${MARBLEWIDGET} )

install(TARGETS plasma_applet_worldclock
        DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES plasma-applet-kworldclock.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
