# SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-3-Clause

add_executable(marbleQuick2 main.cpp)

target_link_libraries(marbleQuick2 PUBLIC Qt::Quick Qt::Widgets)

ecm_add_qml_module(marbleQuick2
    URI org.kde.marble.quick
    QML_FILES
        Main.qml
    DEPENDENCIES
        org.kde.marble
)

install(TARGETS marbleQuick2 RUNTIME DESTINATION ${MARBLE_EXAMPLES_INSTALL_DIR}/marbleQuick2)
