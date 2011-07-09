PROJECT( QtComponentsPlugin )

INCLUDE_DIRECTORIES(
 ${QT_INCLUDE_DIR}
)

QT4_ADD_RESOURCES(my_SRCS qtcomponents.qrc)

MARBLE_ADD_PLUGIN( QtComponentsPlugin ${my_SRCS} )
INSTALL( FILES qmldir DESTINATION ${MARBLE_PLUGIN_PATH}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES MainWidget.qml DESTINATION ${MARBLE_PLUGIN_PATH}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES SettingsPage.qml DESTINATION ${MARBLE_PLUGIN_PATH}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES SettingsListPage.qml DESTINATION ${MARBLE_PLUGIN_PATH}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES MapThemeSelectionPage.qml DESTINATION ${MARBLE_PLUGIN_PATH}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES NavigationSettingsPage.qml DESTINATION ${MARBLE_PLUGIN_PATH}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES ViewSettingsPage.qml DESTINATION ${MARBLE_PLUGIN_PATH}/org/kde/edu/marble/qtcomponents )