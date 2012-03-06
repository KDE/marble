PROJECT( LogPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( log_SRCS LogPlugin.cpp LogRunner.cpp )

marble_add_plugin( LogPlugin ${log_SRCS} )
