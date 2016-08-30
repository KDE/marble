find_package(ECM ${REQUIRED_ECM_VERSION} QUIET)
if(NOT ECM_FOUND)
    return()
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs)
# KDECMakeSettings expexts LIB_INSTALL_DIR set
set(LIB_INSTALL_DIR ${KDE_INSTALL_LIBDIR})
# Plasma macros assume SERVICES_INSTALL_DIR, DATA_INSTALL_DIR
set(SERVICES_INSTALL_DIR ${KDE_INSTALL_KSERVICES5DIR})
set(DATA_INSTALL_DIR ${KDE_INSTALL_DATADIR})
include(KDECMakeSettings)

macro_optional_find_package(KF5 ${REQUIRED_KF5_VERSION} QUIET COMPONENTS Plasma)

if (NOT KF5_FOUND)
  return()
endif()


add_subdirectory(qmlplugin)

add_subdirectory(applets)
add_subdirectory(wallpapers)
