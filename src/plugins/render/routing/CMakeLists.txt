PROJECT(RoutingPlugin)

FIND_PACKAGE(Phonon)
marble_set_package_properties( Phonon PROPERTIES DESCRIPTION "cross-platform multimedia framework that enables the use of audio and video content" )
marble_set_package_properties( Phonon PROPERTIES URL "http://qt.digia.com/" )
marble_set_package_properties( Phonon PROPERTIES TYPE OPTIONAL PURPOSE "Voice navigation (sound or human speakers)" )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
 ${PHONON_INCLUDES}
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
