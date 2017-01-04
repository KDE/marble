SET (TARGET vectorosm-toolchain)
PROJECT (${TARGET})

if (POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW)
endif()

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
../../src/lib/marble/osm
../../src/lib/marble/geodata/writer
../../src/lib/marble/geodata/parser
../../src/lib/marble/geodata/data
../../src/lib/marble/geodata
../../src/lib/marble/
../mbtile-import
)

add_library(${TARGET} STATIC
../mbtile-import/MbTileWriter.cpp
clipper/clipper.cpp
NodeReducer.cpp
PeakAnalyzer.cpp
SpellChecker.cpp
TagsFilter.cpp
TileIterator.cpp
TileDirectory.cpp
TileQueue.cpp
VectorClipper.cpp
WayConcatenator.cpp
WayChunk.cpp
)
target_link_libraries(${TARGET} marblewidget Qt5::Sql)

add_executable(marble-vectorosm-tilecreator vectorosm-tilecreator.cpp)
target_link_libraries(marble-vectorosm-tilecreator ${TARGET})

add_executable(marble-vectorosm-cachetiles vectorosm-cachetiles.cpp)
target_link_libraries(marble-vectorosm-cachetiles ${TARGET})
