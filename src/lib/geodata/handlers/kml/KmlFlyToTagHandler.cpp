//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013       Mayank Madan <maddiemadan@gmail.com>

#include "GeoDataFlyTo.h"
#include "GeoParser.h"
#include "MarbleDebug.h"
#include "KmlFlyToTagHandler.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{

KML_DEFINE_TAG_HANDLER_GX22( FlyTo )

GeoNode* KmlFlyToTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_FlyTo ) );

    /** @todo Integrate with gx:TourPrimitive and gx:Tour */

    return 0;
}

}
}

