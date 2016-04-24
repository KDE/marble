PROJECT(CompassFloatItem)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( compass_SRCS CompassFloatItem.cpp )
set( compass_UI CompassConfigWidget.ui )
qt_wrap_ui( compass_SRCS ${compass_UI} )
qt_add_resources( compass_SRCS compass.qrc )

set( CompassFloatItem_LIBS Qt5::Svg )

marble_add_plugin( CompassFloatItem ${compass_SRCS} )
