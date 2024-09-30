// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
// SPDX-FileCopyrightText: 2014 Dennis Nienhüser <nienhueser@kde.org>
//

#include "GameMainWindow.h"

#include <marble/MarbleDebug.h>
#include <marble/MarbleDirs.h>
#include <marble/MarbleGlobal.h>
#include <marble/MarbleLocale.h>

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QLocale>
#include <QTranslator>

using namespace Marble;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Marble Game"));
    app.setOrganizationName(QStringLiteral("KDE"));
    app.setOrganizationDomain(QStringLiteral("kde.org"));
    // Widget translation

    QString lang = QLocale::system().name().section(QLatin1Char('_'), 0, 0);
    QTranslator translator;
    translator.load(QLatin1StringView("marble-") + lang, MarbleDirs::path(QStringLiteral("lang")));
    app.installTranslator(&translator);

    // For non static builds on mac and win
    // we need to be sure we can find the qt image
    // plugins. In mac be sure to look in the
    // application bundle...

#ifdef Q_WS_WIN
    QApplication::addLibraryPath(QApplication::applicationDirPath() + QDir::separator() + QLatin1StringView("plugins"));
#endif

    QString marbleDataPath;
    int dataPathIndex = 0;

    QStringList args = QApplication::arguments();

    if (args.contains(QStringLiteral("-h")) || args.contains(QStringLiteral("--help"))) {
        qWarning() << "Usage: marble [options]";
        qWarning();
        qWarning() << "general options:";
        qWarning() << "  --marbledatapath=<path> .... Overwrite the compile-time path to map themes and other data";
        qWarning();
        qWarning() << "debug options:";
        qWarning() << "  --debug-info ............... write (more) debugging information to the console";

        return 0;
    }

    for (int i = 1; i < args.count(); ++i) {
        const QString arg = args.at(i);

        if (arg == QLatin1StringView("--debug-info")) {
            MarbleDebug::setEnabled(true);
        } else if (arg.startsWith(QLatin1StringView("--marbledatapath="), Qt::CaseInsensitive)) {
            marbleDataPath = args.at(i).mid(17);
        } else if (arg.compare(QLatin1StringView("--marbledatapath"), Qt::CaseInsensitive) == 0 && i + 1 < args.size()) {
            dataPathIndex = i + 1;
            marbleDataPath = args.value(dataPathIndex);
            ++i;
        }
    }

    MarbleLocale::MeasurementSystem const measurement = (MarbleLocale::MeasurementSystem)QLocale::system().measurementSystem();
    MarbleGlobal::getInstance()->locale()->setMeasurementSystem(measurement);

    MainWindow *window = new MainWindow(marbleDataPath);
    window->show();
    return app.exec();
}
