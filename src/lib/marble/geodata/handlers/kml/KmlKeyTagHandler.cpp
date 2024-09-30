/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlKeyTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"

#include "GeoDataStyleMap.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(key)

GeoNode *KmlkeyTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_key)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_Pair)) {
        QString content = parser.readElementText().trimmed();

        parentItem.nodeAs<GeoDataStyleMap>()->setLastKey(content);
    }

    return nullptr;
}

}
}
