PROJECT(GraticulePlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set(graticule_SRCS GraticulePlugin.cpp GraticulePlugin.h)
set(graticule_UI GraticuleConfigWidget.ui)
qt_wrap_ui( graticule_SRCS ${graticule_UI})

set(GraticulePlugin_LIBS Qt6::Svg)

marble_add_plugin( GraticulePlugin ${graticule_SRCS})
