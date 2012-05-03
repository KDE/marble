PROJECT( FlightGearPositionProviderPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/positionprovider/flightgear
 ${CMAKE_BINARY_DIR}/src/plugins/positionprovider/flightgear
 ${QT_INCLUDE_DIR}
)
set( QT_USE_QTNETWORK  ON )
INCLUDE(${QT_USE_FILE})

set( flightgear_SRCS FlightGearPositionProviderPlugin.cpp )
set( FlightGearPositionProviderPlugin_LIBS ${NMEALIB_LIBRARIES} )

marble_add_plugin( FlightGearPositionProviderPlugin ${flightgear_SRCS} )
