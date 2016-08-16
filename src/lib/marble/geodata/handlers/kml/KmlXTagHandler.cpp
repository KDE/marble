//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "KmlXTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoDataScale.h"


namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( x )

GeoNode* KmlxTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_x)));

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.is<GeoDataScale>() ){
        double x = parser.readElementText().trimmed().toDouble();
	parentItem.nodeAs<GeoDataScale>()->setX( x );
    }
    return 0;
}

}
}
