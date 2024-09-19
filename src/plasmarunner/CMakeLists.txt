project(MarblePlasmaRunner)

macro_optional_find_package(KF6 ${REQUIRED_KF6_MIN_VERSION} QUIET COMPONENTS Runner I18n)

if(NOT KF6_FOUND)
  return()
endif()

set(plasmaRunner_SRCS
  main.cpp
  plasmarunner.cpp
)

add_library(plasma_runner_marble MODULE ${plasmaRunner_SRCS})
target_link_libraries(plasma_runner_marble
  marblewidget
  KF6::Runner
  KF6::I18n
)

install(TARGETS plasma_runner_marble DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5/krunner)
if(KF6Runner_VERSION VERSION_LESS 5.72.0)
  install(FILES plasma-runner-marble.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
else()
  kcoreaddons_desktop_to_json(plasma_runner_marble plasma-runner-marble.desktop)
endif()
