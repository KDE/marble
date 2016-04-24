SET (TARGET stars)
PROJECT (${TARGET})

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( ${TARGET}_SRC stars.cpp )
add_definitions( -D_USE_MATH_DEFINES=1 )
add_executable( ${TARGET} ${${TARGET}_SRC} )

target_link_libraries( ${TARGET} Qt5::Core )
