//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014       Sanjiban Bairagya <sanjiban22393@gmail.com>

#include "KmlBalloonVisibilityTagHandler.h"
#include "GeoDataPlacemark.h"
#include "GeoParser.h"
#include "MarbleDebug.h"
#include "KmlElementDictionary.h"

#include <QDebug>

namespace Marble
{
namespace kml
{
namespace gx
{
KML_DEFINE_TAG_HANDLER_GX22( balloonVisibility )

GeoNode* KmlballoonVisibilityTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_balloonVisibility)));

    QString content = parser.readElementText().trimmed();

    const bool visible = (content == QLatin1String("1"));

    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.is<GeoDataPlacemark>() ){
        parentItem.nodeAs<GeoDataPlacemark>()->setBalloonVisible( visible );
    }

    return 0;
}

}
}
}
