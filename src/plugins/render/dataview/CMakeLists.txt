PROJECT( DataViewPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/dataview
 ${CMAKE_BINARY_DIR}/src/plugins/render/dataview
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( dataview_SRCS DataViewPlugin.cpp GeoDataDebugModel.cpp)
#add_definitions( -DPLACEMARKS_DATA_PATH="\\\"${CMAKE_CURRENT_SOURCE_DIR}\\\"" )
marble_add_plugin( DataViewPlugin ${dataview_SRCS} )
