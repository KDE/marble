PROJECT(RoutingPlugin)

FIND_PACKAGE(Phonon)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/routing
 ${CMAKE_BINARY_DIR}/src/plugins/render/routing
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

IF( PHONON_FOUND )
  set( routing_SRCS RoutingPlugin.cpp AudioOutput.cpp )
ELSE()
  set( routing_SRCS RoutingPlugin.cpp NullAudioOutput.cpp )
ENDIF()
set( routing_UI RoutingPlugin.ui RoutingConfigDialog.ui )
qt4_wrap_ui( routing_SRCS ${routing_UI} )
qt4_add_resources(routing_SRCS routing.qrc)

marble_add_plugin( RoutingPlugin ${routing_SRCS} )
target_link_libraries(RoutingPlugin ${PHONON_LIBS})
