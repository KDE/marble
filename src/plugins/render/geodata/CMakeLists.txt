PROJECT(MarbleGeoDataPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/marble/geodata
 ${CMAKE_BINARY_DIR}/src/plugins/marble/geodata
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( marblegeodata_SRCS MarbleGeoDataPlugin.cpp 
                        MarbleGeoDataView.cpp )
add_definitions( -DGEODATA_DATA_PATH="\\\"${CMAKE_CURRENT_SOURCE_DIR}\\\"" )
marble_add_plugin( MarbleGeoDataPlugin ${marblegeodata_SRCS} )