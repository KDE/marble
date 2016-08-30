project(MarblePlasmaRunner)

find_package(ECM ${REQUIRED_ECM_VERSION} QUIET)
if(NOT ECM_FOUND)
    return()
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(ECMPoQmTools)
include(KDEInstallDirs)
# KDECMakeSettings expexts LIB_INSTALL_DIR set
set(LIB_INSTALL_DIR ${KDE_INSTALL_LIBDIR})
include(KDECMakeSettings)

macro_optional_find_package(KF5 ${REQUIRED_KF5_VERSION} QUIET COMPONENTS Runner I18n)

if(NOT KF5_FOUND)
  return()
endif()

set(plasmaRunner_SRCS
  main.cpp
  plasmarunner.cpp
)

# we have to load the catalog ourselves currently
ecm_create_qm_loader(plasmaRunner_SRCS marble_qt)

add_library(plasma_runner_marble MODULE ${plasmaRunner_SRCS})
target_link_libraries(plasma_runner_marble
  marblewidget
  KF5::Runner
  KF5::I18n
)

install(TARGETS plasma_runner_marble DESTINATION ${KDE_INSTALL_PLUGINDIR})
install(FILES plasma-runner-marble.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
