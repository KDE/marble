PROJECT( NominatimSearchPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

INCLUDE_DIRECTORIES(
  ${Qt5WebKitWidgets_INCLUDE_DIRS}
  ${Qt5Xml_INCLUDE_DIRS}
)

set( nominatim_SRCS OsmNominatimSearchRunner.cpp NominatimSearchPlugin.cpp )

marble_add_plugin( NominatimSearchPlugin ${nominatim_SRCS} )
