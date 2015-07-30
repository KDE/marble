PROJECT( MarbleDeclarativePlugin )

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_INCLUDE_DIR}
    ${Qt5Qml_INCLUDE_DIRS}
    ${Qt5Quick_INCLUDE_DIRS}
)

set( marbleplugin_SRCS
    Bookmarks.cpp
    Coordinate.cpp
    DeclarativeDataPluginModel.cpp
    DeclarativeDataPlugin.cpp
    DeclarativeDataPluginItem.cpp
    DeclarativeMapThemeManager.cpp
    MarbleDeclarativeObject.cpp
    MarbleDeclarativePlugin.cpp
    MarbleDeclarativeWidget.cpp
    Placemark.cpp
    PositionSource.cpp
    Tracking.cpp
    Routing.cpp
    Navigation.cpp
    OfflineDataModel.cpp
    Search.cpp
    RouteRequestModel.cpp
    Settings.cpp
    MapThemeModel.cpp
    CloudSync.cpp
)

marble_add_project_resources(qmldir)

add_library( MarbleDeclarative ${marbleplugin_SRCS} )
target_link_libraries( MarbleDeclarative
    ${QT_QTCORE_LIBRARY}
    ${QT_QTDBUS_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTXML_LIBRARY}
    ${QT_QTSVG_LIBRARY}
    ${QT_QTNETWORK_LIBRARY}
    ${QT_QTMAIN_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${${_target_name}_LIBS}
    ${MARBLEWIDGET}
)

set_target_properties( MarbleDeclarative PROPERTIES
    INSTALL_RPATH_USE_LINK_PATH TRUE
    SKIP_BUILD_RPATH TRUE
    BUILD_WITH_INSTALL_RPATH TRUE
)
install( TARGETS MarbleDeclarative LIBRARY DESTINATION lib${LIB_SUFFIX} )
