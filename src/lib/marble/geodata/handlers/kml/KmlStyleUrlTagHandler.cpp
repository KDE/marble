/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlStyleUrlTagHandler.h"

#include <QString>

#include "GeoDataFeature.h"
#include "GeoDataStyleMap.h"
#include "KmlElementDictionary.h"
#include "MarbleDebug.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(styleUrl)

GeoNode *KmlstyleUrlTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_styleUrl)));

    GeoStackItem parentItem = parser.parentElement();
    GeoStackItem grandParentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_Pair)) {
        QString content = parser.readElementText().trimmed();

        QString key = parentItem.nodeAs<GeoDataStyleMap>()->lastKey();
        (*parentItem.nodeAs<GeoDataStyleMap>())[key] = content;
        parentItem.nodeAs<GeoDataStyleMap>()->setLastKey(QString());

    } else if (parentItem.is<GeoDataFeature>()) {
        QString content = parser.readElementText().trimmed();

        parentItem.nodeAs<GeoDataFeature>()->setStyleUrl(content);
    }
    return nullptr;
}

}
}
