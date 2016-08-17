//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlRefreshVisibilityTagHandler.h"
#include "GeoDataNetworkLink.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"
#include "MarbleDebug.h"
#include "MarbleGlobal.h"

namespace Marble
{
namespace kml
{

KML_DEFINE_TAG_HANDLER( refreshVisibility )

GeoNode *KmlrefreshVisibilityTagHandler::parse(GeoParser & parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_refreshVisibility)));

    GeoStackItem parentItem = parser.parentElement();
    if( parentItem.is<GeoDataNetworkLink>() ) {
        QString content = parser.readElementText().trimmed();
        GeoDataNetworkLink* networkLink = parentItem.nodeAs<GeoDataNetworkLink>();
        networkLink->setRefreshVisibility(content == QLatin1String("1"));
    }

    return 0;
}

}
}
