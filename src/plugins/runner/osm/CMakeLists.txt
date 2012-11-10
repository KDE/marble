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
        handlers/OsmRelationTagHandler.cpp
        handlers/OsmMemberTagHandler.cpp
        handlers/OsmTagTagHandler.cpp
        handlers/OsmWayTagHandler.cpp
        handlers/OsmWayFactory.cpp
        handlers/OsmRelationFactory.cpp
   )

set( osm_SRCS OsmParser.cpp OsmPlugin.cpp OsmRunner.cpp )

marble_add_plugin( OsmPlugin ${osm_SRCS}  ${osm_handlers_SRCS} )

if(QTONLY)
  if(WIN32 OR APPLE)
    # nothing to do
  else(WIN32 OR APPLE)
    install(FILES marble_osm.desktop DESTINATION ${APPS_INSTALL_DIR})
  endif(WIN32 OR APPLE)
else(QTONLY)
  install(PROGRAMS marble_osm.desktop DESTINATION ${APPS_INSTALL_DIR})
  install(FILES marble_part_osm.desktop DESTINATION ${SERVICES_INSTALL_DIR})
endif(QTONLY)
