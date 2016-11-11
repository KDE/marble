//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Gaurav Gupta <1989.gaurav@googlemail.com>
//
#include "KmlLookAtTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataLookAt.h"
#include "GeoParser.h"
#include "GeoDataFeature.h"
#include "GeoDataFlyTo.h"

namespace Marble
{
namespace kml
{

KML_DEFINE_TAG_HANDLER( LookAt )

GeoNode *KmlLookAtTagHandler::parse( GeoParser & parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_LookAt)));

    GeoDataLookAt *lookAt = new GeoDataLookAt();
    GeoStackItem parentItem = parser.parentElement();
    if ( parentItem.is<GeoDataFeature>() ) {
        GeoDataFeature *feature = parentItem.nodeAs<GeoDataFeature>();
        feature->setAbstractView( lookAt );
        return lookAt;
    } if ( parentItem.is<GeoDataFlyTo>() ) {
        GeoDataFlyTo *feature = parentItem.nodeAs<GeoDataFlyTo>();
        feature->setView( lookAt );
        return lookAt;
    } else {
        delete lookAt;
        return 0;
    }
}

}
}
