PROJECT( RouteSimulationPositionProviderPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/positionprovider/routesimulation
 ${CMAKE_BINARY_DIR}/src/plugins/positionprovider/routesimulation
 ${QT_INCLUDE_DIR}
)

include(${QT_USE_FILE})

set( the_SRCS
 RouteSimulationPositionProviderPlugin.cpp
)

marble_add_plugin( RouteSimulationPositionProviderPlugin ${the_SRCS} )
