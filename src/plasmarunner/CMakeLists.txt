project(MarblePlasmaRunner)

include_directories(
  ${KDE4_INCLUDES}
)

set(plasmaRunner_SRCS
  main.cpp
  plasmarunner.cpp
)

kde4_add_plugin(plasma_runner_marble ${plasmaRunner_SRCS})
target_link_libraries(plasma_runner_marble
  ${KDE4_PLASMA_LIBS}
  marblewidget
)

install(TARGETS plasma_runner_marble DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-runner-marble.desktop DESTINATION ${SERVICES_INSTALL_DIR})
