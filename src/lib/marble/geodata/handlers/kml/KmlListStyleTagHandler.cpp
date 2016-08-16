//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlListStyleTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataListStyle.h"
#include "GeoDataStyle.h"
#include "GeoDataParser.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( ListStyle )

GeoNode* KmlListStyleTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_ListStyle)));

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.represents( kmlTag_Style ) ) {
        GeoDataListStyle style;
        KmlObjectTagHandler::parseIdentifiers( parser, &style );

        parentItem.nodeAs<GeoDataStyle>()->setListStyle( style );
        return &parentItem.nodeAs<GeoDataStyle>()->listStyle();
    }
    return 0;
}

}
}
