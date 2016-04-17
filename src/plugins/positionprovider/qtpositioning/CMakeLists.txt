PROJECT( QtPositioningPositionProviderPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${Qt5Location_INCLUDE_DIR}
 ${Qt5Positioning_INCLUDE_DIR}
)

SET( qtpositioning_SRCS
 QtPositioningPositionProviderPlugin.cpp
)

SET( QtPositioningPositionProviderPlugin_LIBS ${Qt5Location_LIBRARIES} ${Qt5Positioning_LIBRARIES} )

MARBLE_ADD_PLUGIN( QtPositioningPositionProviderPlugin ${qtpositioning_SRCS} )
