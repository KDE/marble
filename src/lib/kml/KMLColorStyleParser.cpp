//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "KMLColorStyleParser.h"
#include "KMLColorStyle.h"

KMLColorStyleParser::KMLColorStyleParser( KMLColorStyle& style )
  : KMLObjectParser( style )
{
}

bool KMLColorStyleParser::startElement( const QString& namespaceURI,
                               const QString& localName,
                               const QString& name,
                               const QXmlAttributes& atts )
{
    if ( m_parsed ) {
        return false;
    }

    //TODO
    return false;
}

bool KMLColorStyleParser::endElement( const QString& namespaceURI,
                             const QString& localName,
                             const QString& qName )
{
    if ( m_parsed ) {
        return false;
    }

    //TODO
    return false;
}

bool KMLColorStyleParser::characters( const QString& ch )
{
    if ( m_parsed ) {
        return false;
    }

    //TODO
    return false;
}
