PROJECT( KmlPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${CMAKE_SOURCE_DIR}/src/lib/marble/geodata/handlers/kml
 ${QT_INCLUDE_DIR}
)
if( QT4_FOUND )
  INCLUDE(${QT_USE_FILE})
endif()

set( kml_SRCS KmlDocument.cpp KmlParser.cpp KmlPlugin.cpp KmlRunner.cpp )

macro_optional_find_package( quazip )
marble_set_package_properties( quazip PROPERTIES DESCRIPTION "reading and writing of ZIP archives" )
marble_set_package_properties( quazip PROPERTIES URL "http://quazip.sourceforge.net/" )
marble_set_package_properties( quazip PROPERTIES TYPE OPTIONAL PURPOSE "reading and displaying .kmz files" )
if( QUAZIP_FOUND )
  add_definitions( -DMARBLE_HAVE_QUAZIP )
  set( kml_SRCS ${kml_SRCS} KmzHandler.cpp )
  set( KmlPlugin_LIBS ${QUAZIP_LIBRARIES} )
endif( QUAZIP_FOUND )

marble_add_plugin( KmlPlugin ${kml_SRCS} )

if(QTONLY)
  if(WIN32 OR APPLE)
    # nothing to do
  else(WIN32 OR APPLE)
    install(FILES marble_kml.desktop DESTINATION ${APPS_INSTALL_DIR})
    if( QUAZIP_FOUND )
      install(FILES marble_kmz.desktop DESTINATION ${APPS_INSTALL_DIR})
    endif( QUAZIP_FOUND )
  endif(WIN32 OR APPLE)
else(QTONLY)
  install(PROGRAMS marble_kml.desktop DESTINATION ${APPS_INSTALL_DIR})
  install(FILES marble_part_kml.desktop DESTINATION ${SERVICES_INSTALL_DIR})
  if( QUAZIP_FOUND )
    install(PROGRAMS marble_kmz.desktop DESTINATION ${APPS_INSTALL_DIR})
    install(FILES marble_part_kmz.desktop DESTINATION ${SERVICES_INSTALL_DIR})
  endif( QUAZIP_FOUND )
endif(QTONLY)
