// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Adam Dabrowski <adamdbrw@gmail.com>
//

#include <QApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.loadFromModule("org.kde.marble.quick", "Main");

    return app.exec();
}
