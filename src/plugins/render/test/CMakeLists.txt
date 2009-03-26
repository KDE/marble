PROJECT( TestPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/test
 ${CMAKE_BINARY_DIR}/src/plugins/render/test
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( test_SRCS TestPlugin.cpp )

marble_add_plugin( TestPlugin ${test_SRCS} )
