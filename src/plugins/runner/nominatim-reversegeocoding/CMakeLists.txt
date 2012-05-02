PROJECT( NominatimReverseGeocodingPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( nominatim_SRCS OsmNominatimReverseGeocodingRunner.cpp NominatimReverseGeocodingPlugin.cpp )

marble_add_plugin( NominatimReverseGeocodingPlugin ${nominatim_SRCS} )
