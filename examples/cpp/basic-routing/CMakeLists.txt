SET (TARGET basic-routing)
PROJECT (${TARGET})

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
if( QT4_FOUND )
  include( ${QT_USE_FILE} )
endif()

set( ${TARGET}_SRC main.cpp)
add_executable( ${TARGET} ${${TARGET}_SRC} )

target_link_libraries( ${TARGET} marblewidget )
if (QT4_FOUND)
  target_link_libraries( ${TARGET} ${QT_QTCORE_LIBRARY} ${QT_QTMAIN_LIBRARY} )
else()
  target_link_libraries( ${TARGET} ${Qt5Core_LIBRARIES} )
endif()

install( TARGETS ${TARGET} RUNTIME DESTINATION ${MARBLE_EXAMPLES_INSTALL_DIR}/${TARGET} )
install( FILES ${${TARGET}_SRC} Readme.txt DESTINATION ${MARBLE_EXAMPLES_INSTALL_DIR}/${TARGET} )
install( FILES CMakeLists.txt.external DESTINATION ${MARBLE_EXAMPLES_INSTALL_DIR}/${TARGET} RENAME CMakeLists.txt )
