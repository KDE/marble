PROJECT( GpxPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/runner/gpx
 ${CMAKE_BINARY_DIR}/src/plugins/runner/gpx
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( gpx_SRCS GpxPlugin.cpp GpxRunner.cpp )

marble_add_plugin( GpxPlugin ${gpx_SRCS} )
