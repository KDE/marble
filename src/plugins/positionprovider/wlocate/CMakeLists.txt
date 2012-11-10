PROJECT( WlocatePositionProviderPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/positionprovider/wlocate
 ${CMAKE_BINARY_DIR}/src/plugins/positionprovider/wlocate
 ${QT_INCLUDE_DIR}
 ${liblocation_INCLUDE_DIRS}
)

include(${QT_USE_FILE})

set( wlocate_SRCS
 WlocatePositionProviderPlugin.cpp
)

set( WlocatePositionProviderPlugin_LIBS ${LIBWLOCATE_LIBRARIES} )

marble_add_plugin( WlocatePositionProviderPlugin ${wlocate_SRCS} )
