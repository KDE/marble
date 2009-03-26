PROJECT(GeoDataPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/geodata
 ${CMAKE_BINARY_DIR}/src/plugins/render/geodata
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( marblegeodata_SRCS GeoDataPlugin.cpp 
                        GeoDataView.cpp )
add_definitions( -DGEODATA_DATA_PATH="\\\"${CMAKE_CURRENT_SOURCE_DIR}\\\"" )
marble_add_plugin( GeoDataPlugin ${marblegeodata_SRCS} )
