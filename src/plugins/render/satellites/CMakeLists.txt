PROJECT(SatellitesPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/satellites
 ${CMAKE_BINARY_DIR}/src/plugins/render/satellites
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( satellites_SRCS SatellitesPlugin.cpp )

marble_add_plugin( SatellitesPlugin ${satellites_SRCS} )
