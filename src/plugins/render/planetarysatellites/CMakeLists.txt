PROJECT(PlanetarySatellitesPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/planetarysatellites
 ${CMAKE_BINARY_DIR}/src/plugins/render/planetarysatellites
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( mex_SRCS
 mex/astrolib.cpp
 mex/attlib.cpp
 mex/planetarySats.cpp )

set( planetarysatellites_SRCS
 PlanetarySatellitesPlugin.cpp )

marble_add_plugin( PlanetarySatellitesPlugin
 ${planetarysatellites_SRCS} ${mex_SRCS} )
