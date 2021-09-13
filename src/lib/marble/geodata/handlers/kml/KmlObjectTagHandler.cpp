// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Dennis Nienhüser <nienhueser@kde.org>
//

#include "KmlObjectTagHandler.h"

namespace Marble {

void KmlObjectTagHandler::parseIdentifiers( const GeoParser &parser, GeoDataObject *object )
{
    object->setId( parser.attribute( "id" ).toUtf8() );
    object->setTargetId( parser.attribute( "targetId" ).toUtf8() );
}

}
