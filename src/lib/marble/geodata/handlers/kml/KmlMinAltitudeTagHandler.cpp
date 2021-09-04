//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#include "KmlMinAltitudeTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( minAltitude )

GeoNode* KmlminAltitudeTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_minAltitude)));

    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.represents( kmlTag_LatLonAltBox ) ) {
        float minAltitude = parser.readElementText().trimmed().toFloat();

        parentItem.nodeAs<GeoDataLatLonAltBox>()->setMinAltitude( minAltitude );
    }

    return nullptr;
}

}
}
