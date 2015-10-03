//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#include "KmlRunner.h"

#include "GeoDataDocument.h"
#include "KmlParser.h"
#include "KmlDocument.h"
#include "MarbleDebug.h"

#ifdef MARBLE_HAVE_QUAZIP
#include "KmzHandler.h"
#endif

#include <QFile>
#include <QFileInfo>

namespace Marble
{

KmlRunner::KmlRunner(QObject *parent) :
    ParsingRunner(parent)
{
}

KmlRunner::~KmlRunner()
{
}

GeoDataDocument *KmlRunner::parseFile(const QString &fileName, DocumentRole role, QString &error)
{
    QString kmlFileName = fileName;
    QString kmzPath;
    QStringList kmzFiles;

#ifdef MARBLE_HAVE_QUAZIP
    QFileInfo const kmzFile( fileName );
    if ( kmzFile.exists() && kmzFile.suffix().toLower() == "kmz" ) {
        KmzHandler kmzHandler;
        if ( kmzHandler.open( fileName ) ) {
            kmlFileName = kmzHandler.kmlFile();
            kmzPath = kmzHandler.kmzPath();
            kmzFiles = kmzHandler.kmzFiles();
        } else {
            error = QString("File %1 is not a valid .kmz file").arg(fileName);
            mDebug() << error;
            return nullptr;
        }
    }
#endif

    QFile  file( kmlFileName );
    if ( !file.exists() ) {
        error = QString("File %1 does not exist").arg(fileName);
        mDebug() << error;
        return nullptr;
    }

    // Open file in right mode
    file.open( QIODevice::ReadOnly );

    KmlParser parser;

    if ( !parser.read( &file ) ) {
        error = parser.errorString();
        mDebug() << error;
        return nullptr;
    }
    GeoDocument* document = parser.releaseDocument();
    Q_ASSERT( document );
    KmlDocument* doc = static_cast<KmlDocument*>( document );
    doc->setDocumentRole( role );
    doc->setFileName( fileName );
    doc->setBaseUri( kmlFileName );
    doc->setFiles( kmzPath, kmzFiles );

    file.close();
    return doc;
}

}

#include "moc_KmlRunner.cpp"
