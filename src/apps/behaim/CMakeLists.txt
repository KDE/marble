# SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-3-Clause

add_executable(marble-behaim main.cpp)

target_link_libraries(marble-behaim PUBLIC marbledeclarative)

qt_add_qml_module(marble-behaim
    URI org.kde.marble.behaim
    QML_FILES
        MainScreen.qml
        Legend.qml
)

install(TARGETS marble-behaim ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
