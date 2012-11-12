PROJECT( ShpPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
 ${LIBSHP_INCLUDE_DIR}
)

INCLUDE(${QT_USE_FILE})

set( shp_SRCS ShpPlugin.cpp ShpRunner.cpp )

set( ShpPlugin_LIBS ${LIBSHP_LIBRARIES} )

marble_add_plugin( ShpPlugin ${shp_SRCS} )

if(QTONLY)
  if(WIN32 OR APPLE)
    # nothing to do
  else(WIN32 OR APPLE)
    install(FILES marble_shp.desktop DESTINATION ${APPS_INSTALL_DIR})
  endif(WIN32 OR APPLE)
else(QTONLY)
  install(PROGRAMS marble_shp.desktop DESTINATION ${APPS_INSTALL_DIR})
  install(FILES marble_part_shp.desktop DESTINATION ${SERVICES_INSTALL_DIR})
endif(QTONLY)
