PROJECT( MeasureTool )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( measuretool_SRCS MeasureToolPlugin.cpp )
set( measuretool_UI MeasureConfigWidget.ui )

qt4_wrap_ui(measuretool_SRCS ${measuretool_UI} )
qt4_add_resources( measuretool_SRCS measuretool.qrc )
marble_add_plugin( MeasureTool ${measuretool_SRCS} )
