macro_optional_find_package(KF6 ${REQUIRED_KF6_MIN_VERSION} QUIET COMPONENTS Crash Parts CoreAddons I18n)

if(NOT KF6_FOUND)
    return()
endif()

set(marble_kde_SRCS 
  kdemain.cpp 
  KdeMainWindow.cpp 
  marble_part.cpp
  KdeMainWindow.h
  marble_part.h
)

set(marblepart_SRCS marble_part.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../marble-ui)
kconfig_add_kcfg_files(marblepart_SRCS settings.kcfgc)
add_library(marble_part ${marblepart_SRCS})
set_target_properties(marble_part PROPERTIES PREFIX "${CMAKE_SHARED_LIBRARY_PREFIX}")
target_link_libraries(marble_part
    marbleui
    marblewidget
    KF6::Parts
    KF6::CoreAddons
    KF6::I18n
)

install(TARGETS marble_part DESTINATION ${KDE_INSTALL_PLUGINDIR})
install(FILES marble_part.rc marbleui.rc DESTINATION ${KDE_INSTALL_KXMLGUIDIR}/marble)

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
    KF6::Parts
    KF6::Crash
    KF6::CoreAddons
    KF6::I18n
    )

## this works for win32 only because FindQt does not handle this correct
if(STATIC_BUILD AND WIN32)
    target_link_libraries(marble ${QT_PLUGINS_DIR}/imageformats/qjpeg.lib)
    target_link_libraries(marble ${QT_PLUGINS_DIR}/imageformats/qsvg.lib)
endif()

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

macro_optional_find_package(KF6 ${REQUIRED_KF6_MIN_VERSION} QUIET COMPONENTS DocTools)

if (KF6DocTools_FOUND)
    # Disabled for now
    kdoctools_install(${CMAKE_SOURCE_DIR}/po)
endif()
