SET (TARGET osm-sisyphus)
PROJECT (${TARGET})

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
if( QT4_FOUND )
  include( ${QT_USE_FILE} )
endif()

set( ${TARGET}_SRC
jobparameters.cpp
jobmanager.cpp
jobqueue.cpp
logger.cpp
job.cpp
upload.cpp
region.cpp
main.cpp
)
add_definitions( -DMAKE_MARBLE_LIB )
add_executable( ${TARGET} ${${TARGET}_SRC} )
marble_qt4_automoc( ${${TARGET}_SRC} )

target_link_libraries( ${TARGET} marblewidget )
if(QT4_FOUND)
   target_link_libraries( ${TARGET} ${QT_QTCORE_LIBRARY} ${QT_QTMAIN_LIBRARY} ${QT_QTSQL_LIBRARY} )
else()
  target_link_libraries( ${TARGET} ${Qt5Core_LIBRARIES} ${Qt5Sql_LIBRARIES} )
endif()
