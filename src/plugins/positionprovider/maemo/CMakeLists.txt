PROJECT( MaemoPositionProviderPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${liblocation_INCLUDE_DIRS}
)

set( maemo_SRCS
 MaemoPositionProviderPlugin.cpp
)

set( MaemoPositionProviderPlugin_LIBS ${liblocation_LIBRARIES} )

marble_add_plugin( MaemoPositionProviderPlugin ${maemo_SRCS} )
