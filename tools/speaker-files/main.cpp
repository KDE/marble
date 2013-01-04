//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

/**
  * Takes an .sqlite database with metadata about voice guidance speakers
  * and associated .zip files containing 64.ogg files and produces four files
  * for each speaker:
  * - a .tar.gz to be used by KDE Marble via a GHNS dialog
  * - a .zip to be downloaded by Marble users via edu.kde.org
  * - a .zip to be downloaded by TomTom users via edu.kde.org
  * - an .ogg speaker preview file
  * The archives contain the speaker files and some additional stuff (license, authors, ...)
  *
  * The structure of the .sqlite database is expected as follows:
  * One table called speakers with the following layout:
  * - id PRIMARY KEY
  * - name, email, nickname, gender, language, description, token VARCHAR
  * - created DATETIME
  * Additionally, the field gender is expected to be either "male" or "female" and language
  * to have the format "NAME (langcode)"
  *
  * Also creates a knewstuff .xml file with the metadata.
  *
  * Some processing is done by calling other tools, namely tar, unzip, zip, vorbisgain, viftool.
  * Make sure they're found in $PATH
  *
  */

#include <QtCore/QCoreApplication>
#include <QtCore/QString>
#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QVariant>
#include <QtCore/QTemporaryFile>
#include <QtCore/QProcess>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

QStringList tomTomFiles()
{
    QStringList result;
    result << "100.ogg";
    result << "200.ogg";
    result << "2ndLeft.ogg";
    result << "2ndRight.ogg";
    result << "300.ogg";
    result << "3rdLeft.ogg";
    result << "3rdRight.ogg";
    result << "400.ogg";
    result << "500.ogg";
    result << "50.ogg";
    result << "600.ogg";
    result << "700.ogg";
    result << "800.ogg";
    result << "80.ogg";
    result << "After.ogg";
    result << "AhExitLeft.ogg";
    result << "AhExit.ogg";
    result << "AhExitRight.ogg";
    result << "AhFerry.ogg";
    result << "AhKeepLeft.ogg";
    result << "AhKeepRight.ogg";
    result << "AhLeftTurn.ogg";
    result << "AhRightTurn.ogg";
    result << "AhUTurn.ogg";
    result << "Arrive.ogg";
    result << "BearLeft.ogg";
    result << "BearRight.ogg";
    result << "Charge.ogg";
    result << "Depart.ogg";
    result << "KeepLeft.ogg";
    result << "KeepRight.ogg";
    result << "LnLeft.ogg";
    result << "LnRight.ogg";
    result << "Meters.ogg";
    result << "MwEnter.ogg";
    result << "MwExitLeft.ogg";
    result << "MwExit.ogg";
    result << "MwExitRight.ogg";
    result << "RbBack.ogg";
    result << "RbCross.ogg";
    result << "RbExit1.ogg";
    result << "RbExit2.ogg";
    result << "RbExit3.ogg";
    result << "RbExit4.ogg";
    result << "RbExit5.ogg";
    result << "RbExit6.ogg";
    result << "RbLeft.ogg";
    result << "RbRight.ogg";
    result << "RoadEnd.ogg";
    result << "SharpLeft.ogg";
    result << "SharpRight.ogg";
    result << "Straight.ogg";
    result << "TakeFerry.ogg";
    result << "Then.ogg";
    result << "TryUTurn.ogg";
    result << "TurnLeft.ogg";
    result << "TurnRight.ogg";
    result << "UTurn.ogg";
    result << "Yards.ogg";
    return result;
}

QStringList marbleFiles()
{
    QStringList result;
    result << "Marble.ogg";
    result << "RouteCalculated.ogg";
    result << "RouteDeviated.ogg";
    result << "GpsFound.ogg";
    result << "GpsLost.ogg";
    return result;
}

void usage( const QString &application )
{
    qDebug() << "Usage: " << application << " /path/to/input/directory /path/to/output/directory /path/to/newstuff.xml";
}

void extract( const QString &zip, const QString &output )
{
    QProcess::execute( "unzip", QStringList() << "-q" << "-j" << "-d" << output << zip );
}

void normalize( const QString &output )
{
    QProcess vorbisgain;
    vorbisgain.setWorkingDirectory( output );
    vorbisgain.start( "vorbisgain", QStringList() << "-a" << tomTomFiles() << marbleFiles() );
    vorbisgain.waitForFinished();
}

