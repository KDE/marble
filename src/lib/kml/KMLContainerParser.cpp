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

#include "KMLContainer.h"
#include "KMLPlaceMark.h"
#include "KMLPlaceMarkParser.h"

namespace
{
    const QString PLACEMARK_START_TAG = "placemark";
}

KMLContainerParser::KMLContainerParser( KMLContainer& container )
  : KMLFeatureParser( container ),
    m_currentParser(0),
    m_currentPlaceMark (0)
{
}

KMLContainerParser::~KMLContainerParser()
{
    delete m_currentParser;
    delete m_currentPlaceMark;
}

bool KMLContainerParser::startElement( const QString& namespaceURI,
                            const QString& localName,
                            const QString& name,
                            const QXmlAttributes& atts )
{
    bool result = false;

    if ( m_currentParser != 0 ) {
        result = m_currentParser->startElement( namespaceURI, localName, name, atts );
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
            if ( m_currentParser != 0 ) {
                delete m_currentParser;
                delete m_currentPlaceMark;
            }

            m_currentPlaceMark = new KMLPlaceMark();
            m_currentParser = new KMLPlaceMarkParser( *m_currentPlaceMark );

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

        if ( result ) {
            if ( m_currentParser->isParsed() ) {
                delete m_currentParser;
                m_currentParser = 0;

                KMLContainer& container = (KMLContainer&) m_object;
                container.addPlaceMark( m_currentPlaceMark );
                m_currentPlaceMark = 0;
            }
        }
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
