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

QString MarbleDirs::path( const QString& path )
{ 
    //MARBLE_DATA_PATH is a compiler define set by cmake
    QString marbleDataPath(MARBLE_DATA_PATH);
//    qDebug(marbleDataPath.toLocal8Bit() + " <-- marble data path");
    QString fullpath;
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
      fullpath = myPath + "/Contents/MacOS/resources/data/";
    }
    else //must be running from a non .app bundle
    {
      fullpath = QApplication::applicationDirPath()+"/../share/data/";
    }
    //qDebug("KatlasDirs path calculated as: " + fullpath.toLocal8Bit());
    fullpath += QDir::separator() + path;
    if ( QFile::exists( fullpath ) ){
        return QDir( fullpath ).canonicalPath(); 
    }
#endif
    QString  systempath  = systemDir() + "/" + path;    // system path
    QString  localpath = localDir() + "/" + path;   // local path

    fullpath = systempath;
    if ( QFile::exists( localpath ) )
        fullpath = localpath;

    return QDir( fullpath ).canonicalPath(); 
}


QString MarbleDirs::systemDir()
{
        return QDir( qApp->applicationDirPath() 
#if defined(Q_OS_MACX)
                     + QLatin1String( "/Resources/data" )
#elif defined(Q_OS_UNIX)
                     + QLatin1String( "/../share/apps/marble/data" )
#elif defined(Q_OS_WIN)
 #if defined(QTONLY)
                     + QLatin1String( "/data" )
 #else
                     + QLatin1String( "/../share/apps/marble/data" )
 #endif
#endif
                     ).canonicalPath();
}

QString MarbleDirs::localDir() 
{ 
    return QString( QDir::homePath() + "/.marble/data" ); // local path
}
