//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//


#include "MarbleDirs.h"
#include "MarbleDebug.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtGui/QApplication>

#include <stdlib.h>

#ifdef Q_OS_MACX
//for getting app bundle path
#include <ApplicationServices/ApplicationServices.h>
#endif

#ifdef Q_OS_WIN
//for getting appdata path
//mingw-w64 Internet Explorer 5.01
#define _WIN32_IE 0x0501
#include <shlobj.h>
#endif

#include <config-marble.h>

using namespace Marble;

namespace
{
    QString runTimeMarbleDataPath = "";

    QString runTimeMarblePluginPath = "";
}

MarbleDirs::MarbleDirs()
    : d( 0 )
{
}


QString MarbleDirs::path( const QString& relativePath )
{ 
    QString  localpath = localPath() + '/' + relativePath;	// local path
    QString  systempath  = systemPath() + '/' + relativePath;	// system path


    QString fullpath = systempath;
    if ( QFile::exists( localpath ) ) {
        fullpath = localpath;
    }
    return QDir( fullpath ).canonicalPath(); 
}


QString MarbleDirs::pluginPath( const QString& relativePath )
{ 
    QString  localpath = pluginLocalPath() + QDir::separator() + relativePath;    // local path
    QString  systempath  = pluginSystemPath() + QDir::separator() + relativePath; // system path


    QString fullpath = systempath;
    if ( QFile::exists( localpath ) ) {
        fullpath = localpath;
    }

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

    return allFiles;
}

QStringList MarbleDirs::pluginEntryList( const QString& relativePath, QDir::Filters filters )
{
    QStringList filesLocal = QDir( MarbleDirs::pluginLocalPath() + '/' + relativePath ).entryList(filters);
    QStringList filesSystem = QDir( MarbleDirs::pluginSystemPath() + '/' + relativePath ).entryList(filters);
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

QString MarbleDirs::pluginSystemPath()
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
      systempath = myPath + "/Contents/Resources/plugins";
    }

    if ( QFile::exists( systempath ) ){ 
      return systempath;
    }
#endif   // mac bundle

// Should this happen before the Mac bundle already?
if ( !runTimeMarblePluginPath.isEmpty() )
    return runTimeMarblePluginPath;

#ifdef MARBLE_PLUGIN_PATH
    //MARBLE_PLUGIN_PATH is a compiler define set by cmake
    QString compileTimeMarblePluginPath(MARBLE_PLUGIN_PATH);

    if(QDir(compileTimeMarblePluginPath).exists())
        return compileTimeMarblePluginPath;
#endif  // MARBLE_PLUGIN_PATH

    return QDir( QCoreApplication::applicationDirPath() 

#if defined(QTONLY)
                     + QLatin1String( "/plugins" )
#else
                     + QLatin1String( "/../lib/kde4/plugins/marble" )
#endif
                     ).canonicalPath();
}

QString MarbleDirs::localPath() 
{
#ifndef Q_OS_WIN
    QString dataHome = getenv( "XDG_DATA_HOME" );
    if( dataHome.isEmpty() )
        dataHome = QDir::homePath() + "/.local/share";

    return dataHome + "/marble"; // local path
#else
    HWND hwnd = 0;
    WCHAR *appdata_path = new WCHAR[MAX_PATH+1];
    
    SHGetSpecialFolderPathW( hwnd, appdata_path, CSIDL_APPDATA, 0 );
    QString appdata = QString::fromUtf16( reinterpret_cast<ushort*>( appdata_path ) );
    delete[] appdata_path;
    return QString( QDir::fromNativeSeparators( appdata ) + "/.marble/data" ); // local path
#endif
}

QStringList MarbleDirs::oldLocalPaths()
{
    QStringList possibleOldPaths;

#ifndef Q_OS_WIN
    QString oldDefault = QDir::homePath() + "/.marble/data";
    possibleOldPaths.append( oldDefault );

    QString xdgDefault = QDir::homePath() + "/.local/share/marble";
    possibleOldPaths.append( xdgDefault );

    QString xdg = getenv( "XDG_DATA_HOME" );
    xdg += "/marble/";
    possibleOldPaths.append( xdg );
#endif
    QString currentLocalPath = QDir( MarbleDirs::localPath() ).canonicalPath();
    QStringList oldPaths;
    foreach( const QString& possibleOldPath, possibleOldPaths ) {
        if( !QDir().exists( possibleOldPath ) ) {
            continue;
        }

        QString canonicalPossibleOldPath = QDir( possibleOldPath ).canonicalPath();
        if( canonicalPossibleOldPath == currentLocalPath ) {
            continue;
        }

        oldPaths.append( canonicalPossibleOldPath );
    }

    return oldPaths;
}

QString MarbleDirs::pluginLocalPath() 
{
#ifndef Q_OS_WIN
    return QString( QDir::homePath() + "/.marble/plugins" ); // local path
#else
    HWND hwnd = 0;
    WCHAR *appdata_path = new WCHAR[MAX_PATH+1];
    
    SHGetSpecialFolderPathW( hwnd, appdata_path, CSIDL_APPDATA, 0 );
    QString appdata = QString::fromUtf16( reinterpret_cast<ushort*>( appdata_path ) );
    delete[] appdata_path;
    return QString( QDir::fromNativeSeparators( appdata ) + "/.marble/plugins" ); // local path
#endif
}

QString MarbleDirs::marbleDataPath()
{
    return runTimeMarbleDataPath;
}

QString MarbleDirs::marblePluginPath()
{
    return runTimeMarblePluginPath;
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

void MarbleDirs::setMarblePluginPath( const QString& adaptedPath )
{
    if ( !QDir::root().exists( adaptedPath ) )
    {
        qDebug( "WARNING: Invalid MarblePluginPath %s. Using builtin path instead.", 
                qPrintable( adaptedPath ) );
        return;
    }

    runTimeMarblePluginPath = adaptedPath;
}


void MarbleDirs::debug()
{
    mDebug() << "=== MarbleDirs: ===";
    mDebug() << "Local Path:" << localPath();
    mDebug() << "Plugin Local Path:" << pluginLocalPath();
    mDebug() << "";
    mDebug() << "Marble Data Path (Run Time) :" << runTimeMarbleDataPath; 
    mDebug() << "Marble Data Path (Compile Time):" << QString(MARBLE_DATA_PATH); 
    mDebug() << "";
    mDebug() << "Marble Plugin Path (Run Time) :" << runTimeMarblePluginPath; 
    mDebug() << "Marble Plugin Path (Compile Time):" << QString(MARBLE_PLUGIN_PATH); 
    mDebug() << "";
    mDebug() << "System Path:" << systemPath();
    mDebug() << "Plugin System Path:" << pluginSystemPath();
    mDebug() << "===================";
}
