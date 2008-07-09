# unit testing for marble
set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

marble_add_test( PluginManagerTest )
marble_add_test( MarbleWidgetTest )
add_definitions( -DCITIES_PATH="\\\"${CMAKE_CURRENT_SOURCE_DIR}/../../../data/placemarks/cityplacemarks.kml\\\"" )
marble_add_test( KMLTest )
