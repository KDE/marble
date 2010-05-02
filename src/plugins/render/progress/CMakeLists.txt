PROJECT(ProgressFloatItem)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/progress
 ${CMAKE_BINARY_DIR}/src/plugins/render/progress
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( progress_SRCS ProgressFloatItem.cpp )

marble_add_plugin( ProgressFloatItem ${progress_SRCS} )
