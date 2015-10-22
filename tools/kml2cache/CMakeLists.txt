SET (TARGET kml2cache)
PROJECT (${TARGET})

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)

set( ${TARGET}_SRC kml2cache.cpp )
add_definitions( -DMAKE_MARBLE_LIB )
add_executable( ${TARGET} ${${TARGET}_SRC} )
target_link_libraries( ${TARGET} ${Qt5Core_LIBRARIES} marblewidget-qt5 )
