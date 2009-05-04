PROJECT( GpsdPositionProviderPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/positionprovider/gpsd
 ${CMAKE_BINARY_DIR}/src/plugins/positionprovider/gpsd
 ${QT_INCLUDE_DIR}
 ${libgps_INCLUDE_DIRS}
)

include(${QT_USE_FILE})

set( gpsd_SRCS GpsdPositionProviderPlugin.cpp )

set( GpsdPositionProviderPlugin_LIBS ${libgps_LIBRARIES} )

marble_add_plugin( GpsdPositionProviderPlugin ${gpsd_SRCS} )
