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
#include "KMLContainer.h"

namespace
{
    const QString PLACEMARK_TAG = "placemark";
}

KMLPlaceMarkParser::KMLPlaceMarkParser( KMLObject& object )
  : KMLFeatureParser( object ),
    m_placemark( new KMLPlaceMark() ),
    m_parsed( false )
{
    qDebug("KMLPlaceMarkParser::KMLPlaceMarkParser()");
}

KMLPlaceMarkParser::~KMLPlaceMarkParser()
{
    if ( ! m_parsed ) {
        delete m_placemark;
    }
}

bool KMLPlaceMarkParser::startElement( const QString& namespaceURI,
                            const QString& localName,
                            const QString& name,
                            const QXmlAttributes& atts )
{
    bool result = KMLFeatureParser::startElement( namespaceURI, localName, name, atts );

    if ( ! result ) {
        QString lowerName = name.toLower();

        /*
         * Iterate over tags which current parser supports
         * TODO: check if this tag appers first time or not
         */
        if ( lowerName == PLACEMARK_TAG ) {
            result = true;
        }
    }

    return result;
}

bool KMLPlaceMarkParser::endElement( const QString& namespaceURI,
                            const QString& localName,
                            const QString& qName )
{
    bool result = KMLFeatureParser::endElement( namespaceURI, localName, qName );

    if ( ! result ) {
        /*
         * Check this is our end tag
         * TODO: check if this tag appers first time or not
         */
        QString lowerName = qName.toLower();

        if ( lowerName == PLACEMARK_TAG ) {
            KMLContainer& container = (KMLContainer&) m_object;
            container.addPlaceMark( m_placemark );
            m_parsed = true;

            result = true;
        }
    }

    return result;
}

bool KMLPlaceMarkParser::characters( const QString& ch )
{
    return KMLFeatureParser::characters( ch );
}
