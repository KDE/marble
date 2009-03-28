PROJECT(GraticulePlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/graticule
 ${CMAKE_BINARY_DIR}/src/plugins/render/graticule
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( marblegraticule_SRCS GraticulePlugin.cpp )

marble_add_plugin( GraticulePlugin ${marblegraticule_SRCS} )
