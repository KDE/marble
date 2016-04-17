PROJECT( FlightGearPositionProviderPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( flightgear_SRCS FlightGearPositionProviderPlugin.cpp )
set( FlightGearPositionProviderPlugin_LIBS )

marble_add_plugin( FlightGearPositionProviderPlugin ${flightgear_SRCS} )
