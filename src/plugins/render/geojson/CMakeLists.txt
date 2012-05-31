PROJECT( GeoJsonPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/geojson
 ${CMAKE_BINARY_DIR}/src/plugins/render/geojson
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( my_SRCS
  GeoJsonPlugin.cpp
 )

# set( geojson_UI EarthquakeConfigWidget.ui )
# 
# qt4_wrap_ui(my_SRCS ${geojson_UI})
# qt4_add_resources(my_SRCS geojson.qrc)
# qt4_automoc( ${my_SRCS} )

marble_add_plugin( GeoJsonPlugin ${my_SRCS} )
