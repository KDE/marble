PROJECT( PlacemarkPositionProviderPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)

include(${QT_USE_FILE})

set( placemark_SRCS
 PlacemarkPositionProviderPlugin.cpp
)

marble_add_plugin( PlacemarkPositionProviderPlugin ${placemark_SRCS} )
