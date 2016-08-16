//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "KmlChangeTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoDataUpdate.h"
#include "GeoDataChange.h"
#include "GeoDataParser.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( Change )

GeoNode* KmlChangeTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_Change)));

    GeoDataChange *change = new GeoDataChange;
    KmlObjectTagHandler::parseIdentifiers( parser, change );
    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.represents( kmlTag_Update ) ) {
        parentItem.nodeAs<GeoDataUpdate>()->setChange( change );
        return change;
    } else {
        delete change;
        return 0;
    }
}

}
}
