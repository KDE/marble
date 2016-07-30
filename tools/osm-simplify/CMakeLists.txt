cmake_minimum_required(VERSION 2.8.12)

SET (TARGET osm-simplify)
PROJECT (${TARGET})

if (POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW)
endif()

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
../../src/lib/marble/osm
../../src/lib/marble/geodata/writer
../../src/lib/marble/geodata/parser
../../src/lib/marble/geodata/data
../../src/lib/marble/geodata
../../src/lib/marble/
)

set( ${TARGET}_SRC 
main.cpp
BaseClipper.cpp
BaseFilter.cpp
  PlacemarkFilter.cpp
    ShpCoastlineProcessor.cpp
    LineStringProcessor.cpp
    TinyPlanetProcessor.cpp
    NodeReducer.cpp
    TagsFilter.cpp
    	WayConcatenator.cpp
WayChunk.cpp
)

add_executable( ${TARGET} ${${TARGET}_SRC} )

target_link_libraries( ${TARGET} Qt5::Core marblewidget-qt5)
