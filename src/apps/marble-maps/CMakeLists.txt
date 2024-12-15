# SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-3-Clause

add_executable(marble-maps main.cpp MarbleMaps.cpp TextToSpeechClient.cpp MarbleMaps.h TextToSpeechClient.h)

kconfig_add_kcfg_files(marble-maps GENERATE_MOC config.kcfgc)

target_link_libraries(marble-maps PUBLIC
    Qt::Quick
    Qt::Widgets
    KF6::CoreAddons
    KF6::I18n
    KF6::ConfigGui
)

if(NOT MARBLE_NO_WEBKITWIDGETS)
    target_link_libraries(marble-maps PUBLIC Qt::WebEngineQuick)
    add_definitions(-DMARBLE_WEBKITWIDGETS=1)
endif()

if(NOT ANDROID)
    target_link_libraries(marble-maps PUBLIC KF6::Crash)
endif()

ecm_add_qml_module(marble-maps
    URI org.kde.marble.maps
    QML_FILES
        MainScreen.qml
        IconText.qml
        PositionButton.qml
        BoxedText.qml
        WaypointImage.qml
        RoutingManager.qml
        PlacemarkDialog.qml
        SettingsDialog.qml
        DeveloperDialog.qml
        ProfileSelectorMenu.qml
        FloatingMenuButton.qml
        RouteEditorPage.qml
        RoutesItem.qml
        MenuIcon.qml
        NavigationInfoBar.qml
        NavigationManager.qml
        PositionMarker.qml
        Waypoint.qml
        CurrentPosition.qml
        Options.qml
        Bookmarks.qml
        MarbleScrollBar.qml
        ThemeConfigPage.qml
        SearchResultView.qml
    RESOURCES
        images/directions.svg
        images/transport-mode-bus.svg
        images/transport-mode-car.svg
        images/transport-mode-walk.svg
        images/transport-mode-bike.svg
        images/transport-mode-horse-riding.svg
        images/transport-mode-inline-skater.svg
        images/transport-mode-train.svg
        images/transport-mode-tram.svg
        images/transport-mode-subway.svg
        images/gps_fixed.png
        images/gps_not_fixed.png
        images/backdrop.png
    DEPENDENCIES
        org.kde.marble
        org.kde.kirigami
        org.kde.kirigamiaddons.formcard
        org.kde.kirigamiaddons.delegates
        org.kde.kirigamiaddons.components
)

install(TARGETS marble-maps RUNTIME DESTINATION bin)
install(FILES org.kde.marble.maps.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.marble.maps.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES org.kde.marble.maps.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps)
