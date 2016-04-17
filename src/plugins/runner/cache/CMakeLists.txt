PROJECT( CachePlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( cache_SRCS CachePlugin.cpp CacheRunner.cpp )

marble_add_plugin( CachePlugin ${cache_SRCS} )
