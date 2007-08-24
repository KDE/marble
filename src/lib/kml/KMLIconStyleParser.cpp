//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "KMLIconStyleParser.h"
#include "KMLIconStyle.h"

namespace
{
    const QString ICON_STYLE_TAG        = "iconstyle";
    const QString ICON_TAG              = "icon";
    const QString HREF_TAG              = "href";
}

KMLIconStyleParser::KMLIconStyleParser( KMLIconStyle& iconStyle )
  : KMLColorStyleParser( iconStyle ),
    m_phase( IDLE )
{
}

bool KMLIconStyleParser::startElement( const QString& namespaceURI,
                            const QString& localName,
                            const QString& name,
                            const QXmlAttributes& atts )
{
    if ( m_parsed ) {
        return false;
    }

    bool result = KMLColorStyleParser::startElement( namespaceURI, localName, name, atts );

    if ( ! result ) {
        QString lowerName = name.toLower();

        if ( lowerName == ICON_STYLE_TAG ) {
            result = true;
            qDebug("KMLColorStyleParser: Start to parse IconStyle");
        }
        else if ( lowerName == ICON_TAG ) {
            m_phase = WAIT_ICON;
            result = true;
        }
        else if ( lowerName == HREF_TAG ) {
            m_phase = WAIT_HREF;
            result = true;
        }
    }

    return result;
}

bool KMLIconStyleParser::endElement( const QString& namespaceURI,
                            const QString& localName,
                            const QString& qName )
{
    if ( m_parsed ) {
        return false;
    }

    bool result = KMLColorStyleParser::endElement( namespaceURI, localName, qName );

    if ( ! result ) {
        QString lowerName = qName.toLower();

        switch ( m_phase ) {
            case IDLE:
                if ( lowerName == ICON_STYLE_TAG ) {
                    m_parsed = true;
                    result = true;
                    qDebug("KMLColorStyleParser: IconStyle parsed");
                }
                break;
            case WAIT_ICON:
                if ( lowerName == ICON_TAG ) {
                    m_phase = IDLE;
                    result = true;
                }
                break;
            case WAIT_HREF:
                if ( lowerName == HREF_TAG ) {
                    m_phase = WAIT_ICON;
                    result = true;
                }
                break;
            default:
                break;
        }
    }

    return true;
}

bool KMLIconStyleParser::characters( const QString& ch )
{
    if ( m_parsed ) {
        return false;
    }

    bool result = KMLColorStyleParser::characters( ch );

    if ( ! result ) {
        switch ( m_phase ) {
            case WAIT_HREF:
                /*
                 * TODO
                 */
                result = true;
                break;
            default:
                break;
        }
    }

    return result;
}
