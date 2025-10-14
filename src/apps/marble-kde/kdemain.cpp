// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
// SPDX-FileCopyrightText: 2009 Jens-Michael Hoffmann <jensmh@gmx.de>
//

#include <KAboutData>
#include <KConfig>
#include <KConfigGroup>
#include <KCrash>
#include <KLocalizedString>
#include <KSharedConfig>

#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QFile>
#include <QLocale>
#include <QStandardPaths>
#include <QTranslator>

#include "ControlView.h"
#include "KdeMainWindow.h"
#include "MarbleDebug.h"

#include "MarbleTest.h"

#ifdef STATIC_BUILD
#include <QtPlugin>
Q_IMPORT_PLUGIN(qjpeg)
Q_IMPORT_PLUGIN(qsvg)
#endif

using namespace Marble;

static bool loadTranslation(const QString &localeDirName, QApplication &app)
{
    const QString subPath = QLatin1StringView("locale/") + localeDirName + QLatin1StringView("/LC_MESSAGES/marble_qt.qm");
    const QString fullPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, subPath);
    if (fullPath.isEmpty()) {
        return false;
    }

    auto translator = new QTranslator(&app);
    if (!translator->load(fullPath)) {
        delete translator;
        return false;
    }

    app.installTranslator(translator);

    return true;
}

