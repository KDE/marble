// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlViewBoundScaleTagHandler.h"

#include "GeoDataLink.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{

KML_DEFINE_TAG_HANDLER(viewBoundScale)

GeoNode *KmlviewBoundScaleTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QString::fromLatin1(kmlTag_viewBoundScale)));
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.is<GeoDataLink>()) {
        qreal const viewBoundScale = parser.readElementText().trimmed().toDouble();
        parentItem.nodeAs<GeoDataLink>()->setViewBoundScale(viewBoundScale);
    }

    return nullptr;
}

}
}
