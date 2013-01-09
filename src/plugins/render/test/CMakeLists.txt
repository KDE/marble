PROJECT( TestPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( test_SRCS TestPlugin.cpp )

marble_add_plugin( TestPlugin ${test_SRCS} )
