project(sgp4)

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( sgp4_SRCS
 sgp4ext.cpp
 sgp4unit.cpp
 sgp4io.cpp )

add_library(sgp4 STATIC ${sgp4_SRCS})

