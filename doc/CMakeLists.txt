########### install files ###############
#

find_package(ECM ${REQUIRED_ECM_VERSION} QUIET)
if(NOT ECM_FOUND)
    return()
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})
include(KDEInstallDirs)

macro_optional_find_package(KF5 ${REQUIRED_KF5_VERSION} QUIET COMPONENTS DocTools)

if(KF5_FOUND)
    kdoctools_create_handbook(index.docbook INSTALL_DESTINATION ${KDE_INSTALL_DOCBUNDLEDIR}/en SUBDIR marble)
endif()
