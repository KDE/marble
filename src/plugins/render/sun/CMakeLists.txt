PROJECT( SunPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set(sun_SRCS SunPlugin.cpp SunPlugin.h)

marble_add_plugin( SunPlugin ${sun_SRCS})
