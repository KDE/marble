//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Dennis Nienhüser <nienhueser@kde.org>
//

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQuick>

#include "declarative/MarbleDeclarativePlugin.h"
#include <MarbleGlobal.h>

using namespace Marble;

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);

#ifdef Q_OS_ANDROID
    MarbleGlobal::Profiles profiles = MarbleGlobal::SmallScreen | MarbleGlobal::HighResolution;
    MarbleGlobal::getInstance()->setProfiles( profiles );
#endif

    MarbleDeclarativePlugin declarativePlugin;
    const char * uri = "org.kde.edu.marble";
    declarativePlugin.registerTypes(uri);

    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:/MainScreen.qml"));

    return app.exec();
}
