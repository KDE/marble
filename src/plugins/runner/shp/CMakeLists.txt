PROJECT( ShpPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/runner/shp
 ${CMAKE_BINARY_DIR}/src/plugins/runner/shp
 ${QT_INCLUDE_DIR}
 ${LIBSHP_INCLUDE_DIR}
)

INCLUDE(${QT_USE_FILE})

set( shp_SRCS ShpPlugin.cpp ShpRunner.cpp )

set( ShpPlugin_LIBS ${LIBSHP_LIBRARIES} )

marble_add_plugin( ShpPlugin ${shp_SRCS} )

if(QTONLY)
  if(WIN32)
  else(WIN32)
    if (APPLE)
    else (APPLE)
      install( FILES marble_shp.desktop DESTINATION ${APPS_INSTALL_DIR})
    endif (APPLE)
  endif(WIN32)
else(QTONLY)
  install(PROGRAMS marble_shp.desktop DESTINATION ${APPS_INSTALL_DIR})
  install(FILES marble_part_shp.desktop DESTINATION ${SERVICES_INSTALL_DIR})
endif(QTONLY)
