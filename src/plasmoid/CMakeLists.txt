project(worldclock)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
${CMAKE_SOURCE_DIR}/ )
find_package(KDE4 REQUIRED)
include(KDE4Defaults)
find_package(Plasma REQUIRED)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
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
kde4_add_ui_files( worldclock_SRCS worldclockConfig.ui )
 
kde4_add_plugin(plasma_applet_worldclock ${worldclock_SRCS})
target_link_libraries(plasma_applet_worldclock
                      ${PLASMA_LIBS} ${KDE4_KDEUI_LIBS}
                      marblewidget )
 
install(TARGETS plasma_applet_worldclock
        DESTINATION ${PLUGIN_INSTALL_DIR})
 
install(FILES plasma-applet-kworldclock.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
