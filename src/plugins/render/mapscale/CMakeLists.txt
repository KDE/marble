PROJECT(MapScaleFloatItem)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
if( QT4_FOUND )
  INCLUDE(${QT_USE_FILE})
endif()

set( mapscale_SRCS MapScaleFloatItem.cpp )
set( mapscale_UI MapScaleConfigWidget.ui )

qt_wrap_ui(mapscale_SRCS ${mapscale_UI})

marble_add_plugin( MapScaleFloatItem ${mapscale_SRCS} )
