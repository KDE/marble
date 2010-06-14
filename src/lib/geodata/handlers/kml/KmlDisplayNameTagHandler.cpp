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
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( displayName )

GeoNode* KmldisplayNameTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_displayName ) );

    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.represents( kmlTag_Data ) ) {
        QString displayName = parser.readElementText().trimmed();
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << kmlTag_displayName << "> containing: " << displayName
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
        parentItem.nodeAs<GeoDataData>()->setDisplayName( displayName );
    }
    return 0;
}

}
}
