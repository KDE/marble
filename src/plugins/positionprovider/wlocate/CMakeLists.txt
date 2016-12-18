PROJECT( WlocatePositionProviderPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${LIBWLOCATE_INCLUDE_DIR}
)

INCLUDE_DIRECTORIES(${Qt5Concurrent_INCLUDE_DIRS})

set( wlocate_SRCS
 WlocatePositionProviderPlugin.cpp
)

set( WlocatePositionProviderPlugin_LIBS ${LIBWLOCATE_LIBRARIES} )

marble_add_plugin( WlocatePositionProviderPlugin ${wlocate_SRCS} )
