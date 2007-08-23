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

namespace
{
    const QString STYLE_TAG     = "style";
}

KMLStyleParser::KMLStyleParser( KMLStyle& style ) :
    KMLObjectParser( style )
{
}

KMLStyleParser::~KMLStyleParser()
{
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
    QString lowerName = name.toLower();

    if ( lowerName == STYLE_TAG ) {
        result = true;
        qDebug("Start of style tag. Begining to parse KMLStyle object");
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

    bool result = KMLObjectParser::endElement( namespaceURI, localName, qName );
    QString lowerName = qName.toLower();

    if ( lowerName == STYLE_TAG ) {
        m_parsed = true;
        result = true;
        qDebug("End of style tag. KMLStyle object parsed");
    }

    return result;
}

bool KMLStyleParser::characters( const QString& ch )
{
    if ( m_parsed ) {
        return false;
    }

    return true;
}
