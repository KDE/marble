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
    qDebug() << "Trying to open" << kmz;

    MarbleZipReader zip( kmz );
    if ( zip.status() != MarbleZipReader::NoError ) {
        qDebug() << "Failed to extract " << kmz << ": error code " << zip.status();
        return false;
    }

    QTemporaryFile outputDir ( QDir::tempPath() + "/marble-kmz-XXXXXX" );
    outputDir.setAutoRemove( false );
    outputDir.open();
    if ( !QFile::remove( outputDir.fileName() ) || !QDir("/").mkdir( outputDir.fileName() ) ) {
        qDebug() << "Failed to create temporary storage for extracting " << kmz;
        return false;
    }

    m_kmzPath = outputDir.fileName() + '/';
    qDebug() << "Extracting all to " << m_kmzPath << ", and all are " << zip.fileInfoList().size() << " items";
    if (!zip.extractAll( m_kmzPath ))
    {
        qDebug() << "Failed to extract kmz file contents to " << m_kmzPath;
        return false;
    }
    else
    {
        qDebug() << "Everythign extracted: " << QDir(m_kmzPath).entryList();
    }

    foreach(const MarbleZipReader::FileInfo &fileInfo, zip.fileInfoList()) {
        //if (!fileInfo.isFile) {
        //    continue;
        //}
        QString file = outputDir.fileName() + '/' + fileInfo.filePath;
        qDebug() << "Appending " << file;
        m_kmzFiles << fileInfo.filePath;
        if (file.endsWith(".kml", Qt::CaseInsensitive)) {
            if ( !m_kmlFile.isEmpty() ) {
                qDebug() << "File" << kmz << "contains more than one .kml files";
            }
            m_kmlFile = file;
        }
        qDebug() << "kml file is now " << m_kmlFile;
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
