PROJECT( EarthquakePlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/earthquake
 ${CMAKE_BINARY_DIR}/src/plugins/render/earthquake
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( my_SRCS
  EarthquakePlugin.cpp
  EarthquakeModel.cpp
  EarthquakeItem.cpp
 )
set( earthquake_UI EarthquakeConfigWidget.ui )

qt4_wrap_ui(my_SRCS ${earthquake_UI})
qt4_add_resources(my_SRCS earthquake.qrc)
qt4_automoc( ${my_SRCS} )

marble_add_plugin( EarthquakePlugin ${my_SRCS} )
