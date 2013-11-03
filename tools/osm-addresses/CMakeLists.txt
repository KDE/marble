SET (TARGET osm-addresses)
PROJECT (${TARGET})

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
 ${PROTOBUF_INCLUDE_DIRS}
 ${ZLIB_INCLUDE_DIRS}
 ../../src/plugins/runner/local-osm-search
)
if( QT4_FOUND )
  include( ${QT_USE_FILE} )
endif()

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
add_definitions( -DMAKE_MARBLE_LIB )
add_executable( ${TARGET} ${${TARGET}_SRC} ${PROTO_SRCS} ${PROTO_HDRS} )
marble_qt4_automoc( ${${TARGET}_SRC} )

target_link_libraries( ${TARGET} ${PROTOBUF_LIBRARIES} ${ZLIB_LIBRARIES} marblewidget )
if(QT4_FOUND)
   target_link_libraries( ${TARGET} ${QT_QTCORE_LIBRARY} ${QT_QTMAIN_LIBRARY} ${QT_QTSQL_LIBRARY} )
else()
  target_link_libraries( ${TARGET} ${Qt5Core_LIBRARIES} ${Qt5Sql_LIBRARIES} )
endif()
