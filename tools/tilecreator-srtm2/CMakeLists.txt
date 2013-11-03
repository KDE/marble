SET (TARGET tilecreator-srtm2)
PROJECT (${TARGET})

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
if( QT4_FOUND )
  include( ${QT_USE_FILE} )
endif()

set( ${TARGET}_SRC main.cpp tccore.cpp )
add_definitions( -DMAKE_MARBLE_LIB -DTILECREATOR )
add_executable( ${TARGET} ${${TARGET}_SRC} )

if (QT4_FOUND)
  target_link_libraries( ${TARGET} ${QT_QTCORE_LIBRARY} ${QT_QTMAIN_LIBRARY} marblewidget )
else()
  target_link_libraries( ${TARGET} ${Qt5Core_LIBRARIES} marblewidget )
endif()

if (APPLE)
  target_link_libraries (${TARGET} ${APP_SERVICES_LIBRARY})
endif (APPLE)
