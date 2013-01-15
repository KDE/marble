//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlViewRefreshTimeTagHandler.h"

#include "GeoDataLink.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{

KML_DEFINE_TAG_HANDLER( viewRefreshTime )

GeoNode *KmlviewRefreshTimeTagHandler::parse(GeoParser & parser) const
{
    Q_ASSERT ( parser.isStartElement() && parser.isValidElement( kmlTag_viewRefreshTime ) );

    GeoStackItem parentItem = parser.parentElement();
    if ( parentItem.is<GeoDataLink>() ){
        qreal const viewRefreshTime = parser.readElementText().trimmed().toDouble();
        parentItem.nodeAs<GeoDataLink>()->setViewRefreshTime( viewRefreshTime );
    }

    return 0;
}

}
}
