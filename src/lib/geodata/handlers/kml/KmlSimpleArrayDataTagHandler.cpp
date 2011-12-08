//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Niko Sams <niko.sams@gmail.com>
//

#include "KmlSimpleArrayDataTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"

#include "GeoDataExtendedData.h"
#include "GeoDataSimpleArrayData.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER_GX22( SimpleArrayData )

GeoNode* KmlSimpleArrayDataTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_SimpleArrayData ) );

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.is<GeoDataExtendedData>() ) {
        GeoDataSimpleArrayData *arrayData = new GeoDataSimpleArrayData();
        QString name = parser.attribute( "name" ).trimmed();
        parentItem.nodeAs<GeoDataExtendedData>()->setSimpleArrayData( name, arrayData );
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << kmlTag_SimpleArrayData << ">"
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
        return arrayData;
    }

    return 0;
}

}
}
