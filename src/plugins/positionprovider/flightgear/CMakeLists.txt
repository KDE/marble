PROJECT( FlightGearPositionProviderPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
set( QT_USE_QTNETWORK  ON )
if( QT4_FOUND )
  INCLUDE(${QT_USE_FILE})
endif()

set( flightgear_SRCS FlightGearPositionProviderPlugin.cpp )
set( FlightGearPositionProviderPlugin_LIBS )

marble_add_plugin( FlightGearPositionProviderPlugin ${flightgear_SRCS} )
