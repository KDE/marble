PROJECT(SatellitesPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/satellites
 ${CMAKE_BINARY_DIR}/src/plugins/render/satellites
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( sgp4_SRCS
 sgp4/sgp4ext.cpp
 sgp4/sgp4unit.cpp
 sgp4/sgp4io.cpp )

set( mex_SRCS
 mex/astrolib.cpp
 mex/attlib.cpp
 mex/planetarySats.cpp )

set( satellites_SRCS
 TrackerPluginModel.cpp
 TrackerPluginItem.cpp

 SatellitesPlugin.cpp
 SatellitesModel.cpp
 SatellitesMSCItem.cpp
 SatellitesTLEItem.cpp
 SatellitesConfigModel.cpp
 SatellitesConfigDialog.cpp
 SatellitesConfigAbstractItem.cpp
 SatellitesConfigNodeItem.cpp
 SatellitesConfigLeafItem.cpp)

qt4_wrap_ui( satellites_SRCS SatellitesConfigDialog.ui )

qt4_add_resources( satellites_SRCS satellites.qrc )

marble_add_plugin( SatellitesPlugin
 ${satellites_SRCS}
 ${sgp4_SRCS}
 ${mex_SRCS} )

