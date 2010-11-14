//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Harshit Jain <hjain.itbhu@gmail.com>
//

#include "KmlDataTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"

#include "GeoDataExtendedData.h"
#include "GeoDataData.h"
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( Data )

GeoNode* KmlDataTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_Data ) );

    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.represents( kmlTag_ExtendedData ) ) {
	GeoDataData data;
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << kmlTag_Data << ">"
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
        
        QString name = parser.attribute( "name" ).trimmed();
        data.setName( name );
        QString displayName = parser.attribute( "displayName" ).trimmed();
        data.setDisplayName( displayName );
#ifdef DEBUG_TAGS
        mDebug() << "attribute 'name':" << name;
#endif // DEBUG_TAGS
        parentItem.nodeAs< GeoDataExtendedData >()->addValue( data );
        return static_cast<GeoDataData*>( &parentItem.nodeAs<GeoDataExtendedData>()->valueRef( name ) );
    } else {
        return 0;
    }

}

}
}
