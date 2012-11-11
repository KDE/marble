PROJECT( WlocatePositionProviderPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
 ${liblocation_INCLUDE_DIRS}
)

include(${QT_USE_FILE})

set( wlocate_SRCS
 WlocatePositionProviderPlugin.cpp
)

set( WlocatePositionProviderPlugin_LIBS ${LIBWLOCATE_LIBRARIES} )

marble_add_plugin( WlocatePositionProviderPlugin ${wlocate_SRCS} )
