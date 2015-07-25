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

#include "MarbleQuickItem.h"
#include "MarblePlacemarkModel.h"
#include "SearchBackend.h"

using namespace Marble;

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);
    qmlRegisterUncreatableType<MarblePlacemarkModel>("Marble", 1, 0, "MarblePlacemarkModel", "MarblePlacemarkModel is not instantiable");
    qmlRegisterType<SearchBarBackend>("Marble", 1, 0, "SearchBackend");
    qmlRegisterType<MarbleQuickItem>("Marble", 1, 0, "MarbleItem");
    QQmlApplicationEngine engine(QUrl("qrc:/MainScreen.qml"));

    return app.exec();
}
