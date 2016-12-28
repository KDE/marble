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
#include "MarbleDirs.h"
#include "TextToSpeechClient.h"

using namespace Marble;

static bool loadTranslator(const QString &fullPath, QApplication &app)
{
    QTranslator* translator = new QTranslator(&app);
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
int main(int argc, char ** argv)
{
    QApplication app(argc, argv);
    app.setApplicationName( "Marble Maps" );
    app.setOrganizationName( "KDE" );
    app.setOrganizationDomain( "kde.org" );
#if QT_VERSION >= 0x050700
    app.setDesktopFileName(QStringLiteral("org.kde.marble.maps"));
#endif

    // Load Qt translation system catalog for libmarblewidget, the plugins and this app
    loadTranslations(app);

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
