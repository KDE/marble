// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "KmlplayModeTagHandler.h"

#include "GeoDataTourControl.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER_GX22(playMode)

GeoNode *KmlplayModeTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_playMode)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataTourControl>()) {
        auto control = parentItem.nodeAs<GeoDataTourControl>();

        if (parser.readElementText().trimmed().toLower() == QLatin1StringView("play")) {
            control->setPlayMode(GeoDataTourControl::Play);
        } else {
            control->setPlayMode(GeoDataTourControl::Pause);
        }
    }

    return nullptr;
}

} // namespace kml
} // namespace Marble
