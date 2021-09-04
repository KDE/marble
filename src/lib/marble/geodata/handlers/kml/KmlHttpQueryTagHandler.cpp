// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlHttpQueryTagHandler.h"

#include "GeoDataLink.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{

KML_DEFINE_TAG_HANDLER( httpQuery )

GeoNode* KmlhttpQueryTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_httpQuery)));
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(kmlTag_Link))
    {
        QString content = parser.readElementText().trimmed();
        parentItem.nodeAs<GeoDataLink>()->setHttpQuery(content);
    }

    return nullptr;
}

}
}
