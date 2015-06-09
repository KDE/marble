//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Dennis Nienhüser <nienhueser@kde.org>
//

#include "KmlObjectTagHandler.h"

namespace Marble {

void KmlObjectTagHandler::parseIdentifiers( const GeoParser &parser, GeoDataObject *object )
{
    object->setId( parser.attribute( "id" ).toUtf8() );
    object->setTargetId( parser.attribute( "targetId" ).toUtf8() );
}

}
