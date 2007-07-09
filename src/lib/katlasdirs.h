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


#ifndef KATLASDIRS_H
#define KATLASDIRS_H


#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtGui/QApplication>


class KAtlasDirs
{
 public:
    static QString path( const QString& path ) 
    { 
        QString  fullpath  = systemDir() + "/" + path;	// system path
        QString  localpath = localDir() + "/" + path;	// local path
        QString  unixpath  = unixDir() + "/" + path;	// unix path
	
        if ( QFile::exists( localpath ) )
            fullpath = localpath;
        else if ( QFile::exists( unixpath ) )
            fullpath = unixpath;

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


#endif // KATLASDIRS_H
