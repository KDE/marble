//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Dennis Nienhüser <nienhueser@kde.org>

#include "KmzHandler.h"
#include "MarbleDebug.h"
#include <MarbleZipReader.h>

#include <QTemporaryFile>
#include <QDir>

namespace Marble {

bool KmzHandler::open( const QString &kmz )
{
    MarbleZipReader zip( kmz );
    if ( zip.status() != MarbleZipReader::NoError ) {
        mDebug() << "Failed to extract " << kmz << ": error code " << zip.status();
        return false;
    }

    QTemporaryFile outputDir ( QDir::tempPath() + "/marble-kmz-XXXXXX" );
    outputDir.setAutoRemove( false );
    outputDir.open();
    if ( !QFile::remove( outputDir.fileName() ) || !QDir("/").mkdir( outputDir.fileName() ) ) {
        mDebug() << "Failed to create temporary storage for extracting " << kmz;
        return false;
    }

    m_kmzPath = outputDir.fileName() + '/';
    if (!zip.extractAll( m_kmzPath ))
    {
        mDebug() << "Failed to extract kmz file contents to " << m_kmzPath;
        return false;
    }

    foreach(const MarbleZipReader::FileInfo &fileInfo, zip.fileInfoList()) {
        //if (!fileInfo.isFile) {
        //    continue;
        //}
        QString file = outputDir.fileName() + '/' + fileInfo.filePath;
        m_kmzFiles << fileInfo.filePath;
        if (file.endsWith(".kml", Qt::CaseInsensitive)) {
            if ( !m_kmlFile.isEmpty() ) {
                mDebug() << "File" << kmz << "contains more than one .kml files";
            }
            m_kmlFile = file;
        }
    }
    return true;
}

QString KmzHandler::kmlFile() const
{
    return m_kmlFile;
}

QString KmzHandler::kmzPath() const
{
    return m_kmzPath;
}

QStringList KmzHandler::kmzFiles() const
{
    return m_kmzFiles;
}

}
