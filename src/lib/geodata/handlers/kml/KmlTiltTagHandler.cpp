//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlTiltTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataCamera.h"
#include "GeoDataIconStyle.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( tilt )

GeoNode* KmltiltTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_tilt ) );

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.is<GeoDataCamera>() ){
        qreal tilt = parser.readElementText().trimmed().toDouble();
        parentItem.nodeAs<GeoDataCamera>()->setTilt(tilt);
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << kmlTag_tilt << "> containing: " << parser.readElementText().trimmed()
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
    }
    return 0;
}

}
}

