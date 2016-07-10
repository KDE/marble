SET (TARGET sentineltile)
PROJECT (${TARGET})

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( ${TARGET}_SRC sentineltile.cpp tileprocessor.cpp )
add_executable( ${TARGET} ${${TARGET}_SRC} )

target_link_libraries( ${TARGET}  Qt5::Core Qt5::Widgets )
