//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_KMLOBJECTTAGHANDLER_H
#define MARBLE_KMLOBJECTTAGHANDLER_H

#include <GeoWriter.h>
#include <GeoDataObject.h>

namespace Marble
{

class KmlObjectTagWriter
{
public:

    /**
     * Parses the id and targetId attributes and assign their values to the given object
     */
    static void writeIdentifiers( GeoWriter &writer, const GeoDataObject* object );

private:
    KmlObjectTagWriter(); // not implemented
};

}

#endif
