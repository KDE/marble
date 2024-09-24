// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Dennis Nienhüser <nienhueser@kde.org>
//

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQuick>

#if !MARBLE_WEBKITWIDGETS
#include <QtWebEngineQuick>
#endif

#ifdef Q_OS_ANDROID
Q_DECL_EXPORT
#endif
int main(int argc, char **argv)
{
#if !MARBLE_WEBKITWIDGETS
    QtWebEngineQuick::initialize();
#endif
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.loadFromModule("org.kde.marble.behaim", "MainScreen");

    return app.exec();
}
