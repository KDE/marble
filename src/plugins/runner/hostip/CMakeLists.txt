PROJECT( HostipPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set(hostip_SRCS HostipRunner.cpp HostipPlugin.cpp HostipRunner.h HostipPlugin.h)

marble_add_plugin( HostipPlugin ${hostip_SRCS})
