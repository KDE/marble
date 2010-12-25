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
qt4_add_resources(my_SRCS earthquake.qrc)
qt4_automoc( ${my_SRCS} )

marble_add_plugin( EarthquakePlugin ${my_SRCS} )
