SET (TARGET osm-sisyphus)
PROJECT (${TARGET})

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

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
add_executable( ${TARGET} ${${TARGET}_SRC} )

target_link_libraries(${TARGET} marblewidget Qt5::Sql)
