PROJECT( LatLonEditPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${Qt5Designer_INCLUDE_DIRS}
)

#target_link_libraries( ${MARBLEWIDGET} )

set( latloneditplugin_SRCS LatLonEditPlugin.cpp )

marble_add_designer_plugin( LatLonEditPlugin ${latloneditplugin_SRCS} )
