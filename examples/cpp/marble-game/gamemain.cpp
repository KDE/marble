//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2014      Dennis Nienhüser <nienhueser@kde.org>
//

#include "GameMainWindow.h"

#include <marble/MarbleDirs.h>
#include <marble/MarbleDebug.h>
#include <marble/MarbleLocale.h>
#include <marble/MarbleGlobal.h>

#include <QApplication>
#include <QDir>
#include <QLocale>
#include <QTranslator>
#include <QDebug>

using namespace Marble;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Marble Game"));
    app.setOrganizationName(QStringLiteral("KDE"));
    app.setOrganizationDomain(QStringLiteral("kde.org"));
    // Widget translation

    QString      lang = QLocale::system().name().section(QLatin1Char('_'), 0, 0);
    QTranslator  translator;
    translator.load(QLatin1String("marble-") + lang, MarbleDirs::path(QStringLiteral("lang")));
    app.installTranslator(&translator);

    // For non static builds on mac and win
    // we need to be sure we can find the qt image
    // plugins. In mac be sure to look in the
    // application bundle...

#ifdef Q_WS_WIN
    QApplication::addLibraryPath( QApplication::applicationDirPath()
                                  + QDir::separator() + QLatin1String("plugins"));
#endif

    QString marbleDataPath;
    int dataPathIndex=0;
    MarbleGlobal::Profiles profiles = MarbleGlobal::detectProfiles();

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

    for ( int i = 1; i < args.count(); ++i ) {
        const QString arg = args.at(i);

        if ( arg == QLatin1String( "--debug-info" ) )
        {
            MarbleDebug::setEnabled( true );
        }
        else if ( arg.startsWith( QLatin1String( "--marbledatapath=" ), Qt::CaseInsensitive ) )
        {
            marbleDataPath = args.at(i).mid(17);
        }
        else if ( arg.compare( QLatin1String( "--marbledatapath" ), Qt::CaseInsensitive ) == 0 && i+1 < args.size() ) {
            dataPathIndex = i + 1;
            marbleDataPath = args.value( dataPathIndex );
            ++i;
        }
    }
    MarbleGlobal::getInstance()->setProfiles( profiles );

    MarbleLocale::MeasurementSystem const measurement =
            (MarbleLocale::MeasurementSystem)QLocale::system().measurementSystem();
    MarbleGlobal::getInstance()->locale()->setMeasurementSystem( measurement );

    MainWindow *window = new MainWindow( marbleDataPath );
    window->show();
    return app.exec();
}
