PROJECT( GpsdPositionProviderPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/positionprovider/gpsd
 ${CMAKE_BINARY_DIR}/src/plugins/positionprovider/gpsd
 ${QT_INCLUDE_DIR}
 ${LIBGPS_INCLUDE_DIR}
)

include(${QT_USE_FILE})

set( gpsd_SRCS
 GpsdConnection.cpp
 GpsdPositionProviderPlugin.cpp
 GpsdThread.cpp
)

set( GpsdPositionProviderPlugin_LIBS ${LIBGPS_LIBRARIES} )

marble_add_plugin( GpsdPositionProviderPlugin ${gpsd_SRCS} )
