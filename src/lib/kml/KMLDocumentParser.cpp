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
#include "KMLStyle.h"
#include "KMLStyleParser.h"

namespace
{
    const QString DOCUMENT_TAG  = "document";
    const QString KML_TAG       = "kml";
    const QString STYLE_TAG     = "style";
}

KMLDocumentParser::KMLDocumentParser( KMLDocument& document )
  : KMLContainerParser( document ),
    m_parsed( false ),
    m_currentParser( 0 ),
    m_currentStyle( 0 )
{
}

KMLDocumentParser::~KMLDocumentParser()
{
    delete m_currentParser;
    delete m_currentStyle;
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

    bool result = false;

    if ( m_currentParser != 0 ) {
        result = m_currentParser->startElement( namespaceUri, localName, name, atts );
    }
    else {
        result = KMLContainerParser::startElement( namespaceUri, localName, name, atts);
    }

    if ( ! result ) {
        QString lowerName = name.toLower();

        if ( lowerName == KML_TAG ) {
            result = true;

            // FIXME: remove this hack
            m_level--;
        }
        else if ( lowerName == DOCUMENT_TAG ) {
            result = true;
        }
        else if ( lowerName == STYLE_TAG ) {
            if ( m_currentParser != 0 ) {
                delete m_currentParser;
                delete m_currentStyle;
            }

            m_currentStyle = new KMLStyle();
            m_currentParser = new KMLStyleParser( *m_currentStyle );
            result = m_currentParser->startElement( namespaceUri, localName, name, atts );
        }
    }

    return true;
}

bool KMLDocumentParser::endElement( const QString& namespaceUri,
                                    const QString& localName,
                                    const QString& qName )
{
    if ( m_parsed ) {
        return false;
    }

    bool result = false;

    if ( m_currentParser != 0 ) {
        result = m_currentParser->endElement( namespaceUri, localName, qName );

        if ( result ) {
            if ( m_currentParser->isParsed() ) {
                delete m_currentParser;
                m_currentParser = 0;

                KMLDocument& document = (KMLDocument&) m_object;
                document.addStyle( m_currentStyle );
                m_currentStyle = 0;
            }
        }
    }
    else {
        result = KMLContainerParser::endElement( namespaceUri, localName, qName );
    }

    if ( ! result ) {
        QString lowerName = qName.toLower();

        if ( lowerName == KML_TAG ) {
            result = true;

            // should remove this hack
            m_level++;
        }
        else if ( lowerName == DOCUMENT_TAG ) {
            m_parsed = true;
            result = true;
        }
    }

    m_level--;

    return true;
}

bool KMLDocumentParser::characters( const QString& ch )
{
    if ( m_parsed ) {
        return false;
    }

    if ( m_currentParser != 0 ) {
        m_currentParser->characters( ch );
    }
    else {
        KMLContainerParser::characters( ch );
    }

    //why not return result?
    return true;
}
