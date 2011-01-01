PROJECT(CompassFloatItem)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/compass
 ${CMAKE_BINARY_DIR}/src/plugins/render/compass
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( compass_SRCS CompassFloatItem.cpp )
set( compass_UI CompassConfigWidget.ui )
qt4_wrap_ui( compass_SRCS ${compass_UI} )
qt4_add_resources( compass_SRCS compass.qrc )

marble_add_plugin( CompassFloatItem ${compass_SRCS} )
