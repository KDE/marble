if( QT_QTDECLARATIVE_FOUND )

PROJECT( MarbleDeclarativePlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/declarative
 ${CMAKE_BINARY_DIR}/src/plugins/declarative
 ${QT_INCLUDE_DIR}
)
INCLUDE( ${QT_USE_FILE} )

set( marbleplugin_SRCS
Bookmarks.cpp
Coordinate.cpp
DeclarativeMapThemeManager.cpp
MapTheme.cpp
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
RelatedActivities.cpp
ZoomButtonInterceptor.cpp
)

marble_add_project_resources(qmldir)

marble_add_declarative_plugin( MarbleDeclarativePlugin . ${marbleplugin_SRCS})
qt4_automoc(${marbleplugin_SRCS})
target_link_libraries( MarbleDeclarativePlugin ${QT_QTDECLARATIVE_LIBRARY} ${QT_QTWEBKIT_LIBRARY} )
OPTION(BUILD_HARMATTAN_ZOOMINTERCEPTOR "Add support for intercepting volume keys for zooming")
IF( BUILD_HARMATTAN_ZOOMINTERCEPTOR )
  MESSAGE( STATUS "Building with volume key interception for zooming on harmattan" )
  include(FindPkgConfig)
  pkg_check_modules(libresourceqt1 libresourceqt1)
  ADD_DEFINITIONS( -DHARMATTAN_ZOOMINTERCEPTOR )
  INCLUDE_DIRECTORIES(${libresourceqt1_INCLUDEDIR})
  target_link_libraries( MarbleDeclarativePlugin ${libresourceqt1_LIBRARIES} )
ENDIF()
INSTALL( FILES qmldir MarbleSettings.qml DESTINATION ${MARBLE_QT_IMPORTS_DIR}/org/kde/edu/marble )

endif( QT_QTDECLARATIVE_FOUND )
