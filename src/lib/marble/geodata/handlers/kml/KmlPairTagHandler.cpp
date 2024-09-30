/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlPairTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"

#include "GeoDataStyleMap.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(Pair)

GeoNode *KmlPairTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_Pair)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_StyleMap)) {
        return parentItem.nodeAs<GeoDataStyleMap>();
    }
    return nullptr;
}

}
}
