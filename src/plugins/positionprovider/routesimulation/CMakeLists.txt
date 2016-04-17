PROJECT( RouteSimulationPositionProviderPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( the_SRCS
 RouteSimulationPositionProviderPlugin.cpp
)

marble_add_plugin( RouteSimulationPositionProviderPlugin ${the_SRCS} )
