PROJECT( FileReaderPositionProviderPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/positionprovider/filereader
 ${CMAKE_BINARY_DIR}/src/plugins/positionprovider/filereader
 ${QT_INCLUDE_DIR}
)

include(${QT_USE_FILE})

set( the_SRCS
 FileReaderPositionProviderPlugin.cpp
)

marble_add_plugin( FileReaderPositionProviderPlugin ${the_SRCS} )
