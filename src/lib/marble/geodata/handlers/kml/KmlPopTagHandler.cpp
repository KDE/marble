/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#if KML_LAZY_IMP
#include "KmlPopTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"

#include "GeoDataPlacemark.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(pop)

GeoNode *KmlpopTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_pop)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataPlacemark>()) {
        QString population = parser.readElementText().trimmed();
        qint64 pop = population.toLongLong();
        if (pop < 0)
            pop = 0;
        parentItem.nodeAs<GeoDataPlacemark>()->setPopulation(pop);
    }

    return nullptr;
}

}
}

#endif // KML_LAZY_IMP
