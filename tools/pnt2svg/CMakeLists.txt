SET (TARGET pnt2svg)
PROJECT (${TARGET})

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( ${TARGET}_SRC pnt2svg.cpp )
add_executable( ${TARGET} ${${TARGET}_SRC} )

target_link_libraries( ${TARGET} Qt5::Core )
