PROJECT( LatLonEditPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
 ${QT_QTDESIGNER_INCLUDE_DIR}
)
IF(QT4_FOUND)
  INCLUDE( ${QT_USE_FILE} )
ELSE()
  INCLUDE_DIRECTORIES(${Qt5Designer_INCLUDE_DIRS})
ENDIF()

#target_link_libraries( marblewidget )

set( latloneditplugin_SRCS LatLonEditPlugin.cpp )

marble_add_designer_plugin( LatLonEditPlugin ${latloneditplugin_SRCS} )
