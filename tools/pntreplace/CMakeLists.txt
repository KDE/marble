SET (TARGET pntreplace)
PROJECT (${TARGET})

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set(${TARGET}_SRC pntreplace.cpp svgxmlhandler.cpp)
add_executable(${TARGET} ${${TARGET}_SRC})

target_link_libraries(${TARGET} Qt6::Xml Qt6::Widgets Qt6::Core5Compat)
