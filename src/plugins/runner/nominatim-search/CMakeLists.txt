PROJECT( NominatimSearchPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set(nominatim_SRCS OsmNominatimSearchRunner.cpp NominatimSearchPlugin.cpp OsmNominatimSearchRunner.h NominatimSearchPlugin.h)

marble_add_plugin( NominatimSearchPlugin ${nominatim_SRCS})
