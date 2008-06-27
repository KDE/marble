# unit testing for marble

macro( add_marble_test TEST_NAME )
    set( ${TEST_NAME}_SRCS ${TEST_NAME}.cpp )
    if( QTONLY )
        qt4_generate_moc( ${TEST_NAME}.cpp ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.moc )
        include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
        set( ${TEST_NAME}_SRCS ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.moc ${${TEST_NAME}_SRCS} )
      
        add_executable( ${TEST_NAME} ${${TEST_NAME}_SRCS} )
        target_link_libraries( ${TEST_NAME} ${QT_QTMAIN_LIBRARY} 
 			       		    ${QT_QTCORE_LIBRARY} 
 					    ${QT_QTGUI_LIBRARY} 
 					    ${QT_QTTEST_LIBRARY} 
 					    marblewidget )
    else( QTONLY )
        kde4_add_executable( ${TEST_NAME} ${${TEST_NAME}_SRCS} )
        target_link_libraries( ${TEST_NAME}
			       ${QT_QTMAIN_LIBRARY} 
 			       ${QT_QTCORE_LIBRARY} 
 			       ${QT_QTGUI_LIBRARY} 
			       ${QT_QTTEST_LIBRARY}
			        marblewidget )
    endif( QTONLY )
    add_test( ${TEST_NAME} ${TEST_NAME} )
endmacro( add_marble_test TEST_NAME )

add_marble_test( PluginManagerTest )
add_marble_test( MarbleWidgetTest )
