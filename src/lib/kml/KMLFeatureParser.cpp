//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "KMLFeatureParser.h"

#include "KMLFeature.h"

namespace
{
    const int KML_FEATURE_LEVEL = 2;

    const QString NAME_TAG = "name";
    const QString DESCRIPTION_TAG = "description";
    const QString ADDRESS_TAG = "address";
    const QString PHONE_NUMBER_TAG = "phonenumber";
}

KMLFeatureParser::KMLFeatureParser( KMLFeature& feature )
  : KMLObjectParser( feature ),
    m_phase( IDLE )
{
}

bool KMLFeatureParser::startElement( const QString& namespaceURI,
                            const QString& localName,
                            const QString &name,
                            const QXmlAttributes& atts )
{
    Q_UNUSED( namespaceURI );
    Q_UNUSED( localName );
    Q_UNUSED( atts );

    bool result = false;

    if ( m_level == KML_FEATURE_LEVEL ) {
        QString lowerName = name.toLower();

        /*
        * Iterate over fields which supported by
        * parser of this kml class
        */
        if ( lowerName == NAME_TAG ) {
            m_phase = WAIT_NAME;
            result = true;
        }
        else if ( lowerName == DESCRIPTION_TAG ) {
            m_phase = WAIT_DESCRIPTION;
            result = true;
        }
        else if ( lowerName == ADDRESS_TAG ) {
            m_phase = WAIT_ADDRESS;
            result = true;
        }
        else if ( lowerName == PHONE_NUMBER_TAG ) {
            m_phase = WAIT_PHONE_NUMBER;
            result = true;
        }
    }

    return result;
}

bool KMLFeatureParser::endElement( const QString &namespaceURI,
				   const QString &localName,
				   const QString &name )
{
    Q_UNUSED( namespaceURI );
    Q_UNUSED( localName );

    bool result = false;

    if ( m_level == KML_FEATURE_LEVEL ) {
        QString lowerName = name.toLower();

        switch ( m_phase ) {
        case WAIT_NAME:
            if ( lowerName == NAME_TAG ) {
                m_phase = IDLE;
                result = true;
            }
            break;
        case WAIT_DESCRIPTION:
            if ( lowerName == DESCRIPTION_TAG ) {
                m_phase = IDLE;
                result = true;
            }
        case WAIT_ADDRESS:
            if ( lowerName == ADDRESS_TAG ) {
                m_phase = IDLE;
                result = true;
            }
        case WAIT_PHONE_NUMBER:
            if ( lowerName == PHONE_NUMBER_TAG ) {
                m_phase = IDLE;
                result = true;
            }
        default:
            break;
        }
    }

    return result;
}

bool KMLFeatureParser::characters( const QString& str )
{
    bool result = false;

    if ( m_level == KML_FEATURE_LEVEL ) {
        KMLFeature& feature = (KMLFeature&) m_object;

        switch ( m_phase ) {
        case WAIT_NAME:
            feature.setName( str );
            result = true;
            break;
        case WAIT_DESCRIPTION:
            feature.setDescription( str );
            result = true;
            break;
        case WAIT_ADDRESS:
            feature.setAddress( str );
            result = true;
            break;
        case WAIT_PHONE_NUMBER:
            feature.setPhoneNumber( str );
            result = true;
            break;
        default:
            break;
        }
    }

    return result;
}
