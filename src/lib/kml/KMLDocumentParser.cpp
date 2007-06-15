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

#include "KMLPlaceMarkParser.h"

namespace
{
    const QString PLACEMARK_TAG = "placemark";
}

KMLDocumentParser::KMLDocumentParser( KMLDocument& document )
  : m_document( document ),
    m_currentParser( 0 )
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
}

bool KMLDocumentParser::startElement( const QString& namespaceUri,
                                      const QString& localName,
                                      const QString& name,
                                      const QXmlAttributes& atts)
{
    bool result = false;

    if ( m_currentParser != 0 ) {
        result = m_currentParser->startElement( namespaceUri, localName, name, atts );

        if ( result ) {
            return result;
        }
    }

    QString elementName = name.toLower();

    if ( elementName == PLACEMARK_TAG ) {
        //TODO: Use factory method to get parser object
        switchCurrentParser( new KMLPlaceMarkParser ());
        bool result = m_currentParser->startElement( namespaceUri, localName, name, atts );
    }
    else {
        qDebug("KMLDocumentParser::startElement(). Unsupported start tag");
    }

    qDebug("KMLDocumentParser::startElement(). Result: %d", result);
    return true;
}

bool KMLDocumentParser::endElement( const QString& namespaceUri,
                                    const QString& localName,
                                    const QString& qName )
{
    bool result = false;

    if ( m_currentParser != 0 ) {
        result = m_currentParser->endElement( namespaceUri, localName, qName );

        if ( result ) {
            return result;
        }
        else {
            /* TODO:
             * If parser completely parsed it's object then get object first
             */

             /*
              * Current parser completed it's job
              * Remove current parser and pop from stack
              * if another parser is availebl
              */
             delete m_currentParser;
             m_currentParser = 0;

             if ( m_parserStack.count() > 0 ) {
                m_currentParser = m_parserStack.pop();
                result = m_currentParser->endElement( namespaceUri, localName, qName );
             }
        }
    }

    qDebug("KMLDocumentParser::endElement(). Result: %d", result);
    return true;
}

bool KMLDocumentParser::characters( const QString& ch )
{
    bool result = false;

    if ( m_currentParser != 0 ) {
        result = m_currentParser->characters (ch);
    }

    if ( ! result ) {
        qDebug("KMLDocumentParser::characters. Current parser is NULL");
    }

    return true;
}

void KMLDocumentParser::switchCurrentParser( KMLObjectParser* parser )
{
    if ( m_currentParser != 0 ) {
        m_parserStack.push( m_currentParser );
    }

    m_currentParser = parser;
}
