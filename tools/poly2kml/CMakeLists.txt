project( Poly2KML )
include_directories(
 ${CMAKE_SOURCE_DIR}/src/poly2kml
 ${CMAKE_BINARY_DIR}/src/poly2kml
 ${QT_INCLUDE_DIR}
)
include( ${QT_USE_FILE} )

set( poly2kml_SRC
        ../poly2kml/main.cpp
)

qt4_automoc(${poly2kml_SRC})

add_definitions( -DMAKE_MARBLE_LIB )
add_executable( poly2kml ${poly2kml_SRC} )
target_link_libraries( poly2kml ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTMAIN_LIBRARY} marblewidget )
if (APPLE)
  target_link_libraries (poly2kml ${APP_SERVICES_LIBRARY})
endif (APPLE)

if(WIN32 AND QTONLY)
    install( TARGETS poly2kml RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX} )
else(WIN32 AND QTONLY)
    if (APPLE AND QTONLY)
      # No need for this when installing to a bundle
    else (APPLE AND QTONLY)
      if( NOT QTONLY)
        install( TARGETS poly2kml ${INSTALL_TARGETS_DEFAULT_ARGS} )
      else( NOT QTONLY)
        install( TARGETS poly2kml RUNTIME DESTINATION bin )
      endif(NOT QTONLY)
    endif (APPLE AND QTONLY)
endif(WIN32 AND QTONLY)
