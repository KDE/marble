SET (TARGET asc2kml)
PROJECT (${TARGET})

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set(${TARGET}_SRC asc2kml.cpp)
add_executable(${TARGET} ${${TARGET}_SRC})

target_link_libraries(${TARGET} Qt6::Core)
