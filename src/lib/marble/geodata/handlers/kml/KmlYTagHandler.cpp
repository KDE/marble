// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "KmlYTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoDataScale.h"


namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( y )

GeoNode* KmlyTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_y)));

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.is<GeoDataScale>() ){
        double y = parser.readElementText().trimmed().toDouble();
	parentItem.nodeAs<GeoDataScale>()->setY( y );
    }
    return nullptr;
}

}
}
