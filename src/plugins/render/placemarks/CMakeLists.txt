PROJECT( PlacemarksPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/placemarks
 ${CMAKE_BINARY_DIR}/src/plugins/render/placemarks
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( placemarks_SRCS PlacemarksPlugin.cpp )
add_definitions( -DPLACEMARKS_DATA_PATH="\\\"${CMAKE_CURRENT_SOURCE_DIR}\\\"" )
marble_add_plugin( PlacemarksPlugin ${placemarks_SRCS} )
