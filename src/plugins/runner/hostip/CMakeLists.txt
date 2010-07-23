PROJECT( HostipPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/runner/hostip
 ${CMAKE_BINARY_DIR}/src/plugins/runner/hostip
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( hostip_SRCS HostipRunner.cpp HostipPlugin.cpp )

marble_add_plugin( HostipPlugin ${hostip_SRCS} )
