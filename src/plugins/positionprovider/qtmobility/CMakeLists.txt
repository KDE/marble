PROJECT( QtMobilityPositionProviderPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/positionprovider/qtmobility
 ${CMAKE_BINARY_DIR}/src/plugins/positionprovider/qtmobility
 ${QT_INCLUDE_DIR}
 ${QtLocation_INCLUDE_DIRS}
)

include(${QT_USE_FILE})

set( qtmobility_SRCS
 QtMobilityPositionProviderPlugin.cpp
)

set( QtPositionProviderPlugin_LIBS ${QtLocation_LIBRARIES} )

marble_add_plugin( QtMobilityPositionProviderPlugin ${qtmobility_SRCS} )
