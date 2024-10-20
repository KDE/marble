project(MarblePlasmaRunner)

macro_optional_find_package(KF6 ${KF_MIN_VERSION} QUIET COMPONENTS Runner I18n)

if(NOT KF6_FOUND)
  return()
endif()

add_definitions(-DQT_NO_CAST_FROM_ASCII)

kcoreaddons_add_plugin(plasma_runner_marble INSTALL_NAMESPACE "kf6/krunner")

target_sources(plasma_runner_marble PRIVATE plasmarunner.cpp plasmarunner.h)

target_link_libraries(plasma_runner_marble
  marblewidget
  KF6::Runner
  KF6::I18n
)