static void loadLibAndPluginTranslations(QApplication &app)
{
    // Quote from ecm_create_qm_loader created code:
    // The way Qt translation system handles plural forms makes it necessary to
    // have a translation file which contains only plural forms for `en`.
    // That's why we load the `en` translation unconditionally, then load the
    // translation for the current locale to overload it.
    const QString en(QStringLiteral("en"));

    loadTranslation(en, app);

    QLocale locale = QLocale::system();
    if (locale.name() != en) {
        if (!loadTranslation(locale.name(), app)) {
            loadTranslation(locale.bcp47Name(), app);
        }
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Load Qt translation system catalog for the plugins and libmarblewidget
    loadLibAndPluginTranslations(app);
    // Init KF6 translation system
    KLocalizedString::setApplicationDomain("marble");

    KAboutData aboutData(QStringLiteral("marble"),
                         i18n("Marble Virtual Globe"),
                         ControlView::applicationVersion(),
                         i18n("A World Atlas."),
                         KAboutLicense::LGPL_V2,
                         i18n("(c) 2007-%1", QLatin1StringView("2016")),
                         QString(),
                         QStringLiteral("https://marble.kde.org/"));

    // Active Development Team of Marble
    aboutData.addAuthor(i18n("Torsten Rahn"), i18n("Developer and Original Author"), QStringLiteral("rahn@kde.org"));
    aboutData.addAuthor(i18n("Bernhard Beschow"), i18n("WMS Support, Mobile, Performance"), QStringLiteral("bbeschow@cs.tu-berlin.de"));
    aboutData.addAuthor(i18n("Thibaut Gridel"), i18n("Geodata"), QStringLiteral("tgridel@free.fr"));
    aboutData.addAuthor(i18n("Jens-Michael Hoffmann"), i18n("OpenStreetMap Integration, OSM Namefinder, Download Management"), QStringLiteral("jmho@c-xx.com"));
    aboutData.addAuthor(i18n("Florian E&szlig;er"), i18n("Elevation Profile"), QStringLiteral("f.esser@rwth-aachen.de"));
    aboutData.addAuthor(i18n("Wes Hardaker"), i18n("APRS Plugin"), QStringLiteral("marble@hardakers.net"));
    aboutData.addAuthor(i18n("Bastian Holst"), i18n("Online Services support"), QStringLiteral("bastianholst@gmx.de"));
    aboutData.addAuthor(i18n("Guillaume Martres"), i18n("Satellites"), QStringLiteral("smarter@ubuntu.com"));
    aboutData.addAuthor(i18n("Rene Kuettner"), i18n("Satellites, Eclipses"), QStringLiteral("rene@bitkanal.net"));
    aboutData.addAuthor(i18n("Friedrich W. H. Kossebau"), i18n("Plasma Integration, Bugfixes"), QStringLiteral("kossebau@kde.org"));
    aboutData.addAuthor(i18n("Dennis Nienhüser"), i18n("Routing, Navigation, Mobile"), QStringLiteral("nienhueser@kde.org"));
    aboutData.addAuthor(i18n("Niko Sams"), i18n("Routing, Elevation Profile"), QStringLiteral("niko.sams@gmail.com"));
    aboutData.addAuthor(i18n("Patrick Spendrin"), i18n("Core Developer: KML and Windows support"), QStringLiteral("pspendrin@gmail.com"));
    aboutData.addAuthor(i18n("Eckhart Wörner"), i18n("Bugfixes"), QStringLiteral("kde@ewsoftware.de"));

    // Developers:

    aboutData.addAuthor(i18n("Inge Wallin"), i18n("Core Developer and Co-Maintainer"), QStringLiteral("inge@lysator.liu.se"));
    aboutData.addAuthor(i18n("Henry de Valence"), i18n("Core Developer: Marble Runners, World-Clock Plasmoid"), QStringLiteral("hdevalence@gmail.com"));
    aboutData.addAuthor(i18n("Pino Toscano"), i18n("Network plugins"), QStringLiteral("pino@kde.org"));
    aboutData.addAuthor(i18n("Harshit Jain"), i18n("Planet filter"), QStringLiteral("sonu.itbhu@googlemail.com"));
    aboutData.addAuthor(i18n("Simon Edwards"), i18n("Marble Python Bindings"), QStringLiteral("simon@simonzone.com"));
    aboutData.addAuthor(i18n("Magnus Valle"), i18n("Historical Maps"), {});
    aboutData.addAuthor(i18n("Médéric Boquien"), i18n("Astronomical Observatories"), QStringLiteral("mboquien@free.fr"));

    // ESA Summer of Code in Space
    aboutData.addAuthor(i18n("Rene Kuettner"),
                        i18n("ESA Summer of Code in Space 2012 Project:"
                             " Visualization of planetary satellites"),
                        QStringLiteral("rene@bitkanal.net"));
    aboutData.addAuthor(i18n("Guillaume Martres"),
                        i18n("ESA Summer of Code in Space 2011 Project:"
                             " Visualization of Satellite Orbits"),
                        QStringLiteral("smarter@ubuntu.com"));

    // Google Summer of Code
    aboutData.addAuthor(i18n("Konstantin Oblaukhov"),
                        i18n("Google Summer of Code 2011 Project:"
                             " OpenStreetMap Vector Rendering"),
                        QStringLiteral("oblaukhov.konstantin@gmail.com"));
    aboutData.addAuthor(i18n("Daniel Marth"),
                        i18n("Google Summer of Code 2011 Project:"
                             " Marble Touch on MeeGo"),
                        QStringLiteral("danielmarth@gmx.at"));
    aboutData.addAuthor(i18n("Gaurav Gupta"),
                        i18n("Google Summer of Code 2010 Project:"
                             " Bookmarks"),
                        QStringLiteral("1989.gaurav@gmail.com"));
    aboutData.addAuthor(i18n("Harshit Jain "),
                        i18n("Google Summer of Code 2010 Project:"
                             " Time Support"),
                        QStringLiteral("hjain.itbhu@gmail.com"));
    aboutData.addAuthor(i18n("Siddharth Srivastava"),
                        i18n("Google Summer of Code 2010 Project:"
                             " Turn-by-turn Navigation"),
                        QStringLiteral("akssps011@gmail.com"));
    aboutData.addAuthor(i18n("Andrew Manson"),
                        i18n("Google Summer of Code 2009 Project:"
                             " OSM Annotation"),
                        QStringLiteral("g.real.ate@gmail.com"));
    aboutData.addAuthor(i18n("Bastian Holst"),
                        i18n("Google Summer of Code 2009 Project:"
                             " Online Services"),
                        QStringLiteral("bastianholst@gmx.de"));
    aboutData.addAuthor(i18n("Patrick Spendrin"),
                        i18n("Google Summer of Code 2008 Project:"
                             " Vector Tiles for Marble"),
                        QStringLiteral("pspendrin@gmail.com"));
    aboutData.addAuthor(i18n("Shashank Singh"),
                        i18n("Google Summer of Code 2008 Project:"
                             " Panoramio / Wikipedia -photo support for Marble"),
                        QStringLiteral("shashank.personal@gmail.com"));
    aboutData.addAuthor(i18n("Carlos Licea"),
                        i18n("Google Summer of Code 2007 Project:"
                             " Equirectangular Projection (\"Flat Map\")"),
                        QStringLiteral("carlos.licea@kdemail.net"));
    aboutData.addAuthor(i18n("Andrew Manson"),
                        i18n("Google Summer of Code 2007 Project:"
                             " GPS Support for Marble"),
                        QStringLiteral("g.real.ate@gmail.com"));
    aboutData.addAuthor(i18n("Murad Tagirov"),
                        i18n("Google Summer of Code 2007 Project:"
                             " KML Support for Marble"),
                        QStringLiteral("tmurad@gmail.com"));

    // Developers
    aboutData.addAuthor(i18n("Simon Schmeisser"), i18n("Development & Patches"));
    aboutData.addAuthor(i18n("Claudiu Covaci"), i18n("Development & Patches"));
    aboutData.addAuthor(i18n("David Roberts"), i18n("Development & Patches"));
    aboutData.addAuthor(i18n("Nikolas Zimmermann"), i18n("Development & Patches"));
    aboutData.addAuthor(i18n("Jan Becker"), i18n("Development & Patches"));
    aboutData.addAuthor(i18n("Stefan Asserhäll"), i18n("Development & Patches"));
    aboutData.addAuthor(i18n("Laurent Montel"), i18n("Development & Patches"));
    aboutData.addAuthor(i18n("Mayank Madan"), i18n("Development & Patches"));
    aboutData.addAuthor(i18n("Prashanth Udupa"), i18n("Development & Patches"));
    aboutData.addAuthor(i18n("Anne-Marie Mahfouf"), i18n("Development & Patches"));
    aboutData.addAuthor(i18n("Josef Spillner"), i18n("Development & Patches"));
    aboutData.addAuthor(i18n("Frerich Raabe"), i18n("Development & Patches"));
    aboutData.addAuthor(i18n("Frederik Gladhorn"), i18n("Development & Patches"));
    aboutData.addAuthor(i18n("Fredrik Höglund"), i18n("Development & Patches"));
    aboutData.addAuthor(i18n("Albert Astals Cid"), i18n("Development & Patches"));
    aboutData.addAuthor(i18n("Thomas Zander"), i18n("Development & Patches"));
    aboutData.addAuthor(i18n("Joseph Wenninger"), i18n("Development & Patches"));
    aboutData.addAuthor(i18n("Kris Thomsen"), i18n("Development & Patches"));
    aboutData.addAuthor(i18n("Daniel Molkentin"), i18n("Development & Patches"));
    aboutData.addAuthor(i18n("Christophe Leske"), i18n("Platforms & Distributions"));
    aboutData.addAuthor(i18n("Sebastian Wiedenroth"), i18n("Platforms & Distributions"));
    aboutData.addAuthor(i18n("Tim Sutton"), i18n("Platforms & Distributions"));
    aboutData.addAuthor(i18n("Christian Ehrlicher"), i18n("Platforms & Distributions"));
    aboutData.addAuthor(i18n("Ralf Habacker"), i18n("Platforms & Distributions"));
    aboutData.addAuthor(i18n("Steffen Joeris"), i18n("Platforms & Distributions"));
    aboutData.addAuthor(i18n("Marcus Czeslinski"), i18n("Platforms & Distributions"));
    aboutData.addAuthor(i18n("Marcus D. Hanwell"), i18n("Platforms & Distributions"));
    aboutData.addAuthor(i18n("Chitlesh Goorah"), i18n("Platforms & Distributions"));
    aboutData.addAuthor(i18n("Nuno Pinheiro"), i18n("Artwork"));
    aboutData.addAuthor(i18n("Torsten Rahn"), i18n("Artwork"));

    // Credits
    aboutData.addCredit(i18n("Luis Silva"), i18n("Various Suggestions & Testing"));
    aboutData.addCredit(i18n("Stefan Jordan"), i18n("Various Suggestions & Testing"));
    aboutData.addCredit(i18n("Robert Scott"), i18n("Various Suggestions & Testing"));
    aboutData.addCredit(i18n("Lubos Petrovic"), i18n("Various Suggestions & Testing"));
    aboutData.addCredit(i18n("Benoit Sigoure"), i18n("Various Suggestions & Testing"));
    aboutData.addCredit(i18n("Martin Konold"), i18n("Various Suggestions & Testing"));
    aboutData.addCredit(i18n("Matthias Welwarsky"), i18n("Various Suggestions & Testing"));
    aboutData.addCredit(i18n("Rainer Endres"), i18n("Various Suggestions & Testing"));
    aboutData.addCredit(i18n("Ralf Gesellensetter"), i18n("Various Suggestions & Testing"));
    aboutData.addCredit(i18n("Tim Alder"), i18n("Various Suggestions & Testing"));
    aboutData.addCredit(i18n("John Layt"),
                        i18n("Special thanks for providing an"
                             " important source of inspiration by creating"
                             " Marble's predecessor \"Kartographer\"."));

    KCrash::setCrashHandler(KCrash::defaultCrashHandler);
    KCrash::setDrKonqiEnabled(true);

    KAboutData::setApplicationData(aboutData);
    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("marble")));

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);

    // Autodetect profiles
    MarbleGlobal::Profiles profiles = MarbleGlobal::getInstance()->profiles();

    QCommandLineOption debugOption(QStringLiteral("debug-info"), i18n("Enable debug output"));
    debugOption.setFlags(QCommandLineOption::HiddenFromHelp);
    parser.addOption(debugOption);
    QCommandLineOption levelOption(QStringLiteral("debug-levels"), i18n("Display OSM placemarks according to the level selected"));
    parser.addOption(levelOption);
    QCommandLineOption timeOption(QStringLiteral("timedemo"), i18n("Make a time measurement to check performance"));
    parser.addOption(timeOption);
    QCommandLineOption fpsOption(QStringLiteral("fps"), i18n("Show frame rate"));
    parser.addOption(fpsOption);
    QCommandLineOption tileOption(QStringLiteral("tile-id"), i18n("Show tile IDs"));
    parser.addOption(tileOption);
    QCommandLineOption traceOption(QStringLiteral("runtimeTrace"), i18n("Show time spent in each layer"));
    parser.addOption(traceOption);
    QCommandLineOption dataPathOption(QStringLiteral("marbledatapath"),
                                      i18n("Use a different directory <directory> which contains map data."),
                                      QStringLiteral("directory"));
    parser.addOption(dataPathOption);
    QCommandLineOption noSmallScreenOption(QStringLiteral("nosmallscreen"), i18n("Do not use the interface optimized for small screens"));
    QCommandLineOption smallScreenOption(QStringLiteral("smallscreen"), i18n("Use the interface optimized for small screens"));
    parser.addOption((profiles & MarbleGlobal::SmallScreen) ? noSmallScreenOption : smallScreenOption);
    QCommandLineOption noHighResOption(QStringLiteral("nohighresolution"), i18n("Do not use the interface optimized for high resolutions"));
    QCommandLineOption highResOption(QStringLiteral("highresolution"), i18n("Use the interface optimized for high resolutions"));
    parser.addOption((profiles & MarbleGlobal::HighResolution) ? noHighResOption : highResOption);
    QCommandLineOption coordinatesOption(QStringLiteral("latlon"), i18n("Show map at given lat lon <coordinates>"), QStringLiteral("coordinates"));
    parser.addOption(coordinatesOption);
    QCommandLineOption geoUriOption(QStringLiteral("geo-uri"), i18n("Show map at given geo <uri>"), QStringLiteral("uri"));
    parser.addOption(geoUriOption);
    QCommandLineOption distanceOption(QStringLiteral("distance"), i18n("Set the distance of the observer to the globe (in km)"), QStringLiteral("distance"));
    parser.addOption(distanceOption);
    QCommandLineOption mapIdOption(QStringLiteral("map"), i18n("Use map <id> (e.g. \"earth/openstreetmap/openstreetmap.dgml\")"), QStringLiteral("id"));
    parser.addOption(mapIdOption);
    parser.addPositionalArgument(QStringLiteral("file"), i18n("One or more placemark files to be opened"));

    parser.process(app);
    aboutData.processCommandLine(&parser);

    // use ecm_create_qm_loader(marblewidget_SRCS marble_qt)
    // in the library src/lib/marble/CMakeList.txt to load the second catalog

    if (parser.isSet(debugOption)) {
        qCWarning(MARBLE_DEFAULT) << "The '--debug-info' option is no longer supported, use the Qt logging settings.";
    }

    if (parser.isSet(smallScreenOption)) {
        profiles |= MarbleGlobal::SmallScreen;
    } else {
        profiles &= ~MarbleGlobal::SmallScreen;
    }

    if (parser.isSet(highResOption)) {
        profiles |= MarbleGlobal::HighResolution;
    } else {
        profiles &= ~MarbleGlobal::HighResolution;
    }

    MarbleGlobal::getInstance()->setProfiles(profiles);

    QString marbleDataPath = parser.value(dataPathOption);
    auto window = new MainWindow(marbleDataPath);
    window->show();

    if (parser.isSet(timeOption)) {
        window->resize(900, 640);
        MarbleTest test(window->marbleWidget());
        test.timeDemo();
        return 0;
    }

    if (parser.isSet(fpsOption)) {
        window->marbleControl()->marbleWidget()->setShowFrameRate(true);
    }

    if (parser.isSet(levelOption)) {
        window->marbleWidget()->setDebugLevelTags(true);
    }

    if (parser.isSet(tileOption)) {
        window->marbleControl()->marbleWidget()->setShowTileId(true);
    }

    const QString map = parser.value(mapIdOption);
    if (!map.isEmpty()) {
        window->marbleWidget()->setMapThemeId(map);
    }

    const QString coordinatesString = parser.value(coordinatesOption);
    if (!coordinatesString.isEmpty()) {
        bool success = false;
        const GeoDataCoordinates coordinates = GeoDataCoordinates::fromString(coordinatesString, success);
        if (success) {
            const qreal longitude = coordinates.longitude(GeoDataCoordinates::Degree);
            const qreal latitude = coordinates.latitude(GeoDataCoordinates::Degree);
            window->marbleWidget()->centerOn(longitude, latitude);
        }
    }

    const QString geoUriString = parser.value(geoUriOption);
    if (!geoUriString.isEmpty()) {
        window->marbleControl()->openGeoUri(geoUriString);
    }

    const QString distance = parser.value(distanceOption);
    if (!distance.isEmpty()) {
        bool success = false;
        const qreal distanceValue = distance.toDouble(&success);
        if (success)
            window->marbleWidget()->setDistance(distanceValue);
    }

    // Read the URLs that are given on the command line.
    for (const QString &arg : parser.positionalArguments()) {
        const auto url = QUrl::fromUserInput(arg, QDir::currentPath());
        if (!url.isValid()) {
            continue;
        }
        if (url.isLocalFile()) {
            const auto file = url.toLocalFile();
            if (QFile::exists(file)) {
                window->marbleControl()->addGeoDataFile(file);
            }
            continue;
        }
        window->marbleControl()->openGeoUri(url.toString());
    }

    return app.exec();
}
