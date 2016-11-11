
//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
//
#include "KmlCameraTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"
#include "GeoDataCamera.h"
#include "GeoParser.h"
#include "GeoDataFeature.h"
#include "GeoDataFlyTo.h"

namespace Marble
{
namespace kml
{

KML_DEFINE_TAG_HANDLER( Camera )

GeoNode *KmlCameraTagHandler::parse( GeoParser & parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_Camera)));

    GeoDataCamera* camera = 0;
    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.is<GeoDataFeature>() ) {
        camera = new GeoDataCamera;
        KmlObjectTagHandler::parseIdentifiers( parser, camera );
        parentItem.nodeAs<GeoDataFeature>()->setAbstractView( camera );
    }

    if ( parentItem.is<GeoDataFlyTo>() ) {
        camera = new GeoDataCamera;
        KmlObjectTagHandler::parseIdentifiers( parser, camera );
        parentItem.nodeAs<GeoDataFlyTo>()->setView( camera );
    }

    return camera;
}

}

}