void createLegalFiles( const QString &directory, const QString &name, const QString &email )
{
    QDir input( directory );
    QFile authorsFile( input.filePath( "AUTHORS.txt" ) );
    if ( authorsFile.open( QFile::WriteOnly | QFile::Truncate ) ) {
        QTextStream stream( &authorsFile );
        stream << name << " <" << email << ">";
    }
    authorsFile.close();

    QFile licenseFile( input.filePath( "LICENSE.txt" ) );
    if ( licenseFile.open( QFile::WriteOnly | QFile::Truncate ) ) {
        QTextStream stream( &licenseFile );
        stream << "The ogg files in this directory are licensed under the creative commons Attribution-ShareAlike 3.0 Unported (CC BY-SA 3.0) license. ";
        stream << "See http://creativecommons.org/licenses/by-sa/3.0/ and the file CC-BY-SA-3.0 in this directory.";
    }
    licenseFile.close();

    QFile installFile( input.filePath( "INSTALL.txt" ) );
    if ( installFile.open( QFile::WriteOnly | QFile::Truncate ) ) {
        QTextStream stream( &installFile );
        stream << "To install this voice guidance speaker in Marble, copy the entire directory to the audio/speakers/ directory in Marble's data path.\n\n";
        stream << "For example, if this directory is called 'MySpeaker' and you want to use it on the Nokia N900, copy the directory with all files to /home/user/MyDocs/.local/share/marble/audio/speakers/MySpeaker\n\n";
        stream << "Afterwards start Marble on the N900 and press the routing info box (four icons on the bottom) for two seconds with the pen. Enter the configuration dialog and choose the 'MySpeaker' speaker.\n\n";
        stream << "Check http://edu.kde.org/marble/speakers.php for updates and more speakers.";
    }
    installFile.close();
}

void convertToNewStuffFormat( const QString &input, const QString &output )
{
    QDir inputDirectory( input );
    QStringList files;
    files << tomTomFiles() << marbleFiles();
    files << "AUTHORS.txt" << "INSTALL.txt" << "LICENSE.txt";
    QStringList arguments;
    arguments << "-czf" << output;
    foreach( const QString &file, files ) {
        arguments << inputDirectory.filePath( file );
    }
    arguments << "/usr/share/common-licenses/CC-BY-SA-3.0";

    QProcess::execute( "tar", arguments );
}

void convertToMarbleFormat( const QString &input, const QString &output )
{
    QDir inputDirectory( input );
    QStringList files;
    files << tomTomFiles() << marbleFiles();
    files << "AUTHORS.txt" << "INSTALL.txt" << "LICENSE.txt";
    QStringList arguments;
    arguments << "-q" << "-j" << output;
    foreach( const QString &file, files ) {
        arguments << inputDirectory.filePath( file );
    }
    arguments << "/usr/share/common-licenses/CC-BY-SA-3.0";

    QProcess::execute( "zip", arguments );
}

void convertToTomTomFormat( const QString &input, const QString &output, const QString &nick, const QString &simpleNick, int index, bool male, const QString &lang )
{
    QStringList arguments;
    QString const prefix = input + "/data" + QString::number( index );
    QString const vif = prefix + ".vif";
    QString const chk = prefix + ".chk";
    arguments << "join" << QString::number( index ) << nick << vif;
    QProcess viftool;
    viftool.setWorkingDirectory( input );
    viftool.execute( "viftool", arguments );

    QFile vifFile( vif );
    if ( vifFile.open( QFile::WriteOnly | QFile::Truncate ) ) {
        QTextStream stream( &vifFile );
        stream << nick << "\n"; // Name
        stream << ( male ? 2 : 1 ) << "\n"; // gender index
        /** @todo: flag, language index */
        stream << 2 << "\n"; // Language index
        stream << 114 << "\n"; // Flag index
        stream << "1\n"; // Version number
    }
    vifFile.close();

    QDir inputDirectory( input );
    QStringList files;
    files << vif << chk;
    files << "AUTHORS.txt" << "LICENSE.txt";
    QStringList zipArguments;
    zipArguments << "-q" << "-j" << ( output + '/' + lang + '-' + simpleNick + "-TomTom.zip" );
    foreach( const QString &file, files ) {
        QString const filePath = inputDirectory.filePath( file );
        zipArguments <<  filePath;
    }
    zipArguments << "/usr/share/common-licenses/CC-BY-SA-3.0";

    QProcess::execute( "zip", zipArguments );
}

