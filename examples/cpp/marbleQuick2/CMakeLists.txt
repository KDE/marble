SET (TARGET marbleQuick2)
PROJECT (${TARGET})

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( ${TARGET}_SRC main.cpp)

qt5_add_resources(marble_QRCS marbleQuick2.qrc)
add_executable( ${TARGET} ${${TARGET}_SRC} ${marble_QRCS} )

target_link_libraries( ${TARGET}
    marblewidget
    marbledeclarative
)

# TODO: marbledeclarative is a private lib currently, not a plugin, and headers are not installed
# so do not install this example for now
return()


add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
                       ${CMAKE_CURRENT_SOURCE_DIR}/main.qml
                       ${CMAKE_CURRENT_BINARY_DIR}
                   )

install( TARGETS ${TARGET} RUNTIME DESTINATION ${MARBLE_EXAMPLES_INSTALL_DIR}/${TARGET} )
install( FILES ${${TARGET}_SRC} main.qml Readme.txt DESTINATION ${MARBLE_EXAMPLES_INSTALL_DIR}/${TARGET} )
install( FILES CMakeLists.txt.external DESTINATION ${MARBLE_EXAMPLES_INSTALL_DIR}/${TARGET} RENAME CMakeLists.txt )
