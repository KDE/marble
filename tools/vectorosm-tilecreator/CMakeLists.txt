SET (TARGET marble-vectorosm-tilecreator)
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

set( ${TARGET}_SRC 
../mbtile-import/MbTileWriter.cpp
clipper/clipper.cpp
main.cpp
BaseClipper.cpp
BaseFilter.cpp
NodeReducer.cpp
PeakAnalyzer.cpp
SpellChecker.cpp
TagsFilter.cpp
TileIterator.cpp
TileDirectory.cpp
VectorClipper.cpp
WayConcatenator.cpp
WayChunk.cpp
)

add_executable( ${TARGET} ${${TARGET}_SRC} )

target_link_libraries(${TARGET} marblewidget Qt5::Sql)
