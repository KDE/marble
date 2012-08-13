//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#include "OsmBoundTagHandler.h"

#include "GeoParser.h"
#include "OsmNodeFactory.h"
#include "GeoDataParser.h"
#include "MarbleDebug.h"
#include "OsmElementDictionary.h"

#include <QtCore/QStringList>

namespace Marble
{

namespace osm
{

static GeoTagHandlerRegistrar osmBoundTagHandler( GeoParser::QualifiedName( osmTag_bound, "" ),
        new OsmBoundTagHandler() );

GeoNode* OsmBoundTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() );

    QStringList l = parser.attribute( "box" ).split( ',' );
    qreal minlat = l[0].toFloat();
    qreal minlon = l[1].toFloat();
    qreal maxlat = l[2].toFloat();
    qreal maxlon = l[3].toFloat();

    mDebug() << "[OSM] Bounds: " << minlat << " " << minlon << " " << maxlat << " " << maxlon;
    return 0;
}

}

}
