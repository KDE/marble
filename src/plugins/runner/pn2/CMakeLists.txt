PROJECT( Pn2Plugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/runner/pn2
 ${CMAKE_BINARY_DIR}/src/plugins/runner/pn2
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( pn2_SRCS Pn2Plugin.cpp Pn2Runner.cpp )

marble_add_plugin( Pn2Plugin ${pn2_SRCS} )
