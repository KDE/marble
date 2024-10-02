/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#if KML_LAZY_IMP
#include "KmlCountryNameCodeTagHandler.h"

#include "KmlElementDictionary.h"

#include "GeoDataPlacemark.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(CountryNameCode)

GeoNode *KmlCountryNameCodeTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_CountryNameCode)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.nodeAs<GeoDataPlacemark>()) {
        QString countrycode = parser.readElementText().trimmed();

        parentItem.nodeAs<GeoDataPlacemark>()->setCountryCode(countrycode);
    }

    return nullptr;
}

}
}

#endif // KML_LAZY_IMP
