PROJECT( OsmAnnotatePlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( osmannotate_SRCS
  AreaAnnotation.cpp
  GeoWidgetBubble.cpp
  OsmAnnotatePlugin.cpp
  PlacemarkTextAnnotation.cpp
  SceneGraphicsItem.cpp
  TextEditor.cpp
)

marble_add_plugin( OsmAnnotatePlugin ${osmannotate_SRCS} )
