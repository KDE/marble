//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "KMLContainerParser.h"
#include "KMLPlaceMarkParser.h"

namespace
{
    const QString PLACEMARK_START_TAG = "placemark";
}

KMLContainerParser::KMLContainerParser( KMLObject& object )
  : KMLFeatureParser( object ),
    m_currentParser(0)
{
}

KMLContainerParser::~KMLContainerParser()
{
}

bool KMLContainerParser::startElement( const QString& namespaceURI,
                            const QString& localName,
                            const QString& name,
                            const QXmlAttributes& atts )
{
    bool result = false;

    if ( m_currentParser != 0 ) {
        result = m_currentParser->startElement( namespaceURI, localName, name, atts );

        if ( ! result ) {
            /*
             * Current parser does not support this tag
             * reached end of current parser
             * should delete it
             */
            delete m_currentParser;
            m_currentParser = 0;
        }
    }
    else {
        result = KMLFeatureParser::startElement( namespaceURI, localName, name, atts );
    }

    if ( ! result ) {
        /*
         * Try to find tags which supported by current parser
         * i.e. list of KMLPlaceMark objects
         * If found create appropriate parser object and
         * gave control to it
         */
        QString nameLower = name.toLower();

        if ( nameLower == PLACEMARK_START_TAG ) {
            m_currentParser = new KMLPlaceMarkParser( m_object );
            result = m_currentParser->startElement( namespaceURI, localName, name, atts );
        }
    }

    return result;
}

bool KMLContainerParser::endElement( const QString& namespaceURI,
                            const QString& localName,
                            const QString& qName )
{
    bool result = false;

    if ( m_currentParser != 0 ) {
        result = m_currentParser->endElement( namespaceURI, localName, qName );
    }
    else {
        result = KMLFeatureParser::endElement( namespaceURI, localName, qName );
    }

    return result;
}

bool KMLContainerParser::characters( const QString& ch )
{
    bool result = false;

    if ( m_currentParser != 0 ) {
        m_currentParser->characters( ch );
    }
    else {
        result = KMLFeatureParser::characters( ch );
    }

    return result;
}
