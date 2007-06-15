//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//

#include "KMLDocumentParser.h"

#include <QtCore/QDebug>
#include "KMLPlaceMarkParser.h"

namespace
{
    const QString PLACEMARK_TAG = "placemark";
}

KMLDocumentParser::KMLDocumentParser( KMLObject& document )
  : KMLContainerParser( document )
{
}

bool KMLDocumentParser::startDocument()
{
    qDebug("Start KML-import");
    return true;
}

bool KMLDocumentParser::endDocument()
{
    qDebug("Reached end of document");
    return true;
}

bool KMLDocumentParser::startElement( const QString& namespaceUri,
                                      const QString& localName,
                                      const QString& name,
                                      const QXmlAttributes& atts)
{
    /*
     * Document specific fields will parse will in a feature
     * i.e. list of StyleSelector, Schema objects
     */
    bool result = KMLContainerParser::startElement( namespaceUri, localName, name, atts);

    if ( ! result ) {
        qDebug("KMLDocumentParser::startElement(). Unsupported tag");
        qDebug() << name;
    }

    return true;
}

bool KMLDocumentParser::endElement( const QString& namespaceUri,
                                    const QString& localName,
                                    const QString& qName )
{
    bool result = KMLContainerParser::endElement( namespaceUri, localName, qName );

    qDebug("KMLDocumentParser::endElement(). Result: %d", result);
    return true;
}

bool KMLDocumentParser::characters( const QString& ch )
{
    bool result = KMLContainerParser::characters( ch );

    qDebug("KMLDocumentParser::characters. Result: %d", result);
    return true;
}
