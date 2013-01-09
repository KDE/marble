PROJECT( GpxPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_SOURCE_DIR}/handlers
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( gpx_handlers_SRCS
        handlers/GPXElementDictionary.cpp
        handlers/GPXgpxTagHandler.cpp
        handlers/GPXnameTagHandler.cpp
        handlers/GPXdescTagHandler.cpp
        handlers/GPXtypeTagHandler.cpp
        handlers/GPXtrkTagHandler.cpp
        handlers/GPXtrkptTagHandler.cpp
        handlers/GPXtrksegTagHandler.cpp
        handlers/GPXwptTagHandler.cpp
        handlers/GPXtimeTagHandler.cpp
        handlers/GPXeleTagHandler.cpp
        handlers/GPXextensionsTagHandler.cpp
        handlers/GPXTrackPointExtensionTagHandler.cpp
        handlers/GPXhrTagHandler.cpp
        handlers/GPXrteTagHandler.cpp
        handlers/GPXrteptTagHandler.cpp
        handlers/GPXeleTagHandler.cpp
        handlers/GPXcmtTagHandler.cpp
        handlers/GPXlinkTagHandler.cpp
        handlers/GPXurlTagHandler.cpp
        handlers/GPXurlnameTagHandler.cpp
   )

set( gpx_SRCS GpxParser.cpp GpxPlugin.cpp GpxRunner.cpp )

marble_add_plugin( GpxPlugin ${gpx_SRCS}  ${gpx_handlers_SRCS} )

if(QTONLY)
  if(WIN32 OR APPLE)
    # nothing to do
  else(WIN32 OR APPLE)
    install(FILES marble_gpx.desktop DESTINATION ${APPS_INSTALL_DIR})
  endif(WIN32 OR APPLE)
else(QTONLY)
  install(PROGRAMS marble_gpx.desktop DESTINATION ${APPS_INSTALL_DIR})
  install(FILES marble_part_gpx.desktop DESTINATION ${SERVICES_INSTALL_DIR})
endif(QTONLY)


if( BUILD_MARBLE_TESTS )
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/tests )
    set( TestTrack_SRCS tests/TestTrack.cpp GpxParser.cpp ${gpx_handlers_SRCS} )
    if( QTONLY )
        qt4_generate_moc( tests/TestTrack.cpp ${CMAKE_CURRENT_BINARY_DIR}/TestTrack.moc )
        include_directories( ${CMAKE_CURRENT_BINARY_DIR}/tests )
        set( TestTrack_SRCS TestTrack.moc ${TestTrack_SRCS} )

        add_executable( TestTrack ${TestTrack_SRCS} )
    else( QTONLY )
        kde4_add_executable( TestTrack ${TestTrack_SRCS} )
    endif( QTONLY )
    target_link_libraries( TestTrack ${QT_QTMAIN_LIBRARY}
                                     ${QT_QTCORE_LIBRARY}
                                     ${QT_QTGUI_LIBRARY}
                                     ${QT_QTTEST_LIBRARY}
                                     marblewidget )
    set_target_properties( TestTrack PROPERTIES
                            COMPILE_FLAGS "-DDATA_PATH=\"\\\"${DATA_PATH}\\\"\" -DPLUGIN_PATH=\"\\\"${PLUGIN_PATH}\\\"\"" )
    add_test( TestTrack TestTrack )
endif( BUILD_MARBLE_TESTS )
