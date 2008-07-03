# unit testing for marble
if( WIN32 )
    set( DATA_PATH ${CMAKE_INSTALL_PREFIX}/${MARBLE_DATA_PATH} )
    set( PLUGIN_PATH ${CMAKE_INSTALL_PREFIX}/${MARBLE_PLUGIN_PATH} )
else( WIN32 )
    set( DATA_PATH ${MARBLE_DATA_PATH} )
    set( PLUGIN_PATH ${MARBLE_PLUGIN_PATH} )
endif( WIN32 )

set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

marble_add_test( PluginManagerTest )
marble_add_test( MarbleWidgetTest )
