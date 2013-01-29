PROJECT( AnnotatePlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${CMAKE_SOURCE_DIR}/src/lib/geodata/handlers/kml
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( annotate_SRCS
  AnnotatePlugin.cpp
  AreaAnnotation.cpp
  GeoWidgetBubble.cpp
  PlacemarkTextAnnotation.cpp
  SceneGraphicsItem.cpp
  TextEditor.cpp
)

marble_add_plugin( AnnotatePlugin ${annotate_SRCS} )
