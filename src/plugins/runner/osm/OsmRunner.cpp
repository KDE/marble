//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#include "OsmRunner.h"

#include "GeoDataDocument.h"
#include "OsmParser.h"

#include <QtCore/QFile>

namespace Marble
{

OsmRunner::OsmRunner(QObject *parent) :
    ParsingRunner(parent)
{
}

OsmRunner::~OsmRunner()
{
}

void OsmRunner::parseFile( const QString &fileName, DocumentRole role = UnknownDocument )
{
    QFile  file( fileName );
    if ( !file.exists() ) {
        qWarning( "File does not exist!" );
        emit parsingFinished( 0 );
        return;
    }

    // Open file in right mode
    file.open( QIODevice::ReadOnly );

    OsmParser parser;

    if ( !parser.read( &file ) ) {
        emit parsingFinished( 0, parser.errorString() );
        return;
    }
    GeoDocument* document = parser.releaseDocument();
    Q_ASSERT( document );
    GeoDataDocument* doc = static_cast<GeoDataDocument*>( document );
    doc->setDocumentRole( role );

    file.close();
    emit parsingFinished( doc );
}

}

#include "OsmRunner.moc"
