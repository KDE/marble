//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Harshit Jain <hjain.itbhu@gmail.com>
//

#include "KmlExtendedDataTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataExtendedData.h"
#include "GeoDataFeature.h"

#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( ExtendedData )

GeoNode* KmlExtendedDataTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_ExtendedData ) );

    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.is<GeoDataFeature>() ) {
        GeoDataExtendedData extendedData;
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << kmlTag_ExtendedData << ">"
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
        parentItem.nodeAs<GeoDataFeature>()->setExtendedData( extendedData );
        return static_cast<GeoDataExtendedData*>( &parentItem.nodeAs<GeoDataFeature>()->extendedData() );
    } else {
        return 0;
    }

}

}
}
