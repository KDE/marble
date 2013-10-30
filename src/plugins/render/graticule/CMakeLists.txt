PROJECT(GraticulePlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
if( QT4_FOUND )
  INCLUDE(${QT_USE_FILE})
endif()

set( graticule_SRCS GraticulePlugin.cpp )
set( graticule_UI GraticuleConfigWidget.ui )
qt_wrap_ui( graticule_SRCS ${graticule_UI} )


marble_add_plugin( GraticulePlugin ${graticule_SRCS} )
