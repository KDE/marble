PROJECT(SatellitesPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/satellites
 ${CMAKE_BINARY_DIR}/src/plugins/render/satellites
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( satellites_SRCS
 SatellitesPlugin.cpp
 SatellitesModel.cpp
 SatellitesItem.cpp )
set( sgp4_SRCS
  sgp4/sgp4ext.cpp
  sgp4/sgp4unit.cpp
  sgp4/sgp4io.cpp )

marble_add_plugin( SatellitesPlugin ${satellites_SRCS} ${sgp4_SRCS} )
