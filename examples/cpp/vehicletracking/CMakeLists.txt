SET (TARGET vehicletracking)
PROJECT (${TARGET})

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( ${TARGET}_SRC vehicletracking.cpp)
add_executable( ${TARGET} ${${TARGET}_SRC} )

target_link_libraries(${TARGET} marblewidget)

install( TARGETS ${TARGET} RUNTIME DESTINATION ${MARBLE_EXAMPLES_INSTALL_DIR}/${TARGET} )
install( FILES ${${TARGET}_SRC} DESTINATION ${MARBLE_EXAMPLES_INSTALL_DIR}/${TARGET} )
