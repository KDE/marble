PROJECT(MapScaleFloatItem)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( mapscale_SRCS MapScaleFloatItem.cpp )
set( mapscale_UI MapScaleConfigWidget.ui )

qt4_wrap_ui(mapscale_SRCS ${mapscale_UI})

marble_add_plugin( MapScaleFloatItem ${mapscale_SRCS} )
