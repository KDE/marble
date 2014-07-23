PROJECT( AnnotatePlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${CMAKE_SOURCE_DIR}/src/lib/marble/geodata/handlers/kml
 ${CMAKE_SOURCE_DIR}/src/lib/marble/layers
 ${QT_INCLUDE_DIR}
)
if( QT4_FOUND )
  INCLUDE(${QT_USE_FILE})
endif()

set( annotate_UI
  EditGroundOverlayDialog.ui
  EditPolygonDialog.ui
)

set( annotate_SRCS
  AnnotatePlugin.cpp
  AreaAnnotation.cpp
  EditGroundOverlayDialog.cpp
  EditPolygonDialog.cpp
  GeoWidgetBubble.cpp
  GroundOverlayFrame.cpp
  MergingNodesAnimation.cpp
  PlacemarkTextAnnotation.cpp
  SceneGraphicsItem.cpp
  SceneGraphicsTypes.cpp
  TextEditor.cpp
)

qt_wrap_ui(annotate_SRCS ${annotate_UI})
marble_qt4_automoc(${annotate_SRCS})

marble_add_plugin( AnnotatePlugin ${annotate_SRCS} )
