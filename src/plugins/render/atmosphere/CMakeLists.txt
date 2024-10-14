PROJECT( AtmospherePlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set(atmosphere_SRCS AtmospherePlugin.cpp AtmospherePlugin.h)

marble_add_plugin( AtmospherePlugin ${atmosphere_SRCS})
