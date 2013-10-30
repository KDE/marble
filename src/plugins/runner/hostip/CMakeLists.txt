PROJECT( HostipPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
if( QT4_FOUND )
  INCLUDE(${QT_USE_FILE})
endif()

set( hostip_SRCS HostipRunner.cpp HostipPlugin.cpp )

marble_add_plugin( HostipPlugin ${hostip_SRCS} )
