PROJECT( CrosshairsPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set(crosshairs_SRCS CrosshairsPlugin.cpp CrosshairsPlugin.h)
set(crosshairs_UI CrosshairsConfigWidget.ui)
qt_wrap_ui( crosshairs_SRCS ${crosshairs_UI})
qt_add_resources( crosshairs_SRCS crosshairs.qrc)

set(CrosshairsPlugin_LIBS Qt6::Svg)

marble_add_plugin( CrosshairsPlugin ${crosshairs_SRCS})
