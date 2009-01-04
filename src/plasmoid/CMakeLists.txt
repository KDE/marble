include_directories(
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR}/../lib
   ${CMAKE_CURRENT_SOURCE_DIR}/../lib/Projections
   ${CMAKE_CURRENT_SOURCE_DIR}/../lib/geodata
   ${CMAKE_CURRENT_SOURCE_DIR}/../lib/geodata/data
   ${CMAKE_CURRENT_SOURCE_DIR}/../lib/geodata/parser
   ${CMAKE_CURRENT_SOURCE_DIR}/../lib/geodata/scene
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${KDE4_INCLUDES}
   )

set(worldclock_SRCS worldclock.cpp )

qt4_add_resources(worldclock_SRCS ../marble.qrc)

kde4_add_ui_files( worldclock_SRCS worldclockConfig.ui )

kde4_add_plugin(plasma_applet_worldclock ${worldclock_SRCS})
target_link_libraries(plasma_applet_worldclock
                      ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_PLASMA_LIBS}
                      marblewidget )

install(TARGETS plasma_applet_worldclock
        DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES plasma-applet-kworldclock.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
