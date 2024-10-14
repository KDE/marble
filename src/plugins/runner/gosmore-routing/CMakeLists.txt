PROJECT( GosmoreRoutingPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set(gosmore_SRCS GosmoreRoutingRunner.cpp GosmoreRoutingPlugin.cpp GosmoreRoutingRunner.h GosmoreRoutingPlugin.h)

marble_add_plugin( GosmoreRoutingPlugin ${gosmore_SRCS})
