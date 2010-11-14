//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Harshit Jain <hjain.itbhu@gmail.com>
//

#include "KmlTimeSpanTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataTimeSpan.h"
#include "GeoDataFeature.h"
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( TimeSpan )

GeoNode* KmlTimeSpanTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_TimeSpan ) );

    GeoStackItem parentItem = parser.parentElement();
    if ( parentItem.nodeAs<GeoDataFeature>() ) {
        GeoDataTimeSpan timeSpan;
        timeSpan.setBegin( QDateTime::fromString( "1752-01-02T00:00:00", Qt::ISODate ) );
	timeSpan.setEnd( QDateTime::fromString( "7999-12-31T23:59:59", Qt::ISODate ) );
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << kmlTag_TimeSpan << ">"
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
	QString id = parser.attribute( "id" ).trimmed();
#ifdef DEBUG_TAGS
        mDebug() << "atrribute 'id':" << id;
#endif // DEBUG_TAGS
        parentItem.nodeAs<GeoDataFeature>()->setTimeSpan( timeSpan );        

        return &parentItem.nodeAs<GeoDataFeature>()->timeSpan();             
    }
    
    return 0;
}

}
}
