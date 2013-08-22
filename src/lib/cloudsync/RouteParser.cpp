//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013   Utku Aydın <utkuaydin34@gmail.com>
//

#include "RouteParser.h"

#include "KmlElementDictionary.h"

namespace Marble {

RouteParser::RouteParser() : GeoParser( 0 )
{
    // Nothing to do.
}

GeoDataDocument* RouteParser::createDocument() const
{
    return new GeoDataDocument;
}

bool RouteParser::isValidRootElement()
{
    return isValidElement(kml::kmlTag_kml);
}

}

#include "RouteParser.moc"
