PROJECT( GpxPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_SOURCE_DIR}/handlers
 ${CMAKE_CURRENT_BINARY_DIR}
)

set(gpx_handlers_SRCS
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

        handlers/GPXElementDictionary.h
        handlers/GPXgpxTagHandler.h
        handlers/GPXnameTagHandler.h
        handlers/GPXdescTagHandler.h
        handlers/GPXtypeTagHandler.h
        handlers/GPXtrkTagHandler.h
        handlers/GPXtrkptTagHandler.h
        handlers/GPXtrksegTagHandler.h
        handlers/GPXwptTagHandler.h
        handlers/GPXtimeTagHandler.h
        handlers/GPXeleTagHandler.h
        handlers/GPXextensionsTagHandler.h
        handlers/GPXTrackPointExtensionTagHandler.h
        handlers/GPXhrTagHandler.h
        handlers/GPXrteTagHandler.h
        handlers/GPXrteptTagHandler.h
        handlers/GPXcmtTagHandler.h
        handlers/GPXlinkTagHandler.h
        handlers/GPXurlTagHandler.h
        handlers/GPXurlnameTagHandler.h

)

set(gpx_SRCS GpxParser.cpp GpxPlugin.cpp GpxRunner.cpp GpxPlugin.h GpxRunner.h)

marble_add_plugin( GpxPlugin ${gpx_SRCS}  ${gpx_handlers_SRCS})

if(BUILD_TESTING)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/tests)
    set(TestTrack_SRCS tests/TestTrack.cpp GpxParser.cpp ${gpx_handlers_SRCS})
    qt_generate_moc( tests/TestTrack.cpp ${CMAKE_CURRENT_BINARY_DIR}/TestTrack.moc)
    set(TestTrack_SRCS TestTrack.moc ${TestTrack_SRCS})

    add_executable(TestTrack ${TestTrack_SRCS})
    target_link_libraries(TestTrack Qt6::Test
                                     marblewidget)
    set_target_properties( TestTrack PROPERTIES
                            COMPILE_FLAGS "-DDATA_PATH=\"\\\"${DATA_PATH}\\\"\" -DPLUGIN_PATH=\"\\\"${PLUGIN_PATH}\\\"\"")
		    add_test( NAME TestTrack COMMAND TestTrack)
endif()

macro_optional_find_package(KF6 ${REQUIRED_KF6_MIN_VERSION} QUIET COMPONENTS KIO)
if(NOT KF6_FOUND)
    return()
endif()

# register marble part as handler
# install again once part is fixed to load and show the passed file
# install(FILES marble_part_gpx.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})

if(NOT WIN32 AND NOT APPLE)
    # register marble app as handler
    install(FILES marble_gpx.desktop DESTINATION ${KDE_INSTALL_APPDIR})
endif()
