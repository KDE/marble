// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Dennis Nienhüser <nienhueser@kde.org>
//

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQuick>

#include "declarative/MarbleDeclarativePlugin.h"
#include <MarbleGlobal.h>

using namespace Marble;

#ifdef Q_OS_ANDROID
// Declare symbol of main method as exported as needed by Qt-on-Android,
// where the Dalvik-native QtActivity class needs to find and invoke it
// on loading the "app" module
extern "C" Q_DECL_EXPORT
#endif
int main(int argc, char ** argv)
{
    QApplication app(argc, argv);

#ifdef Q_OS_ANDROID
    MarbleGlobal::Profiles profiles = MarbleGlobal::SmallScreen | MarbleGlobal::HighResolution;
    MarbleGlobal::getInstance()->setProfiles( profiles );
#endif

    MarbleDeclarativePlugin declarativePlugin;
    const char uri[] = "org.kde.marble";
    declarativePlugin.registerTypes(uri);

    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:/MainScreen.qml"));

    return app.exec();
}
