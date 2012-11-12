PROJECT(PositionMarker)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( positionmarker_SRCS PositionMarker.cpp )
set( positionmarker_UI PositionMarkerConfigWidget.ui )
 
qt4_wrap_ui(positionmarker_SRCS ${positionmarker_UI})

marble_add_plugin( PositionMarker ${positionmarker_SRCS} )
