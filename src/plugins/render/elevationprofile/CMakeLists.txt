PROJECT( ElevationProfileFloatItem )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/elevationprofile
 ${CMAKE_CURRENT_BUILD_DIR}/src/plugins/render/elevationprofile
 ${CMAKE_BINARY_DIR}/src/plugins/render/elevationprofile
 ${QT_INCLUDE_DIR}
)
INCLUDE( ${QT_USE_FILE} )

set( my_SRCS
  ElevationProfileFloatItem.cpp
  ElevationProfilePlotAxis.cpp
 )
set( elevationprofile_UI ElevationProfileConfigWidget.ui )

qt4_wrap_ui( my_SRCS ${elevationprofile_UI})
qt4_add_resources( my_SRCS elevationprofile.qrc)
qt4_automoc( ${my_SRCS} )

marble_add_plugin( ElevationProfileFloatItem ${my_SRCS} )
