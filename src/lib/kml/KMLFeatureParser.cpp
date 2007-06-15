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

KMLFeatureParser::KMLFeatureParser( KMLObject& object )
  : KMLObjectParser( object )
{
}

bool KMLFeatureParser::startElement( const QString& namespaceURI,
                            const QString& localName,
                            const QString &name,
                            const QXmlAttributes& atts )
{
    //TODO
    return false;
}

bool KMLFeatureParser::endElement( const QString& namespaceURI,
                            const QString& localName,
                            const QString &name )
{
    //TODO
    return false;
}

bool KMLFeatureParser::characters( const QString& str )
{
    //TODO
    return false;
}
