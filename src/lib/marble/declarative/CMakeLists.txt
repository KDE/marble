PROJECT( MarbleDeclarativePlugin )

macro_optional_find_package(Qt5Positioning)
if(Qt5Positioning_FOUND)
    add_definitions(-DHAVE_QT5_POSITIONING)
endif(Qt5Positioning_FOUND)

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_library( marbledeclarative SHARED
    Bookmarks.cpp
    CloudSync.cpp
    Coordinate.cpp
    DeclarativeDataPluginModel.cpp
    DeclarativeDataPlugin.cpp
    DeclarativeDataPluginItem.cpp
    DeclarativeMapThemeManager.cpp
    MapThemeModel.cpp
    MarbleDeclarativeObject.cpp
    MarbleDeclarativePlugin.cpp
    MarbleDeclarativeWidget.cpp
    MarbleQuickItem.cpp
    Placemark.cpp
    PositionSource.cpp
    Search.cpp
    SearchBackend.cpp
    Tracking.cpp
    Routing.cpp
    Navigation.cpp
    OfflineDataModel.cpp
    RouteRequestModel.cpp
    Settings.cpp
)

# needed for marble_declarative_export.h
set_target_properties(marbledeclarative PROPERTIES
  DEFINE_SYMBOL MAKE_MARBLE_DECLARATIVE_LIB
)

marble_add_project_resources(qmldir)

# TODO: use PUBLIC/PRIVATE everywhere instead of LINK_PUBLIC/LINK_PRIVATE
# once SailfishOS has cmake >= 2.8.12

target_link_libraries( marbledeclarative
    LINK_PUBLIC
        Qt5::Quick
        ${MARBLEWIDGET}
    LINK_PRIVATE
        Qt5::Script
)
if(Qt5Positioning_FOUND)
    target_link_libraries( marbledeclarative
        LINK_PRIVATE
            Qt5::Positioning
    )
endif()

set_target_properties( marbledeclarative PROPERTIES
    INSTALL_RPATH_USE_LINK_PATH TRUE
    SKIP_BUILD_RPATH TRUE
    BUILD_WITH_INSTALL_RPATH TRUE
)

if(WIN32)
  install(TARGETS marbledeclarative RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX} ARCHIVE DESTINATION lib)
else()
  install(TARGETS marbledeclarative LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()
