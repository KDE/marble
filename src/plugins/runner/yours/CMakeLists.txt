PROJECT( YoursPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/runner/yours
 ${CMAKE_BINARY_DIR}/src/plugins/runner/yours
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( yours_SRCS YoursRunner.cpp YoursPlugin.cpp )

marble_add_plugin( YoursPlugin ${yours_SRCS} )
