PROJECT( ElevationProfileFloatItem)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set(my_SRCS
  ElevationProfileFloatItem.cpp
  ElevationProfilePlotAxis.cpp
  ElevationProfileDataSource.cpp
  ElevationProfileContextMenu.cpp
  ElevationProfileFloatItem.h
  ElevationProfilePlotAxis.h
  ElevationProfileDataSource.h
  ElevationProfileContextMenu.h
)
set(elevationprofile_UI ElevationProfileConfigWidget.ui)

qt_wrap_ui( my_SRCS ${elevationprofile_UI})

marble_add_plugin( ElevationProfileFloatItem ${my_SRCS})
