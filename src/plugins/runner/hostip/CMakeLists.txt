PROJECT( HostipPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( hostip_SRCS HostipRunner.cpp HostipPlugin.cpp )

marble_add_plugin( HostipPlugin ${hostip_SRCS} )
