PROJECT( AnnotatePlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${CMAKE_SOURCE_DIR}/src/lib/marble/geodata/handlers/kml
 ${CMAKE_SOURCE_DIR}/src/lib/marble/layers
)

set( annotate_UI
  EditGroundOverlayDialog.ui
  EditPolygonDialog.ui
  EditPolylineDialog.ui
  DownloadOsmDialog.ui
)

set( annotate_SRCS
  AnnotatePlugin.cpp
  AreaAnnotation.cpp
  DownloadOsmDialog.cpp
  EditGroundOverlayDialog.cpp
  EditPolygonDialog.cpp
  NodeItemDelegate.cpp
  EditPolylineDialog.cpp
  GroundOverlayFrame.cpp
  MergingPolygonNodesAnimation.cpp
  MergingPolylineNodesAnimation.cpp
  PlacemarkTextAnnotation.cpp
  PolylineAnnotation.cpp
  PolylineNode.cpp
  SceneGraphicsItem.cpp
  SceneGraphicsTypes.cpp
  NodeModel.cpp
)

qt_wrap_ui(annotate_SRCS ${annotate_UI})

marble_add_plugin( AnnotatePlugin ${annotate_SRCS} )
