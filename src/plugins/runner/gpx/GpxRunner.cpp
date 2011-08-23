//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#include "GpxRunner.h"

#include "GeoDataDocument.h"
#include "GpxParser.h"

#include <QtCore/QFile>

namespace Marble
{

GpxRunner::GpxRunner(QObject *parent) :
    MarbleAbstractRunner(parent)
{
}

GpxRunner::~GpxRunner()
{
}

GeoDataFeature::GeoDataVisualCategory GpxRunner::category() const
{
    return GeoDataFeature::Folder;
}

void GpxRunner::parseFile( const QString &fileName, DocumentRole role = UnknownDocument )
{
    QFile  file( fileName );
    if ( !file.exists() ) {
        qWarning( "File does not exist!" );
        return;
    }

    // Open file in right mode
    file.open( QIODevice::ReadOnly );

    GpxParser parser;

    if ( !parser.read( &file ) ) {
        emit parsingFinished( 0 );
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

#include "GpxRunner.moc"
