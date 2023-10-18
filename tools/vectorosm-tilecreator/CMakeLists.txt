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
clipper2/clipper.engine.cpp
clipper2/clipper.offset.cpp
clipper2/clipper.rectclip.cpp
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
if (STATIC_BUILD)
    target_link_libraries(marble-vectorosm-tilecreator OsmPlugin ShpPlugin)
endif()

add_executable(marble-vectorosm-cachetiles vectorosm-cachetiles.cpp)
target_link_libraries(marble-vectorosm-cachetiles ${TARGET})

add_executable(marble-vectorosm-process-land-polygons
    vectorosm-process-land-polygons.cpp
)
target_link_libraries(marble-vectorosm-process-land-polygons vectorosm-toolchain)
if (STATIC_BUILD)
    target_link_libraries(marble-vectorosm-process-land-polygons OsmPlugin ShpPlugin)
endif()

add_executable(marble-vectorosm-tirex-backend
    vectorosm-tirex-backend.cpp
    TirexBackend.cpp
)
target_link_libraries(marble-vectorosm-tirex-backend vectorosm-toolchain)
if (STATIC_BUILD)
    target_link_libraries(marble-vectorosm-tirex-backend OsmPlugin ShpPlugin)
endif()

if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()
