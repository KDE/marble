# SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-3-Clause

add_executable(marble-behaim main.cpp)

target_link_libraries(marble-behaim PUBLIC Qt::Quick Qt::Widgets)

if(NOT MARBLE_NO_WEBKITWIDGETS)
    target_link_libraries(marble-behaim PUBLIC Qt::WebEngineQuick)
    add_definitions(-DMARBLE_WEBKITWIDGETS=1)
endif()

qt_add_qml_module(marble-behaim
    URI org.kde.marble.behaim
    QML_FILES
        MainScreen.qml
        Legend.qml
    DEPENDENCIES
        org.kde.marble
)

install(TARGETS marble-behaim ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
