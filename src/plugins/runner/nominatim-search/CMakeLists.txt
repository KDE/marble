PROJECT( NominatimSearchPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( nominatim_SRCS OsmNominatimSearchRunner.cpp NominatimSearchPlugin.cpp )

marble_add_plugin( NominatimSearchPlugin ${nominatim_SRCS} )
