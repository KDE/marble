//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Gaurav Gupta <1989.gaurav@googlemail.com>
//
#include "KmlLongitudeTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"

#include "GeoDataLookAt.h"
#include "GeoParser.h"
#include "GeoDataCoordinates.h"
#include "MarbleGlobal.h"

namespace Marble
{
namespace kml
{
    KML_DEFINE_TAG_HANDLER( longitude )

    GeoNode *KmllongitudeTagHandler::parse( GeoParser & parser ) const
    {
        Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_longitude ) );

        GeoStackItem parentItem = parser.parentElement();
        if ( parentItem.is<GeoDataLookAt>() ){
            QString longitudeTemp = parser.readElementText().trimmed();
            qreal longitude = longitudeTemp.toDouble();
            parentItem.nodeAs<GeoDataLookAt>()->setLongitude(longitude,
                                                             GeoDataCoordinates::Degree);
#ifdef DEBUG_TAGS
            mDebug () << "Parsed <" << kmlTag_longitude << "> containing: " <<
            longitude << " parent item name: " << parentItem.qualifiedName ().
            first;
#endif // DEBUG_TAGS
        }

      return 0;
    }

}
}
