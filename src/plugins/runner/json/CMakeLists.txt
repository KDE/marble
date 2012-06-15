PROJECT( JsonPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/runner/json
 ${CMAKE_BINARY_DIR}/src/plugins/runner/json
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( json_SRCS JsonRunner.cpp JsonPlugin.cpp )

marble_add_plugin( JsonPlugin ${json_SRCS} )
