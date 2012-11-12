PROJECT( QtMobilityPositionProviderPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
 ${QTLOCATION_INCLUDE_DIR}
 ${QTMOBILITY_INCLUDE_DIR}
)

INCLUDE(${QT_USE_FILE})

SET( qtmobility_SRCS
 QtMobilityPositionProviderPlugin.cpp
)

SET( QtMobilityPositionProviderPlugin_LIBS ${QTLOCATION_LIBRARIES} )

MARBLE_ADD_PLUGIN( QtMobilityPositionProviderPlugin ${qtmobility_SRCS} )
