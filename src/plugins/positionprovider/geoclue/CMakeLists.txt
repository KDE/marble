PROJECT( GeoCluePositionProviderPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set(geoclue_SRCS
 GeoCluePositionProviderPlugin.cpp
 GeoCute/Accuracy.cpp
 GeoCute/MasterClient.cpp
 GeoCute/PositionProvider.cpp
 GeoCute/Provider.cpp
 GeoCute/SimpleDBusInterface.cpp
 GeoCute/VelocityProvider.cpp
)

marble_add_plugin( GeoCluePositionProviderPlugin ${geoclue_SRCS})
