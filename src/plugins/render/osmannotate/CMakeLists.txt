PROJECT( OsmAnnotatePlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/osmannotate
 ${CMAKE_BINARY_DIR}/src/plugins/render/osmannotate
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( osmannotate_SRCS OsmAnnotatePlugin.cpp TmpGraphicsItem.cpp TextAnnotation.cpp)

marble_add_plugin( OsmAnnotatePlugin ${osmannotate_SRCS} )
