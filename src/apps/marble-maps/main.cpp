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

static bool loadTranslator(const QString &fullPath, QApplication &app)
{
    QTranslator *translator = new QTranslator(&app);
    if (!translator->load(fullPath)) {
        delete translator;
        return false;
    }

    app.installTranslator(translator);

    return true;
}

static bool loadTranslation(const QString &localeDirName, QApplication &app)
{
    // TODO: check if any translations for Qt modules have to be loaded,
    // as they need to be explicitly loaded as well by the Qt-using app

#ifdef Q_OS_ANDROID
    // load translation file from bundled packaging installation
    const QString fullPath = MarbleDirs::systemPath() + QLatin1String("/locale/") + localeDirName + QLatin1String("/marble_qt.qm");
#else
    // load translation file from normal "KDE Applications" packaging installation
    const QString subPath = QLatin1String("locale/") + localeDirName + QLatin1String("/LC_MESSAGES/marble_qt.qm");
    const QString fullPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, subPath);
    if (fullPath.isEmpty()) {
        return false;
    }
#endif

    return loadTranslator(fullPath, app);
}

// load KDE translators system based translations
static void loadTranslations(QApplication &app)
{
    QSettings settings;
    settings.beginGroup("localization");
    bool const translationsDisabled = settings.value("translationsDisabled", QVariant(false)).toBool();
    QString const translationFile = settings.value("translationFile").toUrl().path();
    settings.endGroup();

    if (translationsDisabled) {
        return;
    }

    // Quote from ecm_create_qm_loader created code:
    // The way Qt translation system handles plural forms makes it necessary to
    // have a translation file which contains only plural forms for `en`.
    // That's why we load the `en` translation unconditionally, then load the
    // translation for the current locale to overload it.
    const QString en(QStringLiteral("en"));

    loadTranslation(en, app);

    if (!translationFile.isEmpty() && loadTranslator(translationFile, app)) {
        return;
    }

    QLocale locale = QLocale::system();
    if (locale.name() != en) {
        if (!loadTranslation(locale.name(), app)) {
            loadTranslation(locale.bcp47Name(), app);
        }
    }
}

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

    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("org.kde.marble")));

    // Load Qt translation system catalog for libmarblewidget, the plugins and this app
    loadTranslations(app);

    MarbleDeclarativePlugin declarativePlugin;
    const char uri[] = "org.kde.marble";
    declarativePlugin.registerTypes(uri);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.loadFromModule("org.kde.marble.maps", "MainScreen");

    return app.exec();
}
