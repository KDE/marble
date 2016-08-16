//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010          Harshit Jain <hjain.itbhu@gmail.com>
//

#include "KmlDisplayNameTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataData.h"
#include "GeoDataSimpleField.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( displayName )

GeoNode* KmldisplayNameTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_displayName)));

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.represents( kmlTag_Data ) ) {
        QString displayName = parser.readElementText().trimmed();
        parentItem.nodeAs<GeoDataData>()->setDisplayName( displayName );
    }
    else if ( parentItem.represents( kmlTag_SimpleField ) ) {
        QString displayName = parser.readElementText().trimmed();
        parentItem.nodeAs<GeoDataSimpleField>()->setDisplayName( displayName );
    }
    return 0;
}

}
}
