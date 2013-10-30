PROJECT( WlocatePositionProviderPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
 ${Qt5Concurrent_INCLUDE_DIRS}
 ${liblocation_INCLUDE_DIRS}
)

if( QT4_FOUND )
  include(${QT_USE_FILE})
endif()

set( wlocate_SRCS
 WlocatePositionProviderPlugin.cpp
)

set( WlocatePositionProviderPlugin_LIBS ${LIBWLOCATE_LIBRARIES} )

marble_add_plugin( WlocatePositionProviderPlugin ${wlocate_SRCS} )
