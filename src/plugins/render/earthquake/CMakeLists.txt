PROJECT( EarthquakePlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( my_SRCS
  EarthquakePlugin.cpp
  EarthquakeModel.cpp
  EarthquakeItem.cpp
  EarthquakePlugin.h
  EarthquakeModel.h
  EarthquakeItem.h
 )
set( earthquake_UI EarthquakeConfigWidget.ui )

qt_wrap_ui(my_SRCS ${earthquake_UI})
qt_add_resources(my_SRCS earthquake.qrc)

set(EarthquakePlugin_LIBS Qt6::Svg)

marble_add_plugin( EarthquakePlugin ${my_SRCS} )