int process( const QDir &input, const QDir &output, const QString &xml )
{
    QSqlDatabase database = QSqlDatabase::addDatabase( "QSQLITE" );
    database.setDatabaseName( input.filePath( "speakers.db" ) );
    if ( !database.open() ) {
        qDebug() << "Failed to connect to database " << input.filePath( "speakers.db" );
        return 3;
    }

    output.mkdir( "files.kde.org" );
    QSqlQuery query( "SELECT * FROM speakers ORDER BY Id" );

    QFile xmlFile( xml );
    if ( !xmlFile.open( QFile::WriteOnly | QFile::Truncate ) ) {
        qDebug() << "Failed to write to " << xmlFile.fileName();
        return 3;
    }

    QTextStream xmlOut( &xmlFile );
    xmlOut << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xmlOut << "<!DOCTYPE knewstuff SYSTEM \"knewstuff.dtd\">\n";
    xmlOut << "<?xml-stylesheet type=\"text/xsl\" href=\"speakers.xsl\" ?>\n";
    xmlOut << "<knewstuff>\n";

    int index = 71;
    while (query.next()) {
        QString const name = query.value(1).toString();
        QString const email = query.value(2).toString();
        QString const nick = query.value(3).toString();
        QString const gender = query.value(4).toString();
        QString const language = query.value(5).toString();
        QString const lang = language.mid( 0, language.indexOf( "(" )-1 ).replace( QLatin1Char(' '), QLatin1Char('-') );
        QString const description = query.value(6).toString();
        QString const token = query.value(7).toString();
        QString const date = query.value(8).toString();
        QString const zip = input.filePath( token );
        QTemporaryFile tmpFile;
        tmpFile.open();
        QString const extracted = tmpFile.fileName();
        tmpFile.remove();
        QDir::root().mkdir( extracted );
        qDebug() << "Name: " << name;

        QString const simpleNick = QString( nick ).replace( QLatin1Char(' '), QLatin1Char('-') );
        QString const nickDir = output.filePath( "files.kde.org" ) + '/' + simpleNick;
        QDir::root().mkdir( nickDir );
        extract( zip, extracted );
        normalize( extracted );
        createLegalFiles( extracted, name, email );
        QFile::copy( extracted + "/Marble.ogg", nickDir + '/' + lang + '-' + simpleNick + ".ogg" );
        convertToMarbleFormat( extracted, nickDir + '/' + lang + '-' + simpleNick + ".zip" );
        convertToTomTomFormat( extracted, nickDir, nick, simpleNick, index, gender == "male", lang );
        convertToNewStuffFormat( extracted, nickDir + '/' + lang + '-' + simpleNick + ".tar.gz" );

        xmlOut << "  <stuff category=\"marble/data/audio\">\n";
        xmlOut << "    <name lang=\"en\">" << language << " - " << nick << " (" <<  gender << ")" << "</name>\n";
        xmlOut << "    <author>" << name << "</author>\n";
        xmlOut << "    <licence>CC-By-SA 3.0</licence>\n";
        xmlOut << "    <summary lang=\"en\">" << description << "</summary>\n";
        xmlOut << "    <version>0.1</version>\n";
        xmlOut << "    <releasedate>" << date << "</releasedate>\n";
        xmlOut << "    <preview lang=\"en\">http://edu.kde.org/marble/speaker-" << gender << ".png</preview>\n";
        xmlOut << "    <payload lang=\"en\">http://files.kde.org/marble/audio/speakers/" << simpleNick << "/" << lang << "-" << simpleNick << ".tar.gz</payload>\n";
        xmlOut << "    <payload lang=\"ogg\">http://files.kde.org/marble/audio/speakers/" << simpleNick << "/" << lang << "-" << simpleNick << ".ogg</payload>\n";
        xmlOut << "    <payload lang=\"zip\">http://files.kde.org/marble/audio/speakers/" << simpleNick << "/" << lang << "-" << simpleNick << ".zip</payload>\n";
        xmlOut << "    <payload lang=\"tomtom\">http://files.kde.org/marble/audio/speakers/" << simpleNick << "/" << lang << "-" << simpleNick << "-TomTom.zip</payload>\n";
        xmlOut << "  </stuff>\n";

        ++index;
    }

    xmlOut << "</knewstuff>\n";
    xmlFile.close();
    return 0;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if ( argc < 4 ) {
        usage( argv[0] );
        return 1;
    }

    QFileInfo input( argv[1] );
    if ( !input.exists() || !input.isDir() ) {
        qDebug() << "Incorrect input directory " << argv[1];
        usage( argv[0] );
        return 1;
    }

    QFileInfo output( argv[2] );
    if ( !output.exists() || !output.isWritable() ) {
        qDebug() << "Incorrect output directory " << argv[1];
        usage( argv[0] );
        return 1;
    }

    QFileInfo xmlFile( argv[3] );
    return process( QDir( input.absoluteFilePath() ), QDir( output.absoluteFilePath() ), xmlFile.absoluteFilePath() );
}
