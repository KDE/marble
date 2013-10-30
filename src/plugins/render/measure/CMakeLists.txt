PROJECT( MeasureTool )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
if( QT4_FOUND )
  INCLUDE(${QT_USE_FILE})
endif()

set( measuretool_SRCS MeasureToolPlugin.cpp )
set( measuretool_UI MeasureConfigWidget.ui )

qt_wrap_ui(measuretool_SRCS ${measuretool_UI} )
qt_add_resources( measuretool_SRCS measuretool.qrc )
marble_add_plugin( MeasureTool ${measuretool_SRCS} )
