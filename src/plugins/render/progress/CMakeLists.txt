PROJECT(ProgressFloatItem)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( progress_SRCS ProgressFloatItem.cpp )

marble_add_plugin( ProgressFloatItem ${progress_SRCS} )
