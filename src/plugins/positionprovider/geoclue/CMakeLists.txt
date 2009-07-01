PROJECT( GeoCluePositionProviderPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/positionprovider/geoclue
 ${CMAKE_BINARY_DIR}/src/plugins/positionprovider/geoclue
 ${QT_INCLUDE_DIR}
)

include(${QT_USE_FILE})

set( geoclue_SRCS
 GeoCluePositionProviderPlugin.cpp
 GeoCute/Accuracy.cpp
 GeoCute/MasterClient.cpp
 GeoCute/PositionProvider.cpp
 GeoCute/Provider.cpp
 GeoCute/SimpleDBusInterface.cpp
 GeoCute/VelocityProvider.cpp
)

marble_add_plugin( GeoCluePositionProviderPlugin ${geoclue_SRCS} )
