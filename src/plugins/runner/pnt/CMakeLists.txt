PROJECT( PntPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set(pnt_SRCS PntPlugin.cpp PntRunner.cpp PntPlugin.h PntRunner.h)

marble_add_plugin( PntPlugin ${pnt_SRCS})
