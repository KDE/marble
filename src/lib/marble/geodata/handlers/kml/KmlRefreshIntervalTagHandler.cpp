//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlRefreshIntervalTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoDataLink.h"

namespace Marble
{
namespace kml
{

KML_DEFINE_TAG_HANDLER( refreshInterval )
    GeoNode *KmlrefreshIntervalTagHandler::parse(GeoParser & parser) const
    {
        Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_refreshInterval)));

        GeoStackItem parentItem = parser.parentElement();
        if ( parentItem.is<GeoDataLink>() ) {
            qreal const refreshInterval = parser.readElementText().trimmed().toDouble();
            parentItem.nodeAs<GeoDataLink>()->setRefreshInterval( refreshInterval );
        }

      return 0;
    }
}
}
