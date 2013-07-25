//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Harshit Jain <hjain.itbhu@gmail.com>
//

#include "KmlBeginTagHandler.h"

#include "MarbleDebug.h"
#include <QDateTime> 

#include "KmlWhenTagHandler.h"
#include "KmlElementDictionary.h"
#include "GeoDataTimeSpan.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( begin )

GeoNode* KmlbeginTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_begin ) );

    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.represents( kmlTag_TimeSpan ) ) {
        QString beginString = parser.readElementText().trimmed();
        QDateTime begin = KmlwhenTagHandler::parse( beginString );
        parentItem.nodeAs<GeoDataTimeSpan>()->setBegin( begin );
    }

    return 0;
}

}
}
