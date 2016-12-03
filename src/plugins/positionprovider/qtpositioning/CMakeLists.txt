PROJECT( QtPositioningPositionProviderPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

SET( qtpositioning_SRCS
 QtPositioningPositionProviderPlugin.cpp
)

SET( QtPositioningPositionProviderPlugin_LIBS Qt5::Positioning )

MARBLE_ADD_PLUGIN( QtPositioningPositionProviderPlugin ${qtpositioning_SRCS} )
