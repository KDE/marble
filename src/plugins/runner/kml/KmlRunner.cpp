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

void KmlRunner::parseFile( const QString &fileName, DocumentRole role = UnknownDocument )
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
            qWarning() << "File " << fileName << " is not a valid .kmz file";
            emit parsingFinished( 0 );
            return;
        }
    }
#endif

    QFile  file( kmlFileName );
    if ( !file.exists() ) {
        qWarning() << "File" << kmlFileName << "does not exist!";
        emit parsingFinished( 0 );
        return;
    }

    // Open file in right mode
    file.open( QIODevice::ReadOnly );

    KmlParser parser;

    if ( !parser.read( &file ) ) {
        emit parsingFinished( 0, parser.errorString() );
        return;
    }
    GeoDocument* document = parser.releaseDocument();
    Q_ASSERT( document );
    KmlDocument* doc = static_cast<KmlDocument*>( document );
    doc->setDocumentRole( role );
    doc->setFileName( fileName );
    doc->setBaseUri( kmlFileName );
    doc->setFiles( kmzPath, kmzFiles );

    file.close();
    emit parsingFinished( doc );
}

}

#include "KmlRunner.moc"
