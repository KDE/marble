//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "KMLPlaceMarkParser.h"

#include "KMLPlaceMark.h"
#include "KMLPointParser.h"

namespace
{
    const QString PLACEMARKPARSER_TAG   = "placemark";
    const QString POINTPARSER_TAG       = "point";

    /*
     * TODO: This is Marble's placemarks specific field. Should
     * use "schema" in next step
     */
    const QString POP_TAG               = "pop";
    const QString ROLE_TAG              = "role";
}

KMLPlaceMarkParser::KMLPlaceMarkParser( KMLPlaceMark& placemark )
  : KMLFeatureParser( placemark ),
    m_currentParser( 0 ),
    m_phase( IDLE )
{
}

KMLPlaceMarkParser::~KMLPlaceMarkParser()
{
}

bool KMLPlaceMarkParser::startElement( const QString& namespaceURI,
                            const QString& localName,
                            const QString& name,
                            const QXmlAttributes& atts )
{
    if ( m_parsed ) {
        return false;
    }

    m_level++;

    bool result = false;

    if ( m_currentParser != 0 ) {
        result = m_currentParser->startElement( namespaceURI, localName, name, atts );
    }
    else {
        result = KMLFeatureParser::startElement( namespaceURI, localName, name, atts );
    }

    if ( ! result ) {
        QString lowerName = name.toLower();

        /*
         * Iterate over tags which current parser supports
         * TODO: check if this tag appears first time or not
         */
        if ( lowerName == PLACEMARKPARSER_TAG ) {
            result = true;
        }
        else if ( lowerName == POINTPARSER_TAG ) {
            if ( m_currentParser != 0 ) {
                delete m_currentParser;
                m_currentParser = 0;
            }

            m_currentParser = new KMLPointParser( (KMLPlaceMark&) m_object );
            result = m_currentParser->startElement( namespaceURI, localName, name, atts );
        }
        else if ( lowerName == POP_TAG ) {
            m_phase = WAIT_POP;
            result = true;
        }
        else if ( lowerName == ROLE_TAG ) {
            m_phase = WAIT_ROLE;
            result = true;
        }
    }

    return result;
}

bool KMLPlaceMarkParser::endElement( const QString& namespaceURI,
                            const QString& localName,
                            const QString& qName )
{
    if ( m_parsed ) {
        return false;
    }

    bool result = false;

    if ( m_currentParser != 0 ) {
        result = m_currentParser->endElement( namespaceURI, localName, qName );
    }
    else {
        result = KMLFeatureParser::endElement( namespaceURI, localName, qName );
    }

    if ( ! result ) {
        /*
         * Check this is our end tag
         * TODO: check if this tag appears first time or not
         */
        QString lowerName = qName.toLower();

        switch ( m_phase ) {
            case IDLE:
                if ( lowerName == PLACEMARKPARSER_TAG ) {
                    m_parsed = true;
                    result = true;
                }
                break;
            case WAIT_POP:
                if ( lowerName == POP_TAG ) {
                    m_phase = IDLE;
                    result = true;
                }
                break;
            case WAIT_ROLE:
                if ( lowerName == ROLE_TAG ) {
                    m_phase = IDLE;
                    result = true;
                }
            default:
                break;
        }
    }

    m_level--;

    return result;
}

bool KMLPlaceMarkParser::characters( const QString& str )
{
    if ( m_parsed ) {
        return false;
    }

    bool result = false;

    if ( m_currentParser != 0 ) {
        result = m_currentParser->characters( str );
    }
    else
    {
        result = KMLFeatureParser::characters( str );
    }

    if ( ! result ) {

        KMLPlaceMark& placemark = (KMLPlaceMark&) m_object;

        switch ( m_phase ) {
            case WAIT_POP:
                placemark.setPopulation( str.toInt() );
                result = true;
                break;
            case WAIT_ROLE:
                placemark.setRole( str.at(0) );
                result = true;
                break;
            default:
                break;
        }
    }

    return result;
}
