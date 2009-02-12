//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Tobias Koenig <tokoe@kde.org>"
//


// Own
#include "FileStoragePolicy.h"

// Qt
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

// Marble
#include "MarbleDirs.h"

using namespace Marble;

FileStoragePolicy::FileStoragePolicy( const QString &dataDirectory )
    : m_dataDirectory( dataDirectory )
{
    if ( m_dataDirectory.isEmpty() )
        m_dataDirectory = MarbleDirs::localPath() + "/cache/";
 
    if ( ! QDir( m_dataDirectory ).exists() ) 
        QDir::root().mkpath( m_dataDirectory );
}

FileStoragePolicy::~FileStoragePolicy()
{
}

bool FileStoragePolicy::fileExists( const QString &fileName ) const
{
    const QString fullName( m_dataDirectory + '/' + fileName );

    return QFile::exists( fullName );
}

bool FileStoragePolicy::updateFile( const QString &fileName, const QByteArray &data )
{
    const QString fullName( m_dataDirectory + '/' + fileName );

    // Create directory if it doesn't exist yet...
    QFileInfo info( fullName );
    
    const QDir localFileDir = info.dir();
    const QString localFileDirPath = localFileDir.absolutePath();

    if ( !QDir( localFileDirPath ).exists() )
        QDir::root().mkpath( localFileDirPath );

    // ... and save the file content
    QFile file( fullName );
    if ( !file.open( QIODevice::WriteOnly ) ) {
        m_errorMsg = QString( "%1: %2" ).arg( fullName ).arg( file.errorString() );
        // qDebug() << "file.open" << m_errorMsg;
        return false;
    }

    if ( !file.write( data ) ) {
        m_errorMsg = QString( "%1: %2" ).arg( fullName ).arg( file.errorString() );
        // qDebug() << "file.write" << m_errorMsg;
        return false;
    }

    file.close();

    return true;
}

void FileStoragePolicy::clearCache()
{
    if ( m_dataDirectory.isEmpty() || !m_dataDirectory.endsWith("data") )
    {
        qDebug() << "Error: Refusing to erase files under unknown conditions for safety reasons!";
        return;
    }

    QString cachedMapsDirectory = m_dataDirectory + "/maps";

    QDirIterator it( cachedMapsDirectory, QDir::NoDotAndDotDot | QDir::Dirs );
    qDebug() << cachedMapsDirectory;
    while (it.hasNext()) {
        it.next();
        QString planetDirectory = it.filePath();
        QDirIterator itPlanet( planetDirectory, QDir::NoDotAndDotDot | QDir::Dirs );
        while (itPlanet.hasNext()) {
            itPlanet.next();
            QString themeDirectory = itPlanet.filePath();
            QDirIterator itTheme( themeDirectory, QDir::NoDotAndDotDot | QDir::Dirs );
            while (itTheme.hasNext()) {
                itTheme.next();
                QString tileDirectory = itTheme.filePath();

                if ( itTheme.fileName().toInt() < 5 ) {
                    continue;
                }

                QDirIterator itTile( tileDirectory, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories );
                while (itTile.hasNext()) {
                    itTile.next();
                    QString filePath = itTile.filePath();

                    // We try to be very careful and just delete images
                    if ( filePath.toLower().endsWith( ".jpg" ) 
                      || filePath.toLower().endsWith( ".png" )
                      || filePath.toLower().endsWith( ".gif" )
                      || filePath.toLower().endsWith( ".svg" )
                    )
                    {
                        QFile::remove( filePath );
                    }
                }
            }
        }
    }
}

QString FileStoragePolicy::lastErrorMessage() const
{
    return m_errorMsg;
}
