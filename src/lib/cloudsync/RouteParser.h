//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013   Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef ROUTEPARSER_H
#define ROUTEPARSER_H

#include "GeoParser.h"
#include "GeoDataDocument.h"

namespace Marble {

/**
 * Class that overrides necessary methods of GeoParser.
 * @see GeoParser
 */
class RouteParser : public GeoParser {
public:
    RouteParser();
    virtual GeoDataDocument* createDocument() const;
    virtual bool isValidRootElement();
};

}

#endif // ROUTEPARSER_H
