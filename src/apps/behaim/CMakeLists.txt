# SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-3-Clause

add_executable(marble-behaim main.cpp)

target_link_libraries(marble-behaim PUBLIC
    Qt::Quick
    Qt::Widgets
    KF6::I18n
    KF6::CoreAddons
)

if(NOT ANDROID)
    target_link_libraries(marble-behaim PUBLIC KF6::Crash)
endif()

if(NOT MARBLE_NO_WEBKITWIDGETS)
    target_link_libraries(marble-behaim PUBLIC Qt::WebEngineQuick)
    add_definitions(-DMARBLE_WEBKITWIDGETS=1)
endif()

qt_add_qml_module(marble-behaim
    URI org.kde.marble.behaim
    QML_FILES
        MainScreen.qml
        About.qml
    DEPENDENCIES
        org.kde.marble
)

install(TARGETS marble-behaim ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES org.kde.marble.behaim.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.marble.behaim.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES org.kde.marble.behaim.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps)
