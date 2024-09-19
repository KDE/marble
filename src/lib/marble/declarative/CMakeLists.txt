PROJECT( MarbleDeclarativePlugin )

find_package(Qt6 ${REQUIRED_QT_VERSION} OPTIONAL_COMPONENTS Positioning)
if(Qt6Positioning_FOUND)
    add_definitions(-DHAVE_QT6_POSITIONING)     # PORT_QT6
endif(Qt6Positioning_FOUND)

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_library( marbledeclarative SHARED
    Bookmarks.cpp
    Coordinate.cpp
    GeoItem.cpp
    GeoPolyline.cpp
    DeclarativeMapThemeManager.cpp
    MapTheme.cpp
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

    Bookmarks.h
    Coordinate.h
    GeoItem.h
    GeoPolyline.h
    DeclarativeMapThemeManager.h
    MapTheme.h
    MapThemeModel.h
    MarbleDeclarativeObject.h
    MarbleDeclarativePlugin.h
    MarbleQuickItem.h
    Placemark.h
    PositionSource.h
    SearchBackend.h
    Tracking.h
    Routing.h
    Navigation.h
    OfflineDataModel.h
    RouteRequestModel.h
    Settings.h
    RouteRelationModel.h
)

generate_export_header(marbledeclarative BASE_NAME marble_declarative)
target_include_directories(marbledeclarative PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

marble_add_project_resources(qmldir)

target_link_libraries( marbledeclarative
    PUBLIC
        marblewidget
        Qt6::Quick
)
if(Qt6Positioning_FOUND)
    target_link_libraries( marbledeclarative
        PRIVATE
            Qt6::Positioning
    )
endif()

if(WIN32)
  install(TARGETS marbledeclarative RUNTIME DESTINATION . ARCHIVE DESTINATION lib)
else()
  install(TARGETS marbledeclarative LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()
