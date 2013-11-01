//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlGxTimeSpanTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataTimeSpan.h"
#include "GeoDataFeature.h"
#include "GeoDataAbstractView.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
namespace gx
{

KML_DEFINE_TAG_HANDLER_GX22( TimeSpan )

GeoNode* KmlTimeSpanTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_TimeSpan ) );

    GeoStackItem parentItem = parser.parentElement();
    if ( parentItem.is<GeoDataFeature>() ) {
        GeoDataTimeSpan timeSpan;
        timeSpan.setBegin( QDateTime::fromString( "1752-01-02T00:00:00", Qt::ISODate ) );
        timeSpan.setEnd( QDateTime::fromString( "7999-12-31T23:59:59", Qt::ISODate ) );
        parentItem.nodeAs<GeoDataFeature>()->setTimeSpan( timeSpan );
        return &parentItem.nodeAs<GeoDataFeature>()->timeSpan();
    } else if ( parentItem.is<GeoDataAbstractView>() ) {
        GeoDataTimeSpan timeSpan;
        timeSpan.setBegin( QDateTime::fromString( "1752-01-02T00:00:00", Qt::ISODate ) );
        timeSpan.setEnd( QDateTime::fromString( "7999-12-31T23:59:59", Qt::ISODate ) );
        parentItem.nodeAs<GeoDataAbstractView>()->setTimeSpan( timeSpan );
        return &parentItem.nodeAs<GeoDataAbstractView>()->timeSpan();
    }

    return 0;
}

}
}
}
