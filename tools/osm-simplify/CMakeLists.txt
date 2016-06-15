SET (TARGET osm-simplify)
PROJECT (${TARGET})

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets)
find_package(Qt5Declarative)

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
../../src/plugins/runner/osm
../../src/plugins/runner/osm/writers
../../src/plugins/runner/osm/translators
../../src/lib/marble/geodata/writer
../../src/lib/marble/geodata/parser
../../src/lib/marble/geodata/data
../../src/lib/marble/geodata
../../src/lib/marble/
)

set( ${TARGET}_SRC 
main.cpp
BaseFilter.cpp
  PlacemarkFilter.cpp
    LineStringProcessor.cpp
../../src/plugins/runner/osm/OsmParser.cpp
../../src/plugins/runner/osm/o5mreader.cpp
../../src/plugins/runner/osm/OsmElementDictionary.cpp
../../src/plugins/runner/osm/OsmNode.cpp
../../src/plugins/runner/osm/OsmParser.cpp
../../src/plugins/runner/osm/OsmPlugin.cpp
../../src/plugins/runner/osm/OsmRelation.cpp
../../src/plugins/runner/osm/OsmRunner.cpp
../../src/plugins/runner/osm/OsmWay.cpp
../../src/plugins/runner/osm/translators/OsmDocumentTagTranslator.cpp
../../src/plugins/runner/osm/translators/OsmFeatureTagTranslator.cpp
../../src/plugins/runner/osm/translators/OsmPlacemarkTagTranslator.cpp
../../src/plugins/runner/osm/writers/OsmNodeTagWriter.cpp
../../src/plugins/runner/osm/writers/OsmObjectAttributeWriter.cpp
../../src/plugins/runner/osm/writers/OsmRelationTagWriter.cpp
../../src/plugins/runner/osm/writers/OsmTagTagWriter.cpp
../../src/plugins/runner/osm/writers/OsmTagWriter.cpp
../../src/plugins/runner/osm/writers/OsmWayTagWriter.cpp
)
add_definitions( -DMAKE_MARBLE_LIB )
add_executable( ${TARGET} ${${TARGET}_SRC} )
 
target_link_libraries( ${TARGET} )
target_link_libraries( ${TARGET} Qt5::Core marblewidget-qt5 )
