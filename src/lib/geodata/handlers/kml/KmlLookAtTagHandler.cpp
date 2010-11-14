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
#include "GeoDataParser.h"
#include "GeoDataContainer.h"
#include "GeoDataPlacemark.h"


namespace Marble
{
namespace kml
{
    KML_DEFINE_TAG_HANDLER( LookAt )
    GeoNode *KmlLookAtTagHandler::parse( GeoParser & parser ) const
    {
        Q_ASSERT (parser.isStartElement()
                  && parser.isValidElement( kmlTag_LookAt ) );

        GeoDataLookAt *lookAt = new GeoDataLookAt();
        GeoStackItem parentItem = parser.parentElement();
#ifdef DEBUG_TAGS
        mDebug () << "Parsed <" << kmlTag_LookAt << ">"
                      << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
      if ( parentItem.represents( kmlTag_Placemark ) ) {
          GeoDataPlacemark *placemark = parentItem.nodeAs<GeoDataPlacemark>();
          placemark->setLookAt( lookAt );

          return lookAt;
      }
      else {
          delete lookAt;
          return 0;
      }
    }
}
}
