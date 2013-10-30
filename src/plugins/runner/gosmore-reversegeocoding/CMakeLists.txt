PROJECT( GosmoreReverseGeocodingPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
if( QT4_FOUND )
  INCLUDE(${QT_USE_FILE})
endif()

set( gosmore_SRCS GosmoreReverseGeocodingRunner.cpp GosmoreReverseGeocodingPlugin.cpp )

marble_add_plugin( GosmoreReverseGeocodingPlugin ${gosmore_SRCS} )
