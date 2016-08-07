SET (TARGET maptheme-previewimage)
PROJECT (${TARGET})

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( ${TARGET}_SRC main.cpp )
QT_ADD_RESOURCES( RESOURCES canvas.qrc )
add_executable( ${TARGET} ${${TARGET}_SRC} ${RESOURCES} )

target_link_libraries(${TARGET} marblewidget)
