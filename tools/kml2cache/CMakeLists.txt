SET (TARGET kml2cache)
PROJECT (${TARGET})

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( ${TARGET}_SRC kml2cache.cpp )
add_executable( ${TARGET} ${${TARGET}_SRC} )
target_link_libraries(${TARGET} marblewidget)
