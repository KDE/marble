PROJECT( NominatimPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/runner/nominatim
 ${CMAKE_BINARY_DIR}/src/plugins/runner/nominatim
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( nominatim_SRCS OsmNominatimRunner.cpp NominatimPlugin.cpp )

marble_add_plugin( NominatimPlugin ${nominatim_SRCS} )
