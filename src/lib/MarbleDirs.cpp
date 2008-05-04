//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#include "MarbleDirs.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtGui/QApplication>

#ifdef Q_OS_MACX
//for getting app bundle path
#include <ApplicationServices/ApplicationServices.h>
#endif

#include <config-marble.h>

namespace
{
    QString runTimeMarbleDataPath = "";
}

MarbleDirs::MarbleDirs()
    : d( 0 )
{
}


QString MarbleDirs::path( const QString& relativePath )
{ 
    QString  localpath = localPath() + QDir::separator() + relativePath;	// local path
//    qDebug( "localpath: %s", qPrintable( localpath ) );
    QString  systempath  = systemPath() + QDir::separator() + relativePath;	// system path
//    qDebug( "systempath: %s", qPrintable( systempath ) );


    QString fullpath = systempath;
    if ( QFile::exists( localpath ) ) {
        fullpath = localpath;
    }
//    qDebug( "Using path: %s", qPrintable( fullpath ) );

    return QDir( fullpath ).canonicalPath(); 
}

QStringList MarbleDirs::entryList( const QString& relativePath, QDir::Filters filters )
{
    QStringList filesLocal = QDir( MarbleDirs::localPath() + '/' + relativePath ).entryList(filters);
    QStringList filesSystem = QDir( MarbleDirs::systemPath() + '/' + relativePath ).entryList(filters);
    QStringList allFiles( filesLocal );
    allFiles << filesSystem;

    // remove duplicate entries
    allFiles.sort();
    for ( int i = 1; i < allFiles.size(); ++i ) {
        if ( allFiles.at(i) == allFiles.at( i - 1 ) ) {
            allFiles.removeAt(i);
            --i;
        }
    }

    for (int i = 0; i < allFiles.size(); ++i)
       qDebug() << "Files: " << allFiles.at(i);

    return allFiles;
}

QString MarbleDirs::systemPath()
{
    QString systempath;

#ifdef Q_OS_MACX
    //
    // On OSX lets try to find any file first in the bundle
    // before branching out to home and sys dirs
    //
    CFURLRef myBundleRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFStringRef myMacPath = CFURLCopyFileSystemPath(myBundleRef, kCFURLPOSIXPathStyle);
    const char *mypPathPtr = CFStringGetCStringPtr(myMacPath,CFStringGetSystemEncoding());
    CFRelease(myBundleRef);
    CFRelease(myMacPath);
    QString myPath(mypPathPtr);
    //do some magick so that we can still find data dir if
    //marble was not built as a bundle
    if (myPath.contains(".app"))  //its a bundle!
    {
      systempath = myPath + "/Contents/Resources/data";
    }
/*
    // tackat: If I understood tim correctly this should
    //         now be obsolete, right?
    else //must be running from a non .app bundle
    {
      systempath = QApplication::applicationDirPath()+"/../share/data";
    }
*/
    if ( QFile::exists( systempath ) ){ 
      return systempath;
    }
#endif   // mac bundle

// Should this happen before the Mac bundle already?
if ( !runTimeMarbleDataPath.isEmpty() )
    return runTimeMarbleDataPath;

#ifdef MARBLE_DATA_PATH
    //MARBLE_DATA_PATH is a compiler define set by cmake
    QString compileTimeMarbleDataPath(MARBLE_DATA_PATH);
//    qDebug( "%s <-- marble data path", qPrintable( compileTimeMarbleDataPath ) );
    if(QDir(compileTimeMarbleDataPath).exists())
        return compileTimeMarbleDataPath;
#endif  // MARBLE_DATA_PATH

    return QDir( QCoreApplication::applicationDirPath() 

#if defined(QTONLY)
                     + QLatin1String( "/data" )
#else
                     + QLatin1String( "/../share/apps/marble/data" )
#endif
                     ).canonicalPath();
}

QString MarbleDirs::localPath() 
{ 
    return QString( QDir::homePath() + "/.marble/data" ); // local path
}

QString MarbleDirs::marbleDataPath()
{
    return runTimeMarbleDataPath;
}

void MarbleDirs::setMarbleDataPath( const QString& adaptedPath )
{
    if ( !QDir::root().exists( adaptedPath ) )
    {
        qDebug( "WARNING: Invalid MarbleDataPath %s. Using builtin path instead.", 
                qPrintable( adaptedPath ) );
        return;
    }

    runTimeMarbleDataPath = adaptedPath;
}

void MarbleDirs::debug()
{
    qDebug() << "=== MarbleDirs: ===";
    qDebug() << "Local Path:" << localPath();
    qDebug() << "Marble Data Path (Run Time) :" << runTimeMarbleDataPath; 
    qDebug() << "Marble Data Path (Compile Time):" << QString(MARBLE_DATA_PATH); 
    qDebug() << "System Path:" << systemPath();
    qDebug() << "===================";
}
