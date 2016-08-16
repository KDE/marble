//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlRightFovTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoDataViewVolume.h"
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( rightFov )

GeoNode* KmlrightFovTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_rightFov)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents( kmlTag_ViewVolume ))
    {
        qreal rightFov = parser.readElementText().toDouble();

        parentItem.nodeAs<GeoDataViewVolume>()->setRightFov( rightFov );
    }
    return 0;
}

}
}
