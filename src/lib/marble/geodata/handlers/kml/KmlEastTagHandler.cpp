// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#include "KmlEastTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( east )

GeoNode* KmleastTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_east)));

    GeoStackItem parentItem = parser.parentElement();

    qreal const east = parser.readElementText().trimmed().toDouble();
    if( parentItem.represents( kmlTag_LatLonAltBox ) ) {
        parentItem.nodeAs<GeoDataLatLonAltBox>()->setEast( east, GeoDataCoordinates::Degree );
    } else if( parentItem.represents( kmlTag_LatLonBox ) ) {
        parentItem.nodeAs<GeoDataLatLonBox>()->setEast( east, GeoDataCoordinates::Degree );
    }

    return nullptr;
}

}
}
