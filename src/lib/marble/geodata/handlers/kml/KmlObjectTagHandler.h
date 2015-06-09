//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_KMLOBJECTTAGHANDLER_H
#define MARBLE_KMLOBJECTTAGHANDLER_H

#include <GeoParser.h>
#include <GeoDataObject.h>

namespace Marble
{

class KmlObjectTagHandler
{
public:

    /**
     * Parses the id and targetId attributes and assign their values to the given object
     */
    static void parseIdentifiers( const GeoParser &parser, GeoDataObject* object );

private:
    KmlObjectTagHandler(); // not implemented
};

}

#endif
