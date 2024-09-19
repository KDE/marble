PROJECT( CachePlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( cache_SRCS CachePlugin.cpp CacheRunner.cpp CachePlugin.h CacheRunner.h)

marble_add_plugin( CachePlugin ${cache_SRCS} )
