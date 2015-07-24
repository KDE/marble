set(KDE_SKIP_UNINSTALL_TARGET ON CACHE BOOL "KDE uninstall target must be disabled")
find_package(ECM QUIET)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})
if(ECM_FOUND)
  include(KDEInstallDirs)
  include(KDECMakeSettings)
  include(ECMInstallIcons)
endif()
macro_optional_find_package(KF5 QUIET COMPONENTS Wallet NewStuff Parts CoreAddons I18n)

if (NOT KF5_FOUND)       
  return()
endif()

set(marble_kde_SRCS 
  kdemain.cpp 
  KdeMainWindow.cpp 
  marble_part.cpp
)

set(marblepart_SRCS marble_part.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../marble-ui ${KDE4_INCLUDES})
kconfig_add_kcfg_files(marblepart_SRCS settings.kcfgc)
add_library(marble_part ${marblepart_SRCS})
set_target_properties(marble_part PROPERTIES PREFIX "${CMAKE_SHARED_LIBRARY_PREFIX}")
target_link_libraries(marble_part marbleui ${MARBLEWIDGET}
    KF5::NewStuff
    KF5::Wallet
    KF5::Parts
    KF5::CoreAddons
    KF5::I18n
)

install(TARGETS marble_part DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES marble_part.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES marble_geo.desktop DESTINATION ${APPS_INSTALL_DIR})
install(FILES marble_worldwind.desktop DESTINATION ${APPS_INSTALL_DIR})
install(FILES marble_part.rc marbleui.rc marble.knsrc DESTINATION ${KXMLGUI_INSTALL_DIR}/marble)

ki18n_wrap_ui(marble_kde_SRCS ${marble_UI})
kconfig_add_kcfg_files(marble_kde_SRCS settings.kcfgc)

ecm_install_icons(ICONS
  "${CMAKE_CURRENT_SOURCE_DIR}/../../../data/icons/128-apps-marble.png"
  "${CMAKE_CURRENT_SOURCE_DIR}/../../../data/icons/16-apps-marble.png"
  "${CMAKE_CURRENT_SOURCE_DIR}/../../../data/icons/22-apps-marble.png"
  "${CMAKE_CURRENT_SOURCE_DIR}/../../../data/icons/32-apps-marble.png"
  "${CMAKE_CURRENT_SOURCE_DIR}/../../../data/icons/48-apps-marble.png"
  "${CMAKE_CURRENT_SOURCE_DIR}/../../../data/icons/64-apps-marble.png"
  DESTINATION ${ICON_INSTALL_DIR})
add_executable (marble ${marble_kde_SRCS})

target_link_libraries (
    marble
    marbleui
    ${MARBLEWIDGET}
    astro
    KF5::NewStuff
    KF5::Wallet
    KF5::Parts
    KF5::CoreAddons
    KF5::I18n
    )

## this works for win32 only because FindQt4 does not handle this correct
if(STATIC_BUILD)
    target_link_libraries(marble ${QT_PLUGINS_DIR}/imageformats/qjpeg.lib)
    target_link_libraries(marble ${QT_PLUGINS_DIR}/imageformats/qsvg.lib)
endif(STATIC_BUILD)

if (NOT APPLE)
    install( FILES marble.desktop DESTINATION ${APPS_INSTALL_DIR})
endif()

install(TARGETS marble ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS marble.desktop DESTINATION ${APPS_INSTALL_DIR})
install(FILES marble.kcfg DESTINATION ${KCFG_INSTALL_DIR})
