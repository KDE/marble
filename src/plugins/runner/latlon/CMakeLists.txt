PROJECT( LatLonPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( latlon_SRCS LatLonRunner.cpp LatLonPlugin.cpp )

marble_add_plugin( LatLonPlugin ${latlon_SRCS} )
