PROJECT( YoursPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( yours_SRCS YoursRunner.cpp YoursPlugin.cpp )

marble_add_plugin( YoursPlugin ${yours_SRCS} )
