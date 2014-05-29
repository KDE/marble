SET (TARGET marbleQuick2)
PROJECT (${TARGET})

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)

set( ${TARGET}_SRC main.cpp)

add_executable( ${TARGET} ${${TARGET}_SRC} )

target_link_libraries( ${TARGET} ${OpenCV_LIBRARIES} marblewidget )
target_link_libraries( ${TARGET} ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Quick_LIBRARIES} ${Qt5Widget_LIBRARIES} )

add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
                       ${CMAKE_CURRENT_SOURCE_DIR}/main.qml
                       ${CMAKE_CURRENT_BINARY_DIR}
                   )

install( TARGETS ${TARGET} RUNTIME DESTINATION ${MARBLE_EXAMPLES_INSTALL_DIR}/${TARGET} )
install( FILES ${${TARGET}_SRC} main.qml Readme.txt DESTINATION ${MARBLE_EXAMPLES_INSTALL_DIR}/${TARGET} )
install( FILES CMakeLists.txt.external DESTINATION ${MARBLE_EXAMPLES_INSTALL_DIR}/${TARGET} RENAME CMakeLists.txt )
