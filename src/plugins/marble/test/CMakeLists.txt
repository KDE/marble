PROJECT(MarbleTestPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/marble/test
 ${CMAKE_BINARY_DIR}/src/plugins/marble/test
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( marbletest_SRCS MarbleTestPlugin.cpp )

marble_add_plugin( MarbleTestPlugin ${marbletest_SRCS} )