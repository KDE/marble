// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Dennis Nienhüser <nienhueser@kde.org>
//

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQuick>

#include <KAboutData>
#include <KCrash>
#include <KLocalizedContext>
#include <KLocalizedString>

#include <marble_version.h>

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

    KAboutData about(QStringLiteral("marble-behaim"),
                     i18n("Marble Behaim"),
                     QStringLiteral(MARBLE_VERSION_STRING),
                     i18n("Digital reproduction of a globe crafted at the time of Columbus' first sea travel to the west."),
                     KAboutLicense::GPL_V3,
                     i18n("© KDE Community"),
                     i18n("<p>Martin Behaim's Erdapfel</p>"
                          "<p>The oldest existent globe of the Earth. "
                          "Martin Behaim and collaborators created the globe around 1492 at the time of "
                          "<a href=\"https://en.wikipedia.org/wiki/Voyages_of_Christopher_Columbus\">Columbus'</a> first sea travel to the west."
                          "Hence the American continent is missing on this globe."
                          "Also note the detailed inscriptions in early modern German.</p>"
                          "<p>Please see <a href=\"https://en.wikipedia.org/wiki/Erdapfel\">Wikipedia: Erdapfel</a> "
                          "for further information about the Behaim globe."),
                     QStringLiteral("https://marble.kde.org/"),
                     QStringLiteral("https://bugs.kde.org/describecomponents.cgi?product=marble"));

    about.setOrganizationDomain("kde.org");

    about.addAuthor(i18nc("@info:credit", "Dennis Nienhüser"), i18nc("@info:credit", "Author"), QStringLiteral("nienhueser@kde.org"));

    about.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));

    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("org.kde.marble.behaim")));

    KAboutData::setApplicationData(about);

#ifndef Q_OS_ANDROID
    KCrash::initialize();
#endif

    QCommandLineParser parser;
    about.setupCommandLine(&parser);
    parser.process(app);
    about.processCommandLine(&parser);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.loadFromModule("org.kde.marble.behaim", "MainScreen");

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return QCoreApplication::exec();
}
