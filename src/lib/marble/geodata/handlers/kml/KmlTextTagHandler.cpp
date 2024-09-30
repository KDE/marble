// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlTextTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataBalloonStyle.h"
#include "GeoDataParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(text)

GeoNode *KmltextTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_text)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_BalloonStyle)) {
        QString text = parser.readElementText().trimmed();
        parentItem.nodeAs<GeoDataBalloonStyle>()->setText(text);
    }
    return nullptr;
}

}
}
