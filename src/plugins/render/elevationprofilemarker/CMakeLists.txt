PROJECT( ElevationProfileMarker )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
if( QT4_FOUND )
  INCLUDE(${QT_USE_FILE})
endif()

set( my_SRCS
  ElevationProfileMarker.cpp
 )

qt_add_resources( my_SRCS elevationprofile.qrc)
marble_qt4_automoc( ${my_SRCS} )

marble_add_plugin( ElevationProfileMarker ${my_SRCS} )
