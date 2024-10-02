/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#if KML_LAZY_IMP
#include "KmlAreaTagHandler.h"

#include "KmlElementDictionary.h"

#include "GeoDataPlacemark.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(area)

GeoNode *KmlareaTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_area)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataPlacemark>()) {
        QString area = parser.readElementText().trimmed();
        qreal ar = area.toDouble();
        if (ar < 0.0)
            ar = 0.0;
        parentItem.nodeAs<GeoDataPlacemark>()->setArea(ar);
    }

    return nullptr;
}

}
}

#endif // KML_LAZY_IMP
