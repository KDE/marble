PROJECT(PlanetarySatellitesPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/planetarysatellites
 ${CMAKE_BINARY_DIR}/src/plugins/render/planetarysatellites
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( planetarysatellites_SRCS
 PlanetarySatellitesPlugin.cpp )

marble_add_plugin( PlanetarySatellitesPlugin ${planetarysatellites_SRCS} )
