PROJECT( PlacemarkPositionProviderPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/positionprovider/placemark
 ${CMAKE_BINARY_DIR}/src/plugins/positionprovider/placemark
 ${QT_INCLUDE_DIR}
)

include(${QT_USE_FILE})

set( placemark_SRCS
 PlacemarkPositionProviderPlugin.cpp
)

marble_add_plugin( PlacemarkPositionProviderPlugin ${placemark_SRCS} )
