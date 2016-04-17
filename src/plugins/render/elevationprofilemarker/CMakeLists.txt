PROJECT( ElevationProfileMarker )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( my_SRCS
  ElevationProfileMarker.cpp
 )

qt_add_resources( my_SRCS elevationprofile.qrc)

marble_add_plugin( ElevationProfileMarker ${my_SRCS} )
