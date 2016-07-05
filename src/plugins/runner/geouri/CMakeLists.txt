PROJECT( GeoUriPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( geouri_SRCS GeoUriRunner.cpp GeoUriPlugin.cpp )

marble_add_plugin( GeoUriPlugin ${geouri_SRCS} )
