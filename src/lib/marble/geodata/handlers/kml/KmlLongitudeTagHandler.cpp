//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Gaurav Gupta <1989.gaurav@googlemail.com>
// Copyright 2013      Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "KmlLongitudeTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"

#include "GeoDataLookAt.h"
#include "GeoDataCamera.h"
#include "GeoParser.h"
#include "GeoDataCoordinates.h"
#include "MarbleGlobal.h"
#include "GeoDataLocation.h"

namespace Marble
{
namespace kml
{
    KML_DEFINE_TAG_HANDLER( longitude )

    GeoNode *KmllongitudeTagHandler::parse( GeoParser & parser ) const
    {
        Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_longitude)));

        GeoStackItem parentItem = parser.parentElement();
        if ( parentItem.is<GeoDataLookAt>() ) {
            qreal longitude = parser.readElementText().trimmed().toDouble();
            parentItem.nodeAs<GeoDataLookAt>()->setLongitude(longitude, GeoDataCoordinates::Degree);
        } else if ( parentItem.is<GeoDataCamera>() ) {
            qreal longitude = parser.readElementText().trimmed().toDouble();
            parentItem.nodeAs<GeoDataCamera>()->setLongitude(longitude, GeoDataCoordinates::Degree);
        } else if ( parentItem.is<GeoDataLocation>() ) {
            qreal longitude = parser.readElementText().trimmed().toDouble();
            parentItem.nodeAs<GeoDataLocation>()->setLongitude(longitude, GeoDataCoordinates::Degree);
        }

      return 0;
    }

}
}
