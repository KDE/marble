PROJECT( OsmAnnotatePlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/osmannotate
 ${CMAKE_BINARY_DIR}/src/plugins/render/osmannotate
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

FILE( GLOB geodata_handlers_osm_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} osm/*.cpp )

set( osmannotate_SRCS
        OsmAnnotatePlugin.cpp
        PlacemarkTextAnnotation.cpp
        TmpGraphicsItem.cpp
        TextAnnotation.cpp
        AreaAnnotation.cpp
        GeoWidgetBubble.cpp
        TextEditor.cpp
        ${geodata_handlers_osm_SRCS} )

marble_add_plugin( OsmAnnotatePlugin ${osmannotate_SRCS} )
