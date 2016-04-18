PROJECT( OpenLocationCodeSearchPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( openLocationCodeSearch_SRCS
OpenLocationCodeSearchPlugin.cpp
OpenLocationCodeSearchRunner.cpp
 )

marble_add_plugin( OpenLocationCodeSearchPlugin ${openLocationCodeSearch_SRCS} )
