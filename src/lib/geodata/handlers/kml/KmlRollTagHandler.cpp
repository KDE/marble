//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlRollTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataCamera.h"
#include "GeoDataIconStyle.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( roll )

GeoNode* KmlrollTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_roll ) );

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.is<GeoDataCamera>() ) {
        qreal roll = parser.readElementText().trimmed().toDouble();
        parentItem.nodeAs<GeoDataCamera>()->setRoll(roll);
    }
    return 0;
}

}
}
