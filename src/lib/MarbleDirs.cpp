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

#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtGui/QApplication>

#ifdef Q_OS_MACX
//for getting app bundle path
#include <ApplicationServices/ApplicationServices.h>
#endif

#include <config-marble.h>

namespace
{
    QString runTimeMarbleDataPath;
}

QString MarbleDirs::path( const QString& relativePath )
{ 
    QString  localpath = localPath() + QDir::separator() + relativePath;	// local path
    qDebug( "localpath: %s", qPrintable( localpath ) );
    QString  systempath  = systemPath() + QDir::separator() + relativePath;	// system path
    qDebug( "systempath: %s", qPrintable( systempath ) );


    QString fullpath = systempath;
    if ( QFile::exists( localpath ) ) {
        fullpath = localpath;
    }
    qDebug( "Using path: %s", qPrintable( fullpath ) );

    return QDir( fullpath ).canonicalPath(); 
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
    else //must be running from a non .app bundle
    {
      systempath = QApplication::applicationDirPath()+"/../share/data";
    }
    if ( QFile::exists( systempath ) ){ 
      return systempath;
    }
#endif   // mac

// Should this happen before the Mac bundle already?
if ( !runTimeMarbleDataPath.isEmpty() )
    return runTimeMarbleDataPath;

#if !defined(Q_OS_WIN)
    //MARBLE_DATA_PATH is a compiler define set by cmake
    QString compileTimeMarbleDataPath(MARBLE_DATA_PATH);
    qDebug(compileTimeMarbleDataPath.toLocal8Bit() + " <-- marble data path");

    return compileTimeMarbleDataPath;
#endif

return QDir( qApp->applicationDirPath() 

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
    runTimeMarbleDataPath = adaptedPath;
}
