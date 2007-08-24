//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "KMLStyleParser.h"
#include "KMLStyle.h"
#include "KMLIconStyleParser.h"

namespace
{
    const QString STYLE_TAG         = "style";
    const QString ICON_STYLE_TAG    = "iconstyle";
}

KMLStyleParser::KMLStyleParser( KMLStyle& style )
  : KMLObjectParser( style ),
    m_currentParser( 0 )
{
}

KMLStyleParser::~KMLStyleParser()
{
    delete m_currentParser;
}

bool KMLStyleParser::startElement( const QString& namespaceURI,
                            const QString& localName,
                            const QString& name,
                            const QXmlAttributes& atts )
{
    if ( m_parsed ) {
        return false;
    }

    bool result = false;

    if ( m_currentParser != 0 ) {
        result = m_currentParser->startElement( namespaceURI, localName, name, atts );
    }

    if ( ! result ) {
        QString lowerName = name.toLower();

        if ( lowerName == STYLE_TAG ) {
            qDebug("Start of style tag. Begining to parse KMLStyle object");

            /*
            * Get styleID
            */
            if ( atts.count() != 0 ) {
                KMLStyleSelector& selector = ( KMLStyleSelector& ) m_object;
                selector.setStyleId( atts.value( "id" ) );
            }

            result = true;
        }
        else if ( lowerName == ICON_STYLE_TAG ) {
            if ( m_currentParser != 0 ) {
                delete m_currentParser;
            }

            KMLStyle& style = ( KMLStyle& ) m_object;
            m_currentParser = new KMLIconStyleParser( style.getIconStyle() );

            result = m_currentParser->startElement( namespaceURI, localName, name, atts );
        }
    }

    return result;
}

bool KMLStyleParser::endElement( const QString& namespaceURI,
                            const QString& localName,
                            const QString& qName )
{
    if ( m_parsed ) {
        return false;
    }

    bool result = false;

    if ( m_currentParser != 0 ) {
        result = m_currentParser->endElement( namespaceURI, localName, qName );

        if ( result ) {
            if ( m_currentParser->isParsed() ) {
                delete m_currentParser;
                m_currentParser = 0;
            }
        }
    }

    if ( ! result ) {
        QString lowerName = qName.toLower();

        if ( lowerName == STYLE_TAG ) {
            m_parsed = true;
            result = true;
            qDebug("End of style tag. KMLStyle object parsed");
        }
    }

    return result;
}

bool KMLStyleParser::characters( const QString& ch )
{
    if ( m_parsed ) {
        return false;
    }

    if ( m_currentParser != 0 ) {
        m_currentParser->characters( ch );
    }

    return true;
}
