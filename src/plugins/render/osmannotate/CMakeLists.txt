PROJECT( OsmAnnotatePlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

FILE( GLOB osmannotate_local_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp )

set( osmannotate_SRCS
        ${geodata_handlers_osm_SRCS} )

marble_add_plugin( OsmAnnotatePlugin ${osmannotate_SRCS} )
