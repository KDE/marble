PROJECT( OsmPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_SOURCE_DIR}/src/3rdparty/o5mreader
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_SOURCE_DIR}/writers
 ${CMAKE_CURRENT_SOURCE_DIR}/translators
 ${CMAKE_CURRENT_BINARY_DIR}
 ${ZLIB_INCLUDE_DIRS}
)

if (Protobuf_FOUND AND Protobuf_PROTOC_EXECUTABLE AND NOT MSVC) # FIXME linking issue on Windows
    protobuf_generate_cpp(pbf_srcs pbf_hdrs
        ${CMAKE_SOURCE_DIR}/tools/osm-addresses/pbf/fileformat.proto
        ${CMAKE_SOURCE_DIR}/tools/osm-addresses/pbf/osmformat.proto
    )
    set(EXTRA_LIBS protobuf::libprotobuf protobuf::libprotoc ${ZLIB_LIBRARIES})
    add_definitions(-DHAVE_PROTOBUF)

	if(MSVC)
	add_definitions(-DPROTOBUF_USE_DLLS)
	endif()
endif()

set( osm_writers_SRCS
        writers/OsmObjectAttributeWriter.cpp
        writers/OsmNodeTagWriter.cpp
        writers/OsmWayTagWriter.cpp
        writers/OsmRelationTagWriter.cpp
        writers/OsmTagWriter.cpp
        writers/OsmTagTagWriter.cpp

        writers/OsmObjectAttributeWriter.h
        writers/OsmNodeTagWriter.h
        writers/OsmWayTagWriter.h
        writers/OsmRelationTagWriter.h
        writers/OsmTagWriter.h
        writers/OsmTagTagWriter.h
   )
set( osm_translators_SRCS
        translators/OsmDocumentTagTranslator.cpp
        translators/O5mWriter.cpp
        translators/OsmConverter.cpp

        translators/OsmDocumentTagTranslator.h
        translators/O5mWriter.h
        translators/OsmConverter.h
   )

set( osm_SRCS
  OsmParser.cpp
  OsmPlugin.cpp
  OsmRunner.cpp
  OsmNode.cpp
  OsmWay.cpp
  OsmRelation.cpp
  OsmElementDictionary.cpp
  OsmPbfParser.cpp

  OsmParser.h
  OsmPlugin.h
  OsmRunner.h
  OsmNode.h
  OsmWay.h
  OsmRelation.h
  OsmElementDictionary.h
  OsmPbfParser.h

  ${pbf_srcs}
)

marble_add_plugin( OsmPlugin ${osm_SRCS} ${osm_writers_SRCS} ${osm_translators_SRCS} )
target_link_libraries(OsmPlugin o5mreader ${EXTRA_LIBS})

find_package(ECM ${REQUIRED_ECM_VERSION} QUIET)
if(NOT ECM_FOUND)
    return()
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs)

macro_optional_find_package(KF6 ${REQUIRED_KF6_MIN_VERSION} QUIET COMPONENTS KIO)
if(NOT KF6_FOUND)
    return()
endif()

# register marble part as handler
# install again once part is fixed to load and show the passed file
# install(FILES marble_part_osm.desktop DESTINATION ${KDE_INSTALL_FULL_KSERVICE5DIR})

# register thumbnail plugin as handler
install(FILES marble_thumbnail_osm.desktop DESTINATION ${KDE_INSTALL_FULL_APPDIR})

