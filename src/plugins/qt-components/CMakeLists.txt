PROJECT( QtComponentsPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/qt-components
 ${CMAKE_BINARY_DIR}/src/plugins/qt-components
 ${QT_INCLUDE_DIR}
)

set( my_SRCS
QtComponentsPlugin.cpp
)

QT4_ADD_RESOURCES(my_SRCS qtcomponents.qrc)

MARBLE_ADD_DECLARATIVE_PLUGIN( QtComponentsPlugin qtcomponents ${my_SRCS} )
QT4_AUTOMOC(${my_SRCS})
TARGET_LINK_LIBRARIES( QtComponentsPlugin ${QT_QTDECLARATIVE_LIBRARY} ${QT_QTWEBKIT_LIBRARY} )
INSTALL( FILES qmldir DESTINATION ${QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES MainWidget.qml DESTINATION ${QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES ConfigurationActivityPage.qml DESTINATION ${QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES ConnectionSettingsPage.qml DESTINATION ${QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES MapThemeSelectionPage.qml DESTINATION ${QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES NavigationSettingsPage.qml DESTINATION ${QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES ViewSettingsPage.qml DESTINATION ${QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES SearchBar.qml DESTINATION ${QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES RouteRequestView.qml DESTINATION ${QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES WaypointView.qml DESTINATION ${QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES RoutingDialog.qml DESTINATION ${QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES SearchResultSelectionDialog.qml DESTINATION ${QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES ActivitySelectionView.qml DESTINATION ${QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES VirtualGlobeActivityPage.qml DESTINATION ${QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES SearchActivityPage.qml DESTINATION ${QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES GeocachingActivityPage.qml DESTINATION ${QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES GuidanceActivityPage.qml DESTINATION ${QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES WeatherActivityPage.qml DESTINATION ${QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
