project(MarblePlasmaRunner)

set(KDE_SKIP_UNINSTALL_TARGET ON CACHE BOOL "KDE uninstall target must be disabled")
find_package(ECM QUIET)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})
if(ECM_FOUND)
  include(KDEInstallDirs)
  include(KDECMakeSettings)
endif()
include(FeatureSummary)
macro_optional_find_package(KF5 QUIET COMPONENTS Config Runner Service I18n)

if (NOT KF5_FOUND)       
  return()
endif()
  
set(plasmaRunner_SRCS
  main.cpp
  plasmarunner.cpp
)

add_library(plasma_runner_marble MODULE ${plasmaRunner_SRCS})
target_link_libraries(plasma_runner_marble
  ${MARBLEWIDGET}
  KF5::ConfigCore
  KF5::Runner
  KF5::Service
  KF5::I18n
)

install(TARGETS plasma_runner_marble DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-runner-marble.desktop DESTINATION ${SERVICES_INSTALL_DIR})
