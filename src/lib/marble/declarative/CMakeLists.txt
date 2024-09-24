# SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-3-Clause

ecm_add_qml_module(marbledeclarative
    URI "org.kde.marble"
    VERSION 1.0
    GENERATE_PLUGIN_SOURCE
)

find_package(Qt6 ${REQUIRED_QT_VERSION} OPTIONAL_COMPONENTS Positioning)
if(Qt6Positioning_FOUND)
    add_definitions(-DHAVE_QT6_POSITIONING)     # PORT_QT6
endif(Qt6Positioning_FOUND)

target_sources(marbledeclarative PRIVATE
    Bookmarks.cpp
    Coordinate.cpp
    GeoItem.cpp
    GeoPolyline.cpp
    DeclarativeMapThemeManager.cpp
    MapTheme.cpp
    MapThemeModel.cpp
    MarbleDeclarativeObject.cpp
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

target_link_libraries(marbledeclarative PUBLIC
    marblewidget
    Qt6::Quick
)

if(Qt6Positioning_FOUND)
    target_link_libraries(marbledeclarative PRIVATE Qt6::Positioning)
endif()

ecm_finalize_qml_module(marbledeclarative DESTINATION ${KDE_INSTALL_QMLDIR})
