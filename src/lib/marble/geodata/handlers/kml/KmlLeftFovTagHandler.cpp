//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlLeftFovTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoDataViewVolume.h"
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( leftFov )

GeoNode* KmlleftFovTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_leftFov)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents( kmlTag_ViewVolume ))
    {
        qreal leftFov = parser.readElementText().toDouble();

        parentItem.nodeAs<GeoDataViewVolume>()->setLeftFov( leftFov );
    }
    return 0;
}

}
}
