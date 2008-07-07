PROJECT(Panoramio)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/marble/panoramio
 ${CMAKE_BINARY_DIR}/src/plugins/marble/panoramio
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( panoramio_SRCS PanoramioPlugin.cpp jsonparser.cpp )

marble_add_plugin( Panoramio ${panoramio_SRCS} )