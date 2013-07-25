//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Dennis Nienhüser <earthwings@gentoo.org>

#include "KmzHandler.h"
#include "MarbleDebug.h"

#include <QTemporaryFile>
#include <QDir>

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

namespace Marble {

bool KmzHandler::open( const QString &kmz )
{
    QuaZip zip( kmz );
    if ( !zip.open( QuaZip::mdUnzip ) ) {
        mDebug() << "Failed to extract " << kmz;
        return false;
    }

    QTemporaryFile outputDir ( QDir::tempPath() + "/marble-kmz-XXXXXX" );
    outputDir.setAutoRemove( false );
    outputDir.open();
    if ( !QFile::remove( outputDir.fileName() ) || !QDir("/").mkdir( outputDir.fileName() ) ) {
        mDebug() << "Failed to create temporary storage for extracting " << kmz;
        return false;
    }

    m_kmzPath = outputDir.fileName();
    QuaZipFile zipFile( &zip );
    for ( bool moreFiles=zip.goToFirstFile(); moreFiles; moreFiles=zip.goToNextFile() ) {
        QFileInfo output = QFileInfo( outputDir.fileName() + "/" + zip.getCurrentFileName() );
        if ( !output.dir().exists() ) {
            QDir::root().mkpath( output.dir().absolutePath() );
        }

        QFile outputFile( output.absoluteFilePath() );
        outputFile.open( QIODevice::WriteOnly );
        zipFile.open( QIODevice::ReadOnly );
        outputFile.write( zipFile.readAll() );
        outputFile.close();
        zipFile.close();
        m_kmzFiles << output.absoluteFilePath();

        if ( output.suffix().toLower() == "kml" ) {
            if ( !m_kmlFile.isEmpty() ) {
                mDebug() << "File" << kmz << "contains more than one .kml files";
            }
            m_kmlFile = output.absoluteFilePath();
        }
    }
    zip.close();
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
