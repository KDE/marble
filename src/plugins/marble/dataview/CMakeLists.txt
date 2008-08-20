PROJECT(MarbleDataViewPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/marble/dataview
 ${CMAKE_BINARY_DIR}/src/plugins/marble/dataview
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( marbledataview_SRCS MarbleDataViewPlugin.cpp MarbleGeoDataDebugModel.cpp)
#add_definitions( -DPLACEMARKS_DATA_PATH="\\\"${CMAKE_CURRENT_SOURCE_DIR}\\\"" )
marble_add_plugin( MarbleDataViewPlugin ${marbledataview_SRCS} )