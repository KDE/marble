//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Gábor Péterffy <peterffy95@gmail.com>
//

#include <QApplication>
#include <QQmlApplicationEngine>

#include "declarative/MarbleDeclarativePlugin.h"
#include "MarbleMaps.h"

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);

    MarbleDeclarativePlugin declarativePlugin;
    const char * uri = "org.kde.edu.marble";
    declarativePlugin.registerTypes(uri);
    qmlRegisterType<Marble::MarbleMaps>(uri, 0, 20, "MarbleMaps");

    QQmlApplicationEngine engine(QUrl("qrc:/MainScreen.qml"));

    return app.exec();
}
