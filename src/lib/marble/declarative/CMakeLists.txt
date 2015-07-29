PROJECT( MarbleDeclarativePlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE_DIRECTORIES(
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

marble_add_declarative_plugin( MarbleDeclarativePlugin . ${marbleplugin_SRCS})
target_link_libraries( MarbleDeclarativePlugin ${QT_QTDECLARATIVE_LIBRARY} ${QT_QTWEBKIT_LIBRARY} )
INSTALL( FILES qmldir MarbleSettings.qml DESTINATION ${MARBLE_QT_IMPORTS_DIR}/org/kde/edu/marble )
