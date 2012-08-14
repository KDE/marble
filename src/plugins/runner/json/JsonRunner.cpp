/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright 2012 Ander Pijoan <ander.pijoan@deusto.es>
*/

#include "JsonRunner.h"
#include "JsonParser.h"

#include "GeoDataDocument.h"
#include "MarbleDebug.h"

#include <QtCore/QFile>
#include <QFileInfo>

namespace Marble
{

JsonRunner::JsonRunner(QObject *parent) :
    MarbleAbstractRunner(parent)
{
}

JsonRunner::~JsonRunner()
{
}

GeoDataFeature::GeoDataVisualCategory JsonRunner::category() const
{
    return GeoDataFeature::Folder;
}

void JsonRunner::parseFile( const QString &fileName, DocumentRole role = UnknownDocument )
{
    // Check file exists
    QFile file( fileName );
    if ( !file.exists() ) {
        qWarning() << "File" << fileName << "does not exist!";
        emit parsingFinished( 0 );
        return;
    }

    // Check file extension
    QFileInfo fileinfo( fileName );
    if( fileinfo.suffix().compare( "js", Qt::CaseInsensitive ) != 0 ) {
        emit parsingFinished( 0 );
        return;
    }

    // Open file in right mode
    file.open( QIODevice::ReadOnly );

    // Create parser
    JsonParser parser;

    // Start parsing
    if ( !parser.read( &file ) ) {
        emit parsingFinished( 0, "Could not parse json" );
        return;
    }


    GeoDocument* document = parser.releaseDocument();
    file.close();
    GeoDataDocument* doc = 0;
    if (document) {
        doc = static_cast<GeoDataDocument*>( document );
        doc->setDocumentRole( role );
    }

    emit parsingFinished( doc );
}

}

#include "JsonRunner.moc"
