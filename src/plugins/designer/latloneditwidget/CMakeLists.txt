PROJECT( LatLonEditPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/designer/latloneditwidget
 ${CMAKE_BINARY_DIR}/src/plugins/designer/latloneditwidget
 ${QT_INCLUDE_DIR}
 ${QT_QTDESIGNER_INCLUDE_DIR}
)
INCLUDE( ${QT_USE_FILE} )

#target_link_libraries( marblewidget )

set( latloneditplugin_SRCS LatLonEditPlugin.cpp )

marble_add_designer_plugin( LatLonEditPlugin ${latloneditplugin_SRCS} )
