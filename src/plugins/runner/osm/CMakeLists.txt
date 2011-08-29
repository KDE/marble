PROJECT( OsmPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_SOURCE_DIR}/handlers
 ${CMAKE_BINARY_DIR}/src/plugins/runner/osm
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( osm_handlers_SRCS
        handlers/OsmBoundsTagHandler.cpp
        handlers/OsmBoundTagHandler.cpp
        handlers/OsmElementDictionary.cpp
        handlers/OsmGlobals.cpp
        handlers/OsmNdTagHandler.cpp
        handlers/OsmNodeFactory.cpp
        handlers/OsmNodeTagHandler.cpp
        handlers/OsmOsmTagHandler.cpp
        handlers/OsmTagTagHandler.cpp
        handlers/OsmWayTagHandler.cpp
   )

set( osm_SRCS OsmParser.cpp OsmPlugin.cpp OsmRunner.cpp )

marble_add_plugin( OsmPlugin ${osm_SRCS}  ${osm_handlers_SRCS} )
