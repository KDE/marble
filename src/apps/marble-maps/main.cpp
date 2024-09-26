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
#include <MarbleGlobal.h>
#include <marble_version.h>

#ifndef Q_OS_ANDROID
#include <KCrash>
#endif

#if !MARBLE_WEBKITWIDGETS
#include <QtWebEngineQuick>
#endif

using namespace Marble;

#ifdef Q_OS_ANDROID
Q_DECL_EXPORT
#endif
int main(int argc, char **argv)
{
#if !MARBLE_WEBKITWIDGETS
    QtWebEngineQuick::initialize();
#endif

    QApplication app(argc, argv);

    KAboutData about(QStringLiteral("marble-maps"),
                     i18n("Marble Maps"),
                     QStringLiteral(MARBLE_VERSION_STRING),
                     i18n("Maps"),
                     KAboutLicense::GPL_V3,
                     i18n("© KDE Community"));

    about.setDesktopFileName(QStringLiteral("org.kde.marble.maps"));
    about.setOrganizationDomain("kde.org");
    about.setBugAddress("https://bugs.kde.org/describecomponents.cgi?product=marble");

    about.addAuthor(i18n("Carl Schwan"),
                    i18n("Developer"),
                    QStringLiteral("carl@carlschwan.eu"),
                    QStringLiteral("https://carlschwan.eu"),
                    QUrl(QStringLiteral("https://carlschwan.eu/avatar.png")));

    about.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));

    KAboutData::setApplicationData(about);

    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("org.kde.marble.maps")));

#ifndef Q_OS_ANDROID
    KCrash::initialize();
#endif

    QCommandLineParser parser;
    about.setupCommandLine(&parser);
    parser.process(app);
    about.processCommandLine(&parser);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.loadFromModule("org.kde.marble.maps", "MainScreen");

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return QCoreApplication::exec();
}
