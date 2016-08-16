//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
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

    return 0;
}

}
}
