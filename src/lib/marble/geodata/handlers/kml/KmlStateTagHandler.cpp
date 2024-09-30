// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlStateTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataItemIcon.h"
#include "GeoDataParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(state)

GeoNode *KmlstateTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_state)));

    GeoStackItem parentItem = parser.parentElement();

    GeoDataItemIcon::ItemIconStates itemIconState;

    if (parentItem.represents(kmlTag_ItemIcon)) {
        QString value = parser.readElementText().trimmed();
        QStringList iconStateTextList = value.split(QLatin1Char(' '));

        for (const QString &value : iconStateTextList) {
            if (value == QLatin1StringView("open")) {
                itemIconState |= GeoDataItemIcon::Open;
            } else if (value == QLatin1StringView("closed")) {
                itemIconState |= GeoDataItemIcon::Closed;
            } else if (value == QLatin1StringView("error")) {
                itemIconState |= GeoDataItemIcon::Error;
            } else if (value == QLatin1StringView("fetching0")) {
                itemIconState |= GeoDataItemIcon::Fetching0;
            } else if (value == QLatin1StringView("fetching1")) {
                itemIconState |= GeoDataItemIcon::Fetching1;
            } else if (value == QLatin1StringView("fetching2")) {
                itemIconState |= GeoDataItemIcon::Fetching2;
            } else {
                mDebug() << "Cannot parse state value" << value;
            }
        }

        parentItem.nodeAs<GeoDataItemIcon>()->setState(itemIconState);
    }
    return nullptr;
}

}
}
