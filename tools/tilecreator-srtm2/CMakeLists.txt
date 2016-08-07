SET (TARGET tilecreator-srtm2)
PROJECT (${TARGET})

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( ${TARGET}_SRC main.cpp tccore.cpp )
add_definitions(-DTILECREATOR)
add_executable( ${TARGET} ${${TARGET}_SRC} )

target_link_libraries(${TARGET} marblewidget)

if (APPLE)
  target_link_libraries (${TARGET} ${APP_SERVICES_LIBRARY})
endif (APPLE)
