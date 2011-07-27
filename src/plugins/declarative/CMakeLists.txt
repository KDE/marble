if( QT_QTDECLARATIVE_FOUND )

PROJECT( MarbleDeclarativePlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/declarative
 ${CMAKE_BINARY_DIR}/src/plugins/declarative
 ${QT_INCLUDE_DIR}
)
INCLUDE( ${QT_USE_FILE} )

set( marbleplugin_SRCS
Coordinate.cpp
DeclarativeMapThemeManager.cpp
MapTheme.cpp
MarbleDeclarativePlugin.cpp
MarbleDeclarativeWidget.cpp
PositionSource.cpp
Tracking.cpp
Routing.cpp
Search.cpp
RouteRequestModel.cpp
Settings.cpp
Activity.cpp
ActivityModel.cpp
RelatedActivities.cpp
)

marble_add_declarative_plugin( MarbleDeclarativePlugin . ${marbleplugin_SRCS})
qt4_automoc(${marbleplugin_SRCS})
target_link_libraries( MarbleDeclarativePlugin ${QT_QTDECLARATIVE_LIBRARY} ${QT_QTWEBKIT_LIBRARY} )
INSTALL( FILES qmldir MarbleSettings.qml DESTINATION ${MARBLE_PLUGIN_PATH}/org/kde/edu/marble )

endif( QT_QTDECLARATIVE_FOUND )
