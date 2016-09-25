PROJECT( OsmPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_SOURCE_DIR}/src/3rdparty/o5mreader
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_SOURCE_DIR}/writers
 ${CMAKE_CURRENT_SOURCE_DIR}/translators
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( osm_writers_SRCS
        writers/OsmObjectAttributeWriter.cpp
        writers/OsmNodeTagWriter.cpp
        writers/OsmWayTagWriter.cpp
        writers/OsmRelationTagWriter.cpp
        writers/OsmTagWriter.cpp
        writers/OsmTagTagWriter.cpp
   )
set( osm_translators_SRCS
        translators/OsmDocumentTagTranslator.cpp
        translators/O5mWriter.cpp
        translators/OsmConverter.cpp
   )

set( osm_SRCS
  OsmParser.cpp
  OsmPlugin.cpp
  OsmRunner.cpp
  OsmNode.cpp
  OsmWay.cpp
  OsmRelation.cpp
  OsmElementDictionary.cpp
)

marble_add_plugin( OsmPlugin ${osm_SRCS} ${osm_writers_SRCS} ${osm_translators_SRCS} )
target_link_libraries(OsmPlugin o5mreader)

find_package(ECM ${REQUIRED_ECM_VERSION} QUIET)
if(NOT ECM_FOUND)
    return()
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs)

macro_optional_find_package(KF5 ${REQUIRED_KF5_VERSION} QUIET COMPONENTS KIO)
if(NOT KF5_FOUND)
    return()
endif()

# register marble part as handler
# install again once part is fixed to load and show the passed file
# install(FILES marble_part_osm.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})

# register thumbnail plugin as handler
install(FILES marble_thumbnail_osm.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})

