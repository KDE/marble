find_package(ECM ${REQUIRED_ECM_VERSION} QUIET)
if(NOT ECM_FOUND)
    return()
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs)
# KDECMakeSettings expexts LIB_INSTALL_DIR set
set(LIB_INSTALL_DIR ${KDE_INSTALL_LIBDIR})
include(KDECMakeSettings)
include(ECMInstallIcons)

macro_optional_find_package(KF5 ${REQUIRED_KF5_VERSION} QUIET COMPONENTS Crash NewStuff Parts CoreAddons I18n)

if(NOT KF5_FOUND)
    return()
endif()

set(marble_kde_SRCS 
  kdemain.cpp 
  KdeMainWindow.cpp 
  marble_part.cpp
)

set(marblepart_SRCS marble_part.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../marble-ui)
kconfig_add_kcfg_files(marblepart_SRCS settings.kcfgc)
add_library(marble_part ${marblepart_SRCS})
set_target_properties(marble_part PROPERTIES PREFIX "${CMAKE_SHARED_LIBRARY_PREFIX}")
target_link_libraries(marble_part
    marbleui
    marblewidget
    KF5::NewStuff
    KF5::Parts
    KF5::CoreAddons
    KF5::I18n
)

install(TARGETS marble_part DESTINATION ${KDE_INSTALL_PLUGINDIR})
install(FILES marble_part.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
install(FILES marble_part.rc marbleui.rc DESTINATION ${KDE_INSTALL_KXMLGUI5DIR}/marble)
install(FILES marble.knsrc DESTINATION ${KDE_INSTALL_CONFDIR})

ki18n_wrap_ui(marble_kde_SRCS ${marble_UI})
kconfig_add_kcfg_files(marble_kde_SRCS settings.kcfgc)

ecm_install_icons(ICONS
    "${CMAKE_SOURCE_DIR}/data/icons/128-apps-marble.png"
    "${CMAKE_SOURCE_DIR}/data/icons/16-apps-marble.png"
    "${CMAKE_SOURCE_DIR}/data/icons/22-apps-marble.png"
    "${CMAKE_SOURCE_DIR}/data/icons/32-apps-marble.png"
    "${CMAKE_SOURCE_DIR}/data/icons/48-apps-marble.png"
    "${CMAKE_SOURCE_DIR}/data/icons/64-apps-marble.png"
    DESTINATION ${KDE_INSTALL_ICONDIR})
add_executable (marble ${marble_kde_SRCS})

target_link_libraries (
    marble
    marbleui
    marblewidget
    astro
    KF5::NewStuff
    KF5::Parts
    KF5::Crash
    KF5::CoreAddons
    KF5::I18n
    )

## this works for win32 only because FindQt does not handle this correct
if(STATIC_BUILD)
    target_link_libraries(marble ${QT_PLUGINS_DIR}/imageformats/qjpeg.lib)
    target_link_libraries(marble ${QT_PLUGINS_DIR}/imageformats/qsvg.lib)
endif(STATIC_BUILD)

install(TARGETS marble ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS org.kde.marble.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.marble.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES marble.kcfg DESTINATION ${KDE_INSTALL_KCFGDIR})
# register marble app as schemehandler
install(FILES marble_geo.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES marble_worldwind.desktop DESTINATION ${KDE_INSTALL_APPDIR})

# Handle mo-targetted translation catalogs and translated user documentation
# as added for release tarballs.
# See toplevel CMakeLists.txt for further explanation why it is done here
ki18n_install(${CMAKE_SOURCE_DIR}/po)

macro_optional_find_package(KF5 ${REQUIRED_KF5_VERSION} QUIET COMPONENTS DocTools)

if (KF5DocTools_FOUND)
    kdoctools_install(${CMAKE_SOURCE_DIR}/po)
endif()
