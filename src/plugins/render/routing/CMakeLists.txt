PROJECT(RoutingPlugin)

macro_optional_find_package(Phonon4Qt5 QUIET)
marble_set_package_properties( Phonon4Qt5 PROPERTIES
    DESCRIPTION "Qt-based audio library"
    URL "https://phonon.kde.org/"
    PURPOSE "Voice navigation (sound or human speakers)"
    TYPE OPTIONAL
)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

if(Phonon4Qt5_FOUND)
  set( routing_SRCS RoutingPlugin.cpp AudioOutput.cpp )
ELSE()
  set( routing_SRCS RoutingPlugin.cpp NullAudioOutput.cpp )
ENDIF()
set( routing_UI RoutingPlugin.ui RoutingConfigDialog.ui )
qt_wrap_ui( routing_SRCS ${routing_UI} )
qt_add_resources(routing_SRCS routing.qrc)

marble_add_plugin( RoutingPlugin ${routing_SRCS} )
if(Phonon4Qt5_FOUND)
    target_link_libraries(RoutingPlugin Phonon::phonon4qt5)
endif()
