//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Illya Kovalevskyy   <illya.kovalevskyy@gmail.com>
//

#include "KmldrawOrderTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoDataOverlay.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( drawOrder )

GeoNode* KmldrawOrderTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_drawOrder ) );

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.is<GeoDataOverlay>()) {
        bool ok = false;
        int value = parser.readElementText().trimmed().toInt(&ok, 10);
        if (ok) {
            parentItem.nodeAs<GeoDataOverlay>()->setDrawOrder(value);
        }
    }

    return 0;
}

} // namespace kml
} // namespace Marble
