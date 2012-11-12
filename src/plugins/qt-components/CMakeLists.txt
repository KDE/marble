PROJECT( QtComponentsPlugin )

SET(MARBLE_PLASMA_COMPONENTS FALSE)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)

set( my_SRCS
QtComponentsPlugin.cpp
)

QT4_ADD_RESOURCES(my_SRCS qtcomponents.qrc)

FILE (GLOB QML_FILES *.qml)
marble_add_project_resources(${QML_FILES} qmldir)

MARBLE_ADD_DECLARATIVE_PLUGIN( QtComponentsPlugin qtcomponents ${my_SRCS} )
QT4_AUTOMOC(${my_SRCS})
TARGET_LINK_LIBRARIES( QtComponentsPlugin ${QT_QTDECLARATIVE_LIBRARY} ${QT_QTWEBKIT_LIBRARY} )
INSTALL( FILES qmldir DESTINATION ${MARBLE_QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES SearchField.qml DESTINATION ${MARBLE_QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES RouteEditor.qml DESTINATION ${MARBLE_QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES PlacemarkEditor.qml DESTINATION ${MARBLE_QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES ViaPointEditor.qml DESTINATION ${MARBLE_QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES FileDialog.qml DESTINATION ${MARBLE_QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES FileOpenDialog.qml DESTINATION ${MARBLE_QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
INSTALL( FILES FileSaveDialog.qml DESTINATION ${MARBLE_QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )

IF( MARBLE_PLASMA_COMPONENTS )
  INSTALL( FILES plasma/MarbleToolIcon.qml DESTINATION ${MARBLE_QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
  INSTALL( FILES plasma/MarbleMenuLayout.qml DESTINATION ${MARBLE_QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
  INSTALL( FILES plasma/MenuItemSwitch.qml DESTINATION ${MARBLE_QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
ELSE()
  INSTALL( FILES harmattan/MarbleToolIcon.qml DESTINATION ${MARBLE_QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
  INSTALL( FILES harmattan/MarbleMenuLayout.qml DESTINATION ${MARBLE_QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
  INSTALL( FILES harmattan/MenuItemSwitch.qml DESTINATION ${MARBLE_QT_IMPORTS_DIR}/org/kde/edu/marble/qtcomponents )
ENDIF()
