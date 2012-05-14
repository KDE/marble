//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Gaurav Gupta <1989.gaurav@googlemail.com>
//
#include "KmlAltitudeTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"

#include "MarbleGlobal.h"
#include "GeoDataLookAt.h"
#include "GeoParser.h"
#include "GeoDataCoordinates.h"


namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( altitude )
    GeoNode *KmlaltitudeTagHandler::parse(GeoParser & parser) const
    {
        Q_ASSERT ( parser.isStartElement()
                   && parser.isValidElement( kmlTag_altitude ) );

        GeoStackItem parentItem = parser.parentElement();

        if ( parentItem.is<GeoDataLookAt>() ){

            QString altitudeTemp = parser.readElementText().trimmed ();
            qreal altitude = altitudeTemp.toDouble();
            parentItem.nodeAs<GeoDataLookAt>()->setAltitude( altitude );
#ifdef DEBUG_TAGS
            mDebug () << "Parsed <" << kmlTag_altitude << "> containing: " <<
            altitude << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
        }

      return 0;
    }
}
}


