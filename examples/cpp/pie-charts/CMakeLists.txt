SET (TARGET pie-charts)
PROJECT (${TARGET})

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set(${TARGET}_SRC main.cpp)
add_executable(${TARGET} ${${TARGET}_SRC})

target_link_libraries(${TARGET} marblewidget)

install(TARGETS ${TARGET} RUNTIME DESTINATION bin)
