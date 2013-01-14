//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
//
#include "KmlDurationTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"

#include "MarbleGlobal.h"
#include "GeoDataFlyTo.h"
#include "GeoParser.h"


namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( duration )
    GeoNode *KmldurationTagHandler::parse(GeoParser & parser) const
    {
        Q_ASSERT ( parser.isStartElement()
                   && parser.isValidElement( kmlTag_duration ) );

        GeoStackItem parentItem = parser.parentElement();

        qreal const duration = parser.readElementText().trimmed().toDouble();
        if ( parentItem.is<GeoDataFlyTo>() ){
            parentItem.nodeAs<GeoDataFlyTo>()->setDuration( duration );
#ifdef DEBUG_TAGS
            mDebug () << "Parsed <" << kmlTag_duration << "> containing: " <<
            duration << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
        }
        return 0;
}
}
}
