PROJECT( LogPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set(log_SRCS LogPlugin.cpp LogRunner.cpp LogPlugin.h LogRunner.h)

marble_add_plugin( LogPlugin ${log_SRCS})
