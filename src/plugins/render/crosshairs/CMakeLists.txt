PROJECT( CrosshairsPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
if( QT4_FOUND )
  INCLUDE(${QT_USE_FILE})
endif()

set( crosshairs_SRCS CrosshairsPlugin.cpp )
set( crosshairs_UI CrosshairsConfigWidget.ui )
qt_wrap_ui( crosshairs_SRCS ${crosshairs_UI} )
qt_add_resources( crosshairs_SRCS crosshairs.qrc )


marble_add_plugin( CrosshairsPlugin ${crosshairs_SRCS} )
