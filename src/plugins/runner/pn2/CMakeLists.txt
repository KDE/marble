PROJECT( Pn2Plugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_BINARY_DIR}/src/plugins/runner/pn2
)

set(pn2_SRCS Pn2Plugin.cpp Pn2Runner.cpp Pn2Plugin.h Pn2Runner.h)

marble_add_plugin( Pn2Plugin ${pn2_SRCS})
