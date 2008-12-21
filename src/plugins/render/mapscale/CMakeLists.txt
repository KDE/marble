PROJECT(MapScaleFloatItem)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/mapscale
 ${CMAKE_BINARY_DIR}/src/plugins/render/mapscale
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( mapscale_SRCS MapScaleFloatItem.cpp )

marble_add_plugin( MapScaleFloatItem ${mapscale_SRCS} )
