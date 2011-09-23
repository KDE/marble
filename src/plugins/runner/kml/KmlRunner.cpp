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

#include <QtCore/QFile>

namespace Marble
{

KmlRunner::KmlRunner(QObject *parent) :
    MarbleAbstractRunner(parent)
{
}

KmlRunner::~KmlRunner()
{
}

GeoDataFeature::GeoDataVisualCategory KmlRunner::category() const
{
    return GeoDataFeature::Folder;
}

void KmlRunner::parseFile( const QString &fileName, DocumentRole role = UnknownDocument )
{
    QFile  file( fileName );
    if ( !file.exists() ) {
        qWarning( "File does not exist!" );
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
    GeoDataDocument* doc = static_cast<GeoDataDocument*>( document );
    doc->setDocumentRole( role );

    file.close();
    emit parsingFinished( doc );
}

}

#include "KmlRunner.moc"
