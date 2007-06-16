//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "KMLDocument.h"

#include <QtXml/QXmlInputSource>
#include <QtXml/QXmlSimpleReader>

#include "KMLDocumentParser.h"
#include "PlaceMarkContainer.h"


KMLDocument::KMLDocument()
{
}

void KMLDocument::load( QIODevice& source )
{
    /*
     * 1. create KMLParser
     * 2. create QXMLSimpleReader
     * 3. start to parsing input stream
     */

    KMLDocumentParser parser( *this );
    QXmlInputSource xmlDocumentSource( &source );

    QXmlSimpleReader reader;
    reader.setContentHandler( &parser );
    reader.setErrorHandler( &parser );
    bool result = reader.parse( &xmlDocumentSource );

    if ( ! result ) {
        qDebug("KMLDocument::load( QIODevice& ). Error while parsing xml source");
    }
}
