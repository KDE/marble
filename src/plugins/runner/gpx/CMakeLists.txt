PROJECT( GpxPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_SOURCE_DIR}/handlers
 ${CMAKE_CURRENT_BINARY_DIR}
)

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
        handlers/GPXcmtTagHandler.cpp
        handlers/GPXlinkTagHandler.cpp
        handlers/GPXurlTagHandler.cpp
        handlers/GPXurlnameTagHandler.cpp
   )

set( gpx_SRCS GpxParser.cpp GpxPlugin.cpp GpxRunner.cpp )

marble_add_plugin( GpxPlugin ${gpx_SRCS}  ${gpx_handlers_SRCS} )

install(FILES marble_gpx.desktop DESTINATION ${APPS_INSTALL_DIR})

if( BUILD_MARBLE_TESTS )
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/tests )
    set( TestTrack_SRCS tests/TestTrack.cpp GpxParser.cpp ${gpx_handlers_SRCS} )
    qt_generate_moc( tests/TestTrack.cpp ${CMAKE_CURRENT_BINARY_DIR}/TestTrack.moc )
    set( TestTrack_SRCS TestTrack.moc ${TestTrack_SRCS} )

    add_executable( TestTrack ${TestTrack_SRCS} )
    target_link_libraries( TestTrack Qt5::Test
                                     ${MARBLEWIDGET} )
    set_target_properties( TestTrack PROPERTIES
                            COMPILE_FLAGS "-DDATA_PATH=\"\\\"${DATA_PATH}\\\"\" -DPLUGIN_PATH=\"\\\"${PLUGIN_PATH}\\\"\"" )
    add_test( TestTrack TestTrack )
endif( BUILD_MARBLE_TESTS )

set(KDE_SKIP_UNINSTALL_TARGET ON CACHE BOOL "KDE uninstall target must be disabled")
find_package(ECM QUIET)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})
if(ECM_FOUND)
  include(KDEInstallDirs)
endif()
macro_optional_find_package(KF5 QUIET COMPONENTS KIO)
if(KF5_FOUND)
# install again once part is fixed to load and show the passed file
#   install(FILES marble_part_gpx.desktop DESTINATION ${SERVICES_INSTALL_DIR})
  install(FILES marble_thumbnail_gpx.desktop DESTINATION ${SERVICES_INSTALL_DIR})
endif()
