PROJECT( JsonPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( json_SRCS JsonRunner.cpp JsonPlugin.cpp JsonParser.cpp )

set( JsonPlugin_LIBS Qt5::Script )

marble_add_plugin( JsonPlugin ${json_SRCS} )
