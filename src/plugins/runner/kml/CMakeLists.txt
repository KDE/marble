PROJECT( KmlPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_SOURCE_DIR}/src/lib/geodata/handlers/kml
 ${CMAKE_BINARY_DIR}/src/plugins/runner/kml
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( kml_SRCS KmlParser.cpp KmlPlugin.cpp KmlRunner.cpp )

marble_add_plugin( KmlPlugin ${kml_SRCS} )

if(QTONLY)
  if(WIN32 OR APPLE)
    # nothing to do
  else(WIN32 OR APPLE)
    install(FILES marble_kml.desktop DESTINATION ${APPS_INSTALL_DIR})
  endif(WIN32 OR APPLE)
else(QTONLY)
  install(PROGRAMS marble_kml.desktop DESTINATION ${APPS_INSTALL_DIR})
  install(FILES marble_part_kml.desktop DESTINATION ${SERVICES_INSTALL_DIR})
endif(QTONLY)
