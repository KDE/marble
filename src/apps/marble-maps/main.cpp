// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Gábor Péterffy <peterffy95@gmail.com>
//

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQuick>

#include <KAboutData>
#include <KLocalizedContext>
#include <KLocalizedString>

#include "MarbleMaps.h"
#include "declarative/MarbleDeclarativePlugin.h"
#include <MarbleGlobal.h>
#include <marble_version.h>

#ifndef Q_OS_ANDROID
#include <KCrash>
#endif

using namespace Marble;

#ifdef Q_OS_ANDROID
// Declare symbol of main method as exported as needed by Qt-on-Android,
// where the Dalvik-native QtActivity class needs to find and invoke it
// on loading the "app" module
extern "C" Q_DECL_EXPORT
#endif
    int
    main(int argc, char **argv)
{
    QApplication app(argc, argv);

    KAboutData about(QStringLiteral("marble-maps"),
                     i18n("Marble Maps"),
                     QStringLiteral(MARBLE_VERSION_STRING),
                     i18n("Maps"),
                     KAboutLicense::GPL_V3,
                     i18n("© KDE Community"));

    KAboutData::setApplicationData(about);

#ifndef Q_OS_ANDROID
    KCrash::initialize();
#endif

    QCommandLineParser parser;
    about.setupCommandLine(&parser);
    parser.process(app);
    about.processCommandLine(&parser);

    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("org.kde.marble.maps")));

    MarbleDeclarativePlugin declarativePlugin;
    const char uri[] = "org.kde.marble";
    declarativePlugin.registerTypes(uri);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.loadFromModule("org.kde.marble.maps", "MainScreen");

    return app.exec();
}
