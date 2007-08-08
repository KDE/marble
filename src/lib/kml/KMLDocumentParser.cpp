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

#include "KMLDocument.h"
#include "KMLPlaceMarkParser.h"

namespace
{
    const QString DOCUMENT_TAG = "document";
    const QString KML_TAG = "kml";
}

KMLDocumentParser::KMLDocumentParser( KMLDocument& document )
  : KMLContainerParser( document ),
    m_parsed( false )
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
    if ( m_parsed ) {
        return false;
    }

    m_level++;

    /*
     * Document specific fields will parse will in a feature
     * i.e. list of StyleSelector, Schema objects
     */
    bool result = KMLContainerParser::startElement( namespaceUri, localName, name, atts);

    QString lowerName = name.toLower();

    if ( ! result ) {
        if ( lowerName == KML_TAG ) {
            result = true;

            // should remove this hack
            m_level--;
        }
        else if ( lowerName == DOCUMENT_TAG ) {
            result = true;
        }
    }

//    qDebug("Start - %s. Result: %d", lowerName.toAscii().data(), result);

    return true;
}

bool KMLDocumentParser::endElement( const QString& namespaceUri,
                                    const QString& localName,
                                    const QString& qName )
{
    if ( m_parsed ) {
        return false;
    }

    bool result = KMLContainerParser::endElement( namespaceUri, localName, qName );

    if ( ! result ) {
        QString lowerName = qName.toLower();

        if ( lowerName == KML_TAG ) {
            result = true;

            // should remove this hack
            m_level++;
        }
        if ( lowerName == DOCUMENT_TAG ) {
            m_parsed = true;
            result = true;
        }
    }

    m_level--;

//    qDebug("Stop - %s. Result: %d", qName.toAscii().data(), result);

    return true;
}

bool KMLDocumentParser::characters( const QString& ch )
{
    if ( m_parsed ) {
        return false;
    }

    //Changed by Tim to suport building with -Werror -Wall
    //since unused vars are errors with above flags
    //bool result = KMLContainerParser::characters( ch );
    KMLContainerParser::characters( ch ); 

    //why not return result?
    return true;
}
