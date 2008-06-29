# unit testing for marble
if( WIN32 )
    set( DATA_PATH ${CMAKE_INSTALL_PREFIX}/${MARBLE_DATA_PATH} )
    set( PLUGIN_PATH ${CMAKE_INSTALL_PREFIX}/${MARBLE_PLUGIN_PATH} )
else( WIN32 )
    set( DATA_PATH ${MARBLE_DATA_PATH} )
    set( PLUGIN_PATH ${MARBLE_PLUGIN_PATH} )
endif( WIN32 )

set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

macro( add_marble_test TEST_NAME )
    set( ${TEST_NAME}_SRCS ${TEST_NAME}.cpp )
    if( QTONLY )
        qt4_generate_moc( ${TEST_NAME}.cpp ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.moc )
        include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
        set( ${TEST_NAME}_SRCS ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.moc ${${TEST_NAME}_SRCS} )
      
        add_executable( ${TEST_NAME} ${${TEST_NAME}_SRCS} )
    else( QTONLY )
        kde4_add_executable( ${TEST_NAME} ${${TEST_NAME}_SRCS} )
    endif( QTONLY )
    target_link_libraries( ${TEST_NAME} ${QT_QTMAIN_LIBRARY}
                                        ${QT_QTCORE_LIBRARY} 
                                        ${QT_QTGUI_LIBRARY} 
                                        ${QT_QTTEST_LIBRARY} 
                                        marblewidget )
    set_target_properties( ${TEST_NAME} PROPERTIES 
                           COMPILE_FLAGS "-DDATA_PATH=\"\\\"${DATA_PATH}\\\"\" -DPLUGIN_PATH=\"\\\"${PLUGIN_PATH}\\\"\"" )
    add_test( ${TEST_NAME} ${TEST_NAME} )
endmacro( add_marble_test TEST_NAME )

add_marble_test( PluginManagerTest )
add_marble_test( MarbleWidgetTest )
