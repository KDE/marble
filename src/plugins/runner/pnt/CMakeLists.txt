PROJECT( PntPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/runner/pnt
 ${CMAKE_BINARY_DIR}/src/plugins/runner/pnt
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( pnt_SRCS PntPlugin.cpp PntRunner.cpp )

marble_add_plugin( PntPlugin ${pnt_SRCS} )
