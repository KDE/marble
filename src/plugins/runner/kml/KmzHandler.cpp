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

#include <QDir>
#include <QUuid>

namespace Marble {

bool KmzHandler::open(const QString &kmz, QString &error)
{
    MarbleZipReader zip( kmz );
    if ( zip.status() != MarbleZipReader::NoError ) {
        error = QStringLiteral("Failed to extract %1: error code %2").arg(kmz).arg(zip.status());
		mDebug() << error;
		return false;
    }

	QString const uuid = QUuid::createUuid().toString().mid(1, 8);
	QString const filename = QDir::tempPath() + QLatin1String("/marble-kmz-") + uuid;
	if (!QDir::root().mkpath(filename)) {
        error = QStringLiteral("Failed to create temporary storage %1 for extracting %2").arg(filename).arg(kmz);
        mDebug() << error;
        return false;
    }

    m_kmzPath = filename + QLatin1Char('/');
    if (!zip.extractAll( m_kmzPath ))
    {
        error = QStringLiteral("Failed to extract kmz file contents to %1").arg(m_kmzPath);
        mDebug() << error;
        return false;
    }

    foreach(const MarbleZipReader::FileInfo &fileInfo, zip.fileInfoList()) {
        //if (!fileInfo.isFile) {
        //    continue;
        //}
        QString file = filename + QLatin1Char('/') + fileInfo.filePath;
        m_kmzFiles << fileInfo.filePath;
        if (file.endsWith(QLatin1String(".kml"), Qt::CaseInsensitive)) {
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
