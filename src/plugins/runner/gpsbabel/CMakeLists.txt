PROJECT( GpsbabelPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set(gpsbabel_SRCS GpsbabelPlugin.cpp GpsbabelRunner.cpp GpsbabelPlugin.h GpsbabelRunner.h)

marble_add_plugin( GpsbabelPlugin ${gpsbabel_SRCS})

