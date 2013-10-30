PROJECT( PlacemarkPositionProviderPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
if( QT4_FOUND )
  include(${QT_USE_FILE})
endif()

set( placemark_SRCS
 PlacemarkPositionProviderPlugin.cpp
)

marble_add_plugin( PlacemarkPositionProviderPlugin ${placemark_SRCS} )
