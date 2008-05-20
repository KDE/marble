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
#include <QtCore/QBuffer>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtGui/QImage>

// Marble
#include "MarbleDirs.h"


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
    QByteArray const * convertedData = &data;

    // Create directory if it doesn't exist yet...
    QFileInfo info( fullName );
    
    const QDir localFileDir = info.dir();
    const QString localFileDirPath = localFileDir.absolutePath();

    if ( !QDir( localFileDirPath ).exists() )
        QDir::root().mkpath( localFileDirPath );

    // convert images with format QImage::Format_Indexed8
    QImage image;
    QByteArray ba;
    if ( image.loadFromData( data ) && image.format() == QImage::Format_Indexed8 ) {
        qDebug() << "FileStoragePolicy::updateFile converting" << fullName;
        QImage convertedImage = image.convertToFormat( QImage::Format_RGB32 );
        qDebug() << "convertedImage.depth():" << convertedImage.depth();
        QBuffer buffer( &ba );
        buffer.open( QIODevice::WriteOnly );
        convertedImage.save( &buffer );
        convertedData = &ba;
    }

    // ... and save the file content
    QFile file( fullName );
    if ( !file.open( QIODevice::WriteOnly ) ) {
        m_errorMsg = QString( "%1: %2" ).arg( fullName ).arg( file.errorString() );
        return false;
    }

    if ( !file.write( *convertedData ) ) {
        m_errorMsg = QString( "%1: %2" ).arg( fullName ).arg( file.errorString() );
        return false;
    }

    file.close();

    return true;
}

QString FileStoragePolicy::lastErrorMessage() const
{
    return m_errorMsg;
}
