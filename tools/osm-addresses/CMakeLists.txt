SET (TARGET osm-addresses)
PROJECT (${TARGET})

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${PROTOBUF_INCLUDE_DIRS}
 ${ZLIB_INCLUDE_DIRS}
 ../../src/plugins/runner/local-osm-search
)

set( ${TARGET}_SRC
OsmRegion.cpp
OsmRegionTree.cpp
OsmParser.cpp
SqlWriter.cpp
Writer.cpp
main.cpp
pbf/PbfParser.cpp
xml/XmlParser.cpp
../../src/plugins/runner/local-osm-search/OsmPlacemark.cpp
../../src/plugins/runner/local-osm-search/DatabaseQuery.cpp
)
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS
pbf/fileformat.proto
pbf/osmformat.proto
)
add_executable( ${TARGET} ${${TARGET}_SRC} ${PROTO_SRCS} ${PROTO_HDRS} )

target_link_libraries( ${TARGET}
    marblewidget
    Qt5::Sql
    ${PROTOBUF_LIBRARIES}
    ${ZLIB_LIBRARIES}
)
