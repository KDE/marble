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
#include <QtQuick>

#include "declarative/MarbleDeclarativePlugin.h"
#include <MarbleGlobal.h>
#include "MarbleMaps.h"
#include "TextToSpeechClient.h"

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
    app.setApplicationName( "Marble Maps" );
    app.setOrganizationName( "KDE" );
    app.setOrganizationDomain( "kde.org" );
#if QT_VERSION >= 0x050700
    app.setDesktopFileName(QStringLiteral("org.kde.marble.maps"));
#endif

#ifdef Q_OS_ANDROID
    MarbleGlobal::Profiles profiles = MarbleGlobal::SmallScreen | MarbleGlobal::HighResolution;
    MarbleGlobal::getInstance()->setProfiles( profiles );
#endif

    MarbleDeclarativePlugin declarativePlugin;
    const char uri[] = "org.kde.marble";
    declarativePlugin.registerTypes(uri);
    qmlRegisterType<MarbleMaps>(uri, 0, 20, "MarbleMaps");

    QQmlApplicationEngine engine;
    TextToSpeechClient * tts = new TextToSpeechClient(&engine);
    engine.rootContext()->setContextProperty("textToSpeechClient", tts);
    engine.load(QUrl("qrc:/MainScreen.qml"));
    // @todo Ship translations and only fall back to english if no translations for the system locale are installed
    tts->setLocale("en");

    return app.exec();
}
