PROJECT( MarbleWidgetPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/designer/marblewidget
 ${CMAKE_BINARY_DIR}/src/plugins/designer/marblewidget
 ${QT_INCLUDE_DIR}
 ${QT_QTDESIGNER_INCLUDE_DIR}
)
INCLUDE( ${QT_USE_FILE} )

set( marbleplugin_SRCS MarbleWidgetPlugin.cpp )
set( marbleplugin_HDRS MarbleWidgetPlugin.h )

qt4_add_resources( marbleplugin_SRCS ../../../marble.qrc )

if( QTONLY )
  qt4_automoc( ${marbleplugin_SRCS} )
  add_library( MarbleWidgetPlugin SHARED ${marbleplugin_SRCS} )
  target_link_libraries( MarbleWidgetPlugin ${QT_QTCORE_LIBRARY} 
                                            ${QT_QTDBUS_LIBRARY} 
                                            ${QT_QTGUI_LIBRARY} 
                                            ${QT_QTXML_LIBRARY} 
                                            ${QT_QTSVG_LIBRARY} 
                                            ${QT_QTNETWORK_LIBRARY} 
                                            ${QT_QTMAIN_LIBRARY} 
                                            marblewidget )
  install( TARGETS MarbleWidgetPlugin DESTINATION ${QT_PLUGINS_DIR}/designer )
else( QTONLY )
  kde4_add_library( MarbleWidgetPlugin SHARED ${marbleplugin_SRCS} )
  target_link_libraries( MarbleWidgetPlugin ${QT_QTCORE_LIBRARY} 
                                            ${QT_QTDBUS_LIBRARY}
                                            ${QT_QTGUI_LIBRARY} 
                                            ${QT_QTXML_LIBRARY} 
                                            ${QT_QTSVG_LIBRARY} 
                                            ${QT_QTNETWORK_LIBRARY} 
                                            ${KDE4_KDECORE_LIBRARY} 
                                            ${KDE4_KDEUI_LIBRARY} 
                                            ${KDE4_KIO_LIBRARY} 
                                            ${QT_QTMAIN_LIBRARY} 
                                            marblewidget )
  install( TARGETS MarbleWidgetPlugin DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/designer )
endif( QTONLY )


