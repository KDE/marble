PROJECT( CachePlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/runner/cache
 ${CMAKE_BINARY_DIR}/src/plugins/runner/cache
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( cache_SRCS CachePlugin.cpp CacheRunner.cpp )

marble_add_plugin( CachePlugin ${cache_SRCS} )
