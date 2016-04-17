PROJECT( LatLonPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( latlon_SRCS LatLonRunner.cpp LatLonPlugin.cpp )

marble_add_plugin( LatLonPlugin ${latlon_SRCS} )
