PROJECT( ElevationProfileFloatItem )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
if( QT4_FOUND )
  INCLUDE( ${QT_USE_FILE} )
endif()

set( my_SRCS
  ElevationProfileFloatItem.cpp
  ElevationProfilePlotAxis.cpp
 )
set( elevationprofile_UI ElevationProfileConfigWidget.ui )

qt_wrap_ui( my_SRCS ${elevationprofile_UI})
marble_qt4_automoc( ${my_SRCS} )

marble_add_plugin( ElevationProfileFloatItem ${my_SRCS} )
