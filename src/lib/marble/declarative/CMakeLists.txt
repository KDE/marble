PROJECT( MarbleDeclarativePlugin )

find_package(Qt5 ${REQUIRED_QT_VERSION} COMPONENTS Positioning)
if(Qt5Positioning_FOUND)
    add_definitions(-DHAVE_QT5_POSITIONING)
endif(Qt5Positioning_FOUND)

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_library( marbledeclarative SHARED
    Bookmarks.cpp
    Coordinate.cpp
    DeclarativeMapThemeManager.cpp
    MapThemeModel.cpp
    MarbleDeclarativeObject.cpp
    MarbleDeclarativePlugin.cpp
    MarbleQuickItem.cpp
    Placemark.cpp
    PositionSource.cpp
    SearchBackend.cpp
    Tracking.cpp
    Routing.cpp
    Navigation.cpp
    OfflineDataModel.cpp
    RouteRequestModel.cpp
    Settings.cpp
    RouteRelationModel.cpp
)

generate_export_header(marbledeclarative BASE_NAME marble_declarative)
target_include_directories(marbledeclarative PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

marble_add_project_resources(qmldir)

# TODO: use PUBLIC/PRIVATE everywhere instead of LINK_PUBLIC/LINK_PRIVATE
# once SailfishOS has cmake >= 2.8.12

target_link_libraries( marbledeclarative
    LINK_PUBLIC
        marblewidget
        Qt5::Quick
)
if(Qt5Positioning_FOUND)
    target_link_libraries( marbledeclarative
        LINK_PRIVATE
            Qt5::Positioning
    )
endif()

if(WIN32)
  install(TARGETS marbledeclarative RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX} ARCHIVE DESTINATION lib)
else()
  install(TARGETS marbledeclarative LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()
