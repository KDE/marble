//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
// Copyright 2013      Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "KmlRollTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoDataCamera.h"
#include "GeoParser.h"
#include "GeoDataOrientation.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( roll )

GeoNode* KmlrollTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_roll)));

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.is<GeoDataCamera>() ) {
        qreal roll = parser.readElementText().trimmed().toDouble();
        parentItem.nodeAs<GeoDataCamera>()->setRoll(roll);
    } else if (parentItem.is<GeoDataOrientation>() ) {
        qreal roll = parser.readElementText().trimmed().toDouble();
        parentItem.nodeAs<GeoDataOrientation>()->setRoll(roll);
    }
    return 0;
}

}
}
