# SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-3-Clause

add_executable(marble-maps main.cpp MarbleMaps.cpp TextToSpeechClient.cpp MarbleMaps.h TextToSpeechClient.h)

target_link_libraries(marble-maps PUBLIC
    marbledeclarative
    KF6::CoreAddons
    KF6::I18n
    KF6::Crash
)

qt_add_qml_module(marble-maps
    URI org.kde.marble.maps
    QML_FILES
        Completion.qml
        MainScreen.qml
        SearchResults.qml
        SearchField.qml
        Search.qml
        IconText.qml
        PositionButton.qml
        BoxedText.qml
        ImageButton.qml
        WaypointImage.qml
        RoutingManager.qml
        PlacemarkDialog.qml
        SettingsDialog.qml
        DeveloperDialog.qml
        ProfileSelectorMenu.qml
        FloatingMenuButton.qml
        RouteEditor.qml
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
)

install(TARGETS marble-maps RUNTIME DESTINATION bin)
install(PROGRAMS org.kde.marble.maps.desktop DESTINATION ${KDE_INSTALL_APPDIR})
