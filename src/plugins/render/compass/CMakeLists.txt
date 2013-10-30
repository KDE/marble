PROJECT(CompassFloatItem)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
if( QT4_FOUND )
  INCLUDE(${QT_USE_FILE})
endif()

set( compass_SRCS CompassFloatItem.cpp )
set( compass_UI CompassConfigWidget.ui )
qt_wrap_ui( compass_SRCS ${compass_UI} )
qt_add_resources( compass_SRCS compass.qrc )

marble_add_plugin( CompassFloatItem ${compass_SRCS} )
