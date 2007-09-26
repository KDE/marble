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
    const QString COUNTRYNAMECODE_TAG   = "countrynamecode";
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
        else if ( lowerName == COUNTRYNAMECODE_TAG ) {
            m_phase = WAIT_COUNTRYNAMECODE;
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
                    setPlaceMarkSymbol();
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
            case WAIT_COUNTRYNAMECODE:
                if ( lowerName == COUNTRYNAMECODE_TAG ) {
                    m_phase = IDLE;
                    result = true;
                }
                break;
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

        //removed decleration from the switch statment
        int population;

        switch ( m_phase ) {
            case WAIT_POP:
                population = str.toInt();
                placemark.setPopularity( population );
                placemark.setPopularityIndex( popIdx( population ) );
                result = true;
                break;
            case WAIT_ROLE:
                placemark.setRole( str.at(0) );
                result = true;
                break;
            case WAIT_COUNTRYNAMECODE:
                placemark.setCountryCode( str );
                break;
            default:
                break;
        }
    }

    return result;
}

void KMLPlaceMarkParser::setPlaceMarkSymbol()
{
    KMLPlaceMark& placemark = (KMLPlaceMark&) m_object;

    if ( placemark.role() == 'P' )      placemark.setSymbolIndex(16);
    else if ( placemark.role() == 'M' ) placemark.setSymbolIndex(17);
    else if ( placemark.role() == 'H' ) placemark.setSymbolIndex(18);
    else if ( placemark.role() == 'V' ) placemark.setSymbolIndex(19);
    else if ( placemark.role() == 'F' ) placemark.setSymbolIndex(20);
    else if ( placemark.role() == 'N' ) placemark.setSymbolIndex( ( placemark.popularityIndex() -1 ) / 4 * 4 );
    else if ( placemark.role() == 'R' ) placemark.setSymbolIndex( ( placemark.popularityIndex() -1 ) / 4 * 4 + 2);
    else if ( placemark.role() == 'C' || placemark.role() == 'B' )
        placemark.setSymbolIndex( ( placemark.popularityIndex() -1 ) / 4 * 4 + 3 );
}

int KMLPlaceMarkParser::popIdx( int population )
{
    int popidx = 15;

    if ( population < 2500 )        popidx=1;
    else if ( population < 5000)    popidx=2;
    else if ( population < 7500)    popidx=3;
    else if ( population < 10000)   popidx=4;
    else if ( population < 25000)   popidx=5;
    else if ( population < 50000)   popidx=6;
    else if ( population < 75000)   popidx=7;
    else if ( population < 100000)  popidx=8;
    else if ( population < 250000)  popidx=9;
    else if ( population < 500000)  popidx=10;
    else if ( population < 750000)  popidx=11;
    else if ( population < 1000000) popidx=12;
    else if ( population < 2500000) popidx=13;
    else if ( population < 5000000) popidx=14;

    return popidx;
}
