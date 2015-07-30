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

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);

    MarbleDeclarativePlugin declarativePlugin;
    declarativePlugin.registerTypes("org.kde.edu.marble");

    QQmlApplicationEngine engine(QUrl("qrc:/MainScreen.qml"));

    return app.exec();
}
