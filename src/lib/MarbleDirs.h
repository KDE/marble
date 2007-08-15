//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#ifndef MARBLEDIRS_H
#define MARBLEDIRS_H


#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtGui/QApplication>


#ifdef Q_OS_MACX
//for getting app bundle path
#include <ApplicationServices/ApplicationServices.h>
#endif

#include <config-marble.h>

/**
 * @short A class that manages data look-up for Marble.
 *
 * The class basically does for Marble what KStandardDirs does for KDE.
 * Given that the MarbleWidget is a Qt4-only library and given that it 
 * comes with its own model and data we need this class.
 *
 * The class needs to respect the requirements of the different plattforms,
 * so to avoid very different implementations for each plattform we 
 * specify how data should get looked up:
 * 
 * Generally there are two places of which Marble will draw it's data from:
 * 
 * "localDir" and "systemDir".
 * 
 * look-up of the data should happen in the localDir first.
 * Only if the look-up in the localDir failed then MarbleDirs should 
 * look up the data in the systemDir.
 * 
 * homeDir:
 * The place for homeDir should match space that is fully accessible to 
 * the user. On Unix-like plattforms this matches 
 * QDir::homePath() + "/.marble/data"
 * 
 * systemDir:
 * Initially the systemDir should match the place where cmake installed the 
 * data for marble. It should be possible to change that location using
 * the cmake MARBLE_DATA_PATH option at compile time.
 * 
 * To allow kiosk-like setups and for custom setups in general
 * it should be possible to change the place of the systemDir at runtime. 
 * Therefore we introduce a global variable "MarbleDataPath" in the 
 * MarbleDirs.h source code.
 * Initially MarbleDataPath is empty. The systemDir will point to 
 * MarbleDataPath as soon as it gets changed to a valid non-empty path. So 
 * as soon as MarbleDataPath contains a valid path the path specified by 
 * cmake will get ignored.

 * It's the duty of each application that uses the MarbleWidget to retrieve 
 * the value of the MarbleDataPath from the MarbleWidget and to save it 
 * in its settings and restore it on start-up of the application.
 * 
 */

class MarbleDirs
{
 public:
    static QString path( const QString& path ) 
    { 
        //MARBLE_DATA_PATH is a compiler define set by cmake
        QString marbleDataPath(MARBLE_DATA_PATH);
//        qDebug(marbleDataPath.toLocal8Bit() + " <-- marble data path");
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
        QString  systempath  = systemDir() + "/" + path;	// system path
        QString  localpath = localDir() + "/" + path;	// local path
        QString  unixpath  = unixDir() + "/" + path;	// unix path
	
        fullpath = unixpath;
        if ( QFile::exists( localpath ) )
            fullpath = localpath;
        else if ( QFile::exists( systempath ) )
            fullpath = systempath;

        return QDir( fullpath ).canonicalPath(); 
    }

    static QString systemDir() 
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

    static QString localDir() 
    { 
        return QString( QDir::homePath() + "/.marble/data" ); // local path
    }

    static QString unixDir()
    {
        return QString( "/usr/local/share/apps/marble/data" );	// unix system path
    }
};


#endif // MARBLEDIRS_H
