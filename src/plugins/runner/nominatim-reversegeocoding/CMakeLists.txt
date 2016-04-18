PROJECT( NominatimReverseGeocodingPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( nominatim_SRCS OsmNominatimReverseGeocodingRunner.cpp NominatimReverseGeocodingPlugin.cpp )

marble_add_plugin( NominatimReverseGeocodingPlugin ${nominatim_SRCS} )
