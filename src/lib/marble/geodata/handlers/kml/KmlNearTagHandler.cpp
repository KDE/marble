//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlNearTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoDataViewVolume.h"
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( near )

GeoNode* KmlnearTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_near ) );

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents( kmlTag_ViewVolume ))
    {
        qreal near = parser.readElementText().toDouble();

        parentItem.nodeAs<GeoDataViewVolume>()->setNear( near );
    }
    return 0;
}

}
}
